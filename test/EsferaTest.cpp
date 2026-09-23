#include <gtest/gtest.h>

#include <stdexcept>

#include "Core/Esfera.h"

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_rejeitar_raio_nao_positivo)
{
  EXPECT_THROW(TEsfera({ 0.0, 0.0, 0.0 }, 0.0), std::invalid_argument);
  EXPECT_THROW(TEsfera({ 0.0, 0.0, 0.0 }, -1.0), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_como_vazia_uma_regiao_fora_da_esfera)
{
  const TClassificadorEsferaOctree classificador(
    TEsfera({ 0.0, 0.0, 0.0 }, 0.5)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.75, 0.0, 0.0 }, 0.25)),
    EEstadoNoOctree::VAZIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_como_cheia_uma_regiao_dentro_da_esfera)
{
  const TClassificadorEsferaOctree classificador(
    TEsfera({ 0.0, 0.0, 0.0 }, 1.0)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.0, 0.0, 0.0 }, 1.0)),
    EEstadoNoOctree::CHEIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_como_parcial_uma_regiao_que_intercepta_a_esfera)
{
  const TClassificadorEsferaOctree classificador(
    TEsfera({ 0.0, 0.0, 0.0 }, 0.5)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.4, 0.0, 0.0 }, 0.4)),
    EEstadoNoOctree::PARCIAL
  );
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_tratar_contato_apenas_pela_fronteira_como_regiao_vazia)
{
  const TClassificadorEsferaOctree classificador(
    TEsfera({ 0.0, 0.0, 0.0 }, 0.5)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.75, 0.0, 0.0 }, 0.5)),
    EEstadoNoOctree::VAZIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_corretamente_uma_regiao_fora_na_diagonal)
{
  const TClassificadorEsferaOctree classificador(
    TEsfera({ 0.0, 0.0, 0.0 }, 1.0)
  );

  EXPECT_EQ(
    classificador.Classifica(TCubo({ 0.8, 0.8, 0.8 }, 0.1)),
    EEstadoNoOctree::VAZIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_construir_uma_esfera_no_primeiro_octante)
{
  TOctree octree(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3));
  const TClassificadorEsferaOctree classificador(
    TEsfera({ -0.5, -0.5, -0.5 }, 0.5)
  );

  octree.Constroi(classificador);

  ASSERT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::PARCIAL);
  ASSERT_EQ(octree.Raiz().Filho(0).Estado(), EEstadoNoOctree::PARCIAL);

  for (std::size_t indice = 1; indice < 8; ++indice) {
    EXPECT_EQ(octree.Raiz().Filho(indice).Estado(), EEstadoNoOctree::VAZIO);
  }

  for (std::size_t indice = 0; indice < 8; ++indice) {
    EXPECT_EQ(octree.Raiz().Filho(0).Filho(indice).Estado(), EEstadoNoOctree::CHEIO);
  }
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_rejeitar_uma_esfera_fora_do_dominio)
{
  TOctree octree;
  const TClassificadorEsferaOctree classificador(
    TEsfera({ 0.75, 0.0, 0.0 }, 0.5)
  );

  EXPECT_THROW(octree.Constroi(classificador), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------
