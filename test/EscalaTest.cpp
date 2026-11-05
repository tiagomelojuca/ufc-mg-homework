#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <stdexcept>

#include "Core/ArvoreCSG.h"
#include "Core/Bloco.h"
#include "Core/ClassificadorCSG.h"
#include "Core/Esfera.h"

namespace
{
  constexpr double TOLERANCIA = 1e-12;

  //--------------------------------------------------------------------------------------------

  TArvoreCSG CriaBloco(
    double lado
  )
  {
    return CriaPrimitiva(std::make_shared<TBloco>(lado, lado, lado));
  }

  //--------------------------------------------------------------------------------------------

  TArvoreCSG CriaEsfera(
    double raio
  )
  {
    return CriaPrimitiva(std::make_shared<TEsfera>(raio));
  }

  //--------------------------------------------------------------------------------------------

  TArvoreCSG Transladada(
    const TArvoreCSG& arvore,
    const TVetor3D& deslocamento
  )
  {
    return CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, deslocamento),
      arvore
    );
  }

  //--------------------------------------------------------------------------------------------

  const TBloco& BlocoDaFolha(
    const TArvoreCSG& folha
  )
  {
    return dynamic_cast<const TBloco&>(folha->Primitiva());
  }
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_multiplicar_os_lados_do_bloco_sem_criar_no)
{
  const TArvoreCSG escalado = Escala(CriaBloco(2.0), { 2.0, 3.0, 4.0 });

  ASSERT_EQ(escalado->Tipo(), ETipoNoCSG::PRIMITIVA);
  EXPECT_DOUBLE_EQ(BlocoDaFolha(escalado).LadoX(), 4.0);
  EXPECT_DOUBLE_EQ(BlocoDaFolha(escalado).LadoY(), 6.0);
  EXPECT_DOUBLE_EQ(BlocoDaFolha(escalado).LadoZ(), 8.0);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_multiplicar_o_raio_da_esfera)
{
  const TArvoreCSG escalada = Escala(CriaEsfera(1.5), { 2.0, 2.0, 2.0 });

  ASSERT_EQ(escalada->Tipo(), ETipoNoCSG::PRIMITIVA);
  EXPECT_DOUBLE_EQ(dynamic_cast<const TEsfera&>(escalada->Primitiva()).Raio(), 3.0);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_rejeitar_escala_nao_uniforme_da_esfera)
{
  EXPECT_THROW(Escala(CriaEsfera(1.0), { 2.0, 1.0, 1.0 }), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_rejeitar_fatores_invalidos)
{
  const double infinito = std::numeric_limits<double>::infinity();
  const double indefinido = std::numeric_limits<double>::quiet_NaN();
  const TArvoreCSG bloco = CriaBloco(2.0);

  EXPECT_THROW(Escala(bloco, { 0.0, 1.0, 1.0 }), std::invalid_argument);
  EXPECT_THROW(Escala(bloco, { 1.0, -2.0, 1.0 }), std::invalid_argument);
  EXPECT_THROW(Escala(bloco, { 1.0, 1.0, infinito }), std::invalid_argument);
  EXPECT_THROW(Escala(bloco, { indefinido, 1.0, 1.0 }), std::invalid_argument);
  EXPECT_THROW(Escala(nullptr, { 1.0, 1.0, 1.0 }), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_preservar_a_arvore_original)
{
  const TArvoreCSG bloco = CriaBloco(2.0);

  const TArvoreCSG escalado = Escala(bloco, { 3.0, 3.0, 3.0 });

  EXPECT_NE(escalado, bloco);
  EXPECT_DOUBLE_EQ(BlocoDaFolha(bloco).LadoX(), 2.0);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_manter_os_movimentos_acima_da_primitiva)
{
  const TArvoreCSG movido = Transladada(CriaBloco(2.0), { 5.0, 0.0, 0.0 });

  const TArvoreCSG escalado = Escala(movido, { 2.0, 1.0, 1.0 });

  ASSERT_EQ(escalado->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
  EXPECT_EQ(escalado->Movimento().Tipo(), ETipoMovimentoRigido::TRANSLACAO);
  EXPECT_DOUBLE_EQ(escalado->Movimento().Valores().x, 5.0);
  ASSERT_EQ(escalado->Filho()->Tipo(), ETipoNoCSG::PRIMITIVA);
  EXPECT_DOUBLE_EQ(BlocoDaFolha(escalado->Filho()).LadoX(), 4.0);

  // O bloco 4 x 2 x 2 deslocado ocupa x em [3, 7]; a aresta vai de 2 a 8.
  const TClassificacaoAresta resultado = ClassificaAresta(
    escalado,
    TAresta({ 2.0, 0.0, 0.0 }, { 8.0, 0.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 1.0 / 6.0, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 5.0 / 6.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_escalar_nos_eixos_canonicos_da_primitiva)
{
  // Girado 90 graus em z, o eixo x canônico do bloco fica ao longo de y.
  const TArvoreCSG girado = CriaMovimentoRigido(
    TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, { 0.0, 0.0, 90.0 }),
    CriaBloco(2.0)
  );

  const TArvoreCSG escalado = Escala(girado, { 2.0, 1.0, 1.0 });

  EXPECT_EQ(ClassificaPonto(escalado, { 0.0, 1.5, 0.0 }), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(escalado, { 0.0, 2.0, 0.0 }), EClassificacao::ON);
  EXPECT_EQ(ClassificaPonto(escalado, { 1.5, 0.0, 0.0 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_manter_o_deslocamento_sem_escala)
{
  // A escala muda só os lados do bloco; o deslocamento acima dele continua 1, e não 2.
  const TArvoreCSG escalado = Escala(Transladada(CriaBloco(2.0), { 1.0, 0.0, 0.0 }), { 2.0, 1.0, 1.0 });

  // O bloco 4 x 2 x 2 deslocado de 1 ocupa x em [-1, 3].
  EXPECT_EQ(ClassificaPonto(escalado, { -0.5, 0.0, 0.0 }), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(escalado, { 3.5, 0.0, 0.0 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_rejeitar_escala_de_operacao_booleana)
{
  const TArvoreCSG uniao = CriaOperacaoBooleana(
    EOperacaoBooleana::UNIAO,
    CriaBloco(2.0),
    Transladada(CriaEsfera(1.0), { 2.0, 0.0, 0.0 })
  );

  EXPECT_THROW(Escala(uniao, { 2.0, 2.0, 2.0 }), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------
