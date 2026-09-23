#include <gtest/gtest.h>

#include <stdexcept>

#include "Core/Bloco.h"

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_rejeitar_lados_nao_positivos)
{
  EXPECT_THROW(TBloco({ 0.0, 0.0, 0.0 }, 0.0, 1.0, 1.0), std::invalid_argument);
  EXPECT_THROW(TBloco({ 0.0, 0.0, 0.0 }, 1.0, -1.0, 1.0), std::invalid_argument);
  EXPECT_THROW(TBloco({ 0.0, 0.0, 0.0 }, 1.0, 1.0, 0.0), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_como_vazia_uma_regiao_fora_do_bloco)
{
  const TClassificadorBlocoOctree classificador(
    TBloco({ 0.0, 0.0, 0.0 }, 1.0, 1.0, 1.0)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.75, 0.0, 0.0 }, 0.25)),
    EEstadoNoOctree::VAZIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_como_cheia_uma_regiao_dentro_do_bloco)
{
  const TClassificadorBlocoOctree classificador(
    TBloco({ 0.0, 0.0, 0.0 }, 1.0, 1.0, 1.0)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.0, 0.0, 0.0 }, 0.5)),
    EEstadoNoOctree::CHEIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_como_parcial_uma_regiao_que_intercepta_o_bloco)
{
  const TClassificadorBlocoOctree classificador(
    TBloco({ 0.0, 0.0, 0.0 }, 1.0, 1.0, 1.0)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.4, 0.0, 0.0 }, 0.4)),
    EEstadoNoOctree::PARCIAL
  );
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_tratar_contato_apenas_pela_fronteira_como_regiao_vazia)
{
  const TClassificadorBlocoOctree classificador(
    TBloco({ 0.0, 0.0, 0.0 }, 1.0, 1.0, 1.0)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.75, 0.0, 0.0 }, 0.5)),
    EEstadoNoOctree::VAZIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_construir_um_bloco_alinhado_ao_primeiro_octante)
{
  TOctree octree(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3));
  const TClassificadorBlocoOctree classificador(
    TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0)
  );

  octree.Constroi(classificador);

  ASSERT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(octree.Raiz().Filho(0).Estado(), EEstadoNoOctree::CHEIO);

  for (std::size_t indice = 1; indice < 8; ++indice) {
    EXPECT_EQ(octree.Raiz().Filho(indice).Estado(), EEstadoNoOctree::VAZIO);
  }
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_representar_o_dominio_inteiro_com_uma_folha_cheia)
{
  TOctree octree;
  const TClassificadorBlocoOctree classificador(
    TBloco({ 0.0, 0.0, 0.0 }, 2.0, 2.0, 2.0)
  );

  octree.Constroi(classificador);

  EXPECT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_TRUE(octree.Raiz().EhFolha());
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_rejeitar_um_bloco_fora_do_dominio)
{
  TOctree octree;
  const TClassificadorBlocoOctree classificador(
    TBloco({ 0.75, 0.0, 0.0 }, 1.0, 1.0, 1.0)
  );

  EXPECT_THROW(octree.Constroi(classificador), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------
