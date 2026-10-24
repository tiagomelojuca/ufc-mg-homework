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

  TArvoreCSG CriaBloco(
    double lado
  )
  {
    return CriaPrimitiva(std::make_shared<TBloco>(lado, lado, lado));
  }

  TArvoreCSG CriaEsfera(
    double raio
  )
  {
    return CriaPrimitiva(std::make_shared<TEsfera>(raio));
  }

  TArvoreCSG Escalada(
    const TArvoreCSG& arvore,
    const TVetor3D& fatores
  )
  {
    return CriaEscala(TEscala(fatores), arvore);
  }

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
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_rejeitar_fatores_invalidos)
{
  const double infinito = std::numeric_limits<double>::infinity();
  const double indefinido = std::numeric_limits<double>::quiet_NaN();

  EXPECT_THROW(TEscala({ 0.0, 1.0, 1.0 }), std::invalid_argument);
  EXPECT_THROW(TEscala({ 1.0, 0.0, 1.0 }), std::invalid_argument);
  EXPECT_THROW(TEscala({ 1.0, 1.0, infinito }), std::invalid_argument);
  EXPECT_THROW(TEscala({ indefinido, 1.0, 1.0 }), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_criar_no_de_escala_com_um_filho)
{
  const TArvoreCSG bloco = CriaBloco(2.0);

  const TArvoreCSG escalado = Escalada(bloco, { 2.0, 3.0, 4.0 });

  EXPECT_EQ(escalado->Tipo(), ETipoNoCSG::ESCALA);
  EXPECT_DOUBLE_EQ(escalado->Escala().Fatores().x, 2.0);
  EXPECT_DOUBLE_EQ(escalado->Escala().Fatores().y, 3.0);
  EXPECT_DOUBLE_EQ(escalado->Escala().Fatores().z, 4.0);
  EXPECT_EQ(escalado->Filho(), bloco);
  EXPECT_THROW(CriaEscala(TEscala({ 1.0, 1.0, 1.0 }), nullptr), std::invalid_argument);
  EXPECT_THROW(bloco->Escala(), std::logic_error);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_levar_o_ponto_ao_sistema_do_filho)
{
  const TEscala escala({ 2.0, 4.0, 5.0 });

  const TVetor3D ponto = escala.LevaAoSistemaDoFilho(TVetor3D { 2.0, 4.0, 10.0 });

  EXPECT_NEAR(ponto.x, 1.0, TOLERANCIA);
  EXPECT_NEAR(ponto.y, 1.0, TOLERANCIA);
  EXPECT_NEAR(ponto.z, 2.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_transformar_a_esfera_em_elipsoide)
{
  const TArvoreCSG elipsoide = Escalada(CriaEsfera(1.0), { 2.0, 1.0, 1.0 });

  EXPECT_EQ(ClassificaPonto(elipsoide, { 1.5, 0.0, 0.0 }), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(elipsoide, { 2.0, 0.0, 0.0 }), EClassificacao::ON);
  EXPECT_EQ(ClassificaPonto(elipsoide, { 2.5, 0.0, 0.0 }), EClassificacao::OUT);
  EXPECT_EQ(ClassificaPonto(elipsoide, { 0.0, 1.5, 0.0 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_preservar_o_parametro_da_aresta)
{
  const TArvoreCSG elipsoide = Escalada(CriaEsfera(1.0), { 2.0, 1.0, 1.0 });

  // O elipsoide ocupa x em [-2, 2]; a aresta vai de -3 a 3 (comprimento 6).
  const TClassificacaoAresta resultado = ClassificaAresta(elipsoide, TAresta({ -3.0, 0.0, 0.0 }, { 3.0, 0.0, 0.0 }));

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 1.0 / 6.0, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 5.0 / 6.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_compor_escala_e_translacao_na_ordem_dos_nos)
{
  // O no externo e aplicado por ultimo: aqui o bloco e escalado e depois deslocado.
  const TArvoreCSG escalaDepoisTranslacao = Transladada(Escalada(CriaBloco(2.0), { 2.0, 1.0, 1.0 }), { 5.0, 0.0, 0.0 });

  // O bloco escalado ocupa x em [-2, 2]; deslocado, x em [3, 7]; a aresta vai de 2 a 8.
  const TClassificacaoAresta resultado = ClassificaAresta(
    escalaDepoisTranslacao,
    TAresta({ 2.0, 0.0, 0.0 }, { 8.0, 0.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 1.0 / 6.0, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 5.0 / 6.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(EscalaTest, deve_dar_resultados_diferentes_conforme_a_ordem_dos_nos)
{
  // Translada 1 e escala por 2: bloco em x [0, 4].
  const TArvoreCSG transladaDepoisEscala = Escalada(Transladada(CriaBloco(2.0), { 1.0, 0.0, 0.0 }), { 2.0, 1.0, 1.0 });
  // Escala por 2 e translada 1: bloco em x [-1, 3].
  const TArvoreCSG escalaDepoisTranslacao = Transladada(Escalada(CriaBloco(2.0), { 2.0, 1.0, 1.0 }), { 1.0, 0.0, 0.0 });

  EXPECT_EQ(ClassificaPonto(transladaDepoisEscala, { 3.5, 0.0, 0.0 }), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(escalaDepoisTranslacao, { 3.5, 0.0, 0.0 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------
