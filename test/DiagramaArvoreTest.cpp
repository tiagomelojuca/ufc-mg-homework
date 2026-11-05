#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

#include "Application/DiagramaArvore.h"
#include "Core/Bloco.h"
#include "Core/Cilindro.h"
#include "Core/Esfera.h"

namespace
{
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

  TArvoreCSG Uniao(
    const TArvoreCSG& esquerda,
    const TArvoreCSG& direita
  )
  {
    return CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, esquerda, direita);
  }
}

//----------------------------------------------------------------------------------------------

TEST(DiagramaArvoreTest, deve_ter_um_unico_no_para_uma_primitiva)
{
  const TDiagramaArvore diagrama = MontaDiagrama(CriaBloco(2.0));

  ASSERT_EQ(diagrama.nos.size(), 1u);
  EXPECT_EQ(diagrama.colunas, 1);
  EXPECT_EQ(diagrama.niveis, 1);
  EXPECT_EQ(diagrama.nos[0].rotulo.titulo, "Bloco");
  EXPECT_EQ(diagrama.nos[0].rotulo.detalhe, "2 x 2 x 2");
  EXPECT_EQ(diagrama.nos[0].nivel, 0);
  EXPECT_DOUBLE_EQ(diagrama.nos[0].coluna, 0.0);
  EXPECT_FALSE(diagrama.nos[0].pai.has_value());
}

//----------------------------------------------------------------------------------------------

TEST(DiagramaArvoreTest, deve_listar_os_nos_em_pre_ordem_com_nivel_e_pai)
{
  const TDiagramaArvore diagrama =
    MontaDiagrama(Uniao(CriaBloco(2.0), Transladada(CriaEsfera(1.0), { 2.0, 0.0, 0.0 })));

  ASSERT_EQ(diagrama.nos.size(), 4u);
  EXPECT_EQ(diagrama.colunas, 2);
  EXPECT_EQ(diagrama.niveis, 3);

  EXPECT_EQ(diagrama.nos[0].tipo, ETipoNoCSG::OPERACAO_BOOLEANA);
  EXPECT_EQ(diagrama.nos[0].rotulo.titulo, "União");
  EXPECT_EQ(diagrama.nos[0].rotulo.detalhe, "U");
  EXPECT_EQ(diagrama.nos[1].rotulo.titulo, "Bloco");
  EXPECT_EQ(diagrama.nos[2].rotulo.titulo, "Translação");
  EXPECT_EQ(diagrama.nos[2].rotulo.detalhe, "(2, 0, 0)");
  EXPECT_EQ(diagrama.nos[3].rotulo.titulo, "Esfera");
  EXPECT_EQ(diagrama.nos[3].rotulo.detalhe, "r = 1");

  EXPECT_EQ(diagrama.nos[1].pai, 0u);
  EXPECT_EQ(diagrama.nos[2].pai, 0u);
  EXPECT_EQ(diagrama.nos[3].pai, 2u);
  EXPECT_EQ(diagrama.nos[3].nivel, 2);
}

//----------------------------------------------------------------------------------------------

TEST(DiagramaArvoreTest, deve_centrar_cada_no_sobre_os_seus_filhos)
{
  // Três folhas nas colunas 0, 1 e 2; a união interna fica em 0,5 e a raiz entre 0,5 e 2.
  const TDiagramaArvore diagrama = MontaDiagrama(
    Uniao(Uniao(CriaBloco(1.0), CriaEsfera(1.0)), Transladada(CriaBloco(2.0), { 1.0, 0.0, 0.0 }))
  );

  ASSERT_EQ(diagrama.nos.size(), 6u);
  EXPECT_EQ(diagrama.colunas, 3);
  EXPECT_DOUBLE_EQ(diagrama.nos[0].coluna, 1.25);
  EXPECT_DOUBLE_EQ(diagrama.nos[1].coluna, 0.5);
  EXPECT_DOUBLE_EQ(diagrama.nos[2].coluna, 0.0);
  EXPECT_DOUBLE_EQ(diagrama.nos[3].coluna, 1.0);

  // O movimento rígido fica na mesma coluna do seu único filho.
  EXPECT_DOUBLE_EQ(diagrama.nos[4].coluna, 2.0);
  EXPECT_DOUBLE_EQ(diagrama.nos[5].coluna, 2.0);
}

//----------------------------------------------------------------------------------------------

TEST(DiagramaArvoreTest, deve_rotular_cilindro_rotacao_e_intersecao)
{
  const TArvoreCSG arvore = CriaOperacaoBooleana(
    EOperacaoBooleana::INTERSECAO,
    CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, { 0.0, 90.0, 0.0 }),
      CriaPrimitiva(std::make_shared<TCilindro>(0.5, 3.0))
    ),
    CriaBloco(2.0)
  );

  const TDiagramaArvore diagrama = MontaDiagrama(arvore);

  ASSERT_EQ(diagrama.nos.size(), 4u);
  EXPECT_EQ(diagrama.nos[0].rotulo.titulo, "Interseção");
  EXPECT_EQ(diagrama.nos[0].rotulo.detalhe, "I");
  EXPECT_EQ(diagrama.nos[1].rotulo.titulo, "Rotação");
  EXPECT_EQ(diagrama.nos[1].rotulo.detalhe, "(0, 90, 0) graus");
  EXPECT_EQ(diagrama.nos[2].rotulo.titulo, "Cilindro");
  EXPECT_EQ(diagrama.nos[2].rotulo.detalhe, "r = 0.5, h = 3");
}

//----------------------------------------------------------------------------------------------

TEST(DiagramaArvoreTest, deve_repetir_a_subarvore_compartilhada_em_cada_uso)
{
  const TArvoreCSG esfera = CriaEsfera(1.0);

  const TDiagramaArvore diagrama = MontaDiagrama(Uniao(esfera, Transladada(esfera, { 1.0, 0.0, 0.0 })));

  ASSERT_EQ(diagrama.nos.size(), 4u);
  EXPECT_EQ(diagrama.colunas, 2);
  EXPECT_THROW(MontaDiagrama(nullptr), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------
