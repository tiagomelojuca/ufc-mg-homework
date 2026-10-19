#include <gtest/gtest.h>

#include <stdexcept>

#include "Core/Bloco.h"

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_rejeitar_lados_invalidos)
{
  EXPECT_THROW(TBloco(0.0, 1.0, 1.0), std::invalid_argument);
  EXPECT_THROW(TBloco(1.0, -1.0, 1.0), std::invalid_argument);
  EXPECT_THROW(TBloco(1.0, 1.0, 0.0), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_ser_centrado_na_origem_com_lados_paralelos_aos_eixos)
{
  const TBloco bloco(2.0, 4.0, 6.0);

  EXPECT_EQ(bloco.ClassificaPonto({ 0.0, 0.0, 0.0 }), EClassificacao::IN);
  EXPECT_EQ(bloco.ClassificaPonto({ 1.0, 0.0, 0.0 }), EClassificacao::ON);
  EXPECT_EQ(bloco.ClassificaPonto({ 0.0, -2.0, 0.0 }), EClassificacao::ON);
  EXPECT_EQ(bloco.ClassificaPonto({ 0.0, 0.0, 3.0 }), EClassificacao::ON);
  EXPECT_EQ(bloco.ClassificaPonto({ 1.0, 2.0, 3.0 }), EClassificacao::ON);
  EXPECT_EQ(bloco.ClassificaPonto({ 1.5, 0.0, 0.0 }), EClassificacao::OUT);
  EXPECT_EQ(bloco.ClassificaPonto({ 0.0, 0.0, 3.5 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_aresta_que_atravessa_o_bloco)
{
  const TBloco bloco(2.0, 2.0, 2.0);

  const TClassificacaoAresta resultado = bloco.ClassificaAresta(
    TAresta({ -2.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_EQ(resultado[2].classificacao, EClassificacao::OUT);
  EXPECT_DOUBLE_EQ(resultado[1].tInicio, 0.25);
  EXPECT_DOUBLE_EQ(resultado[1].tFim, 0.75);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_aresta_diagonal)
{
  const TBloco bloco(2.0, 2.0, 2.0);

  const TClassificacaoAresta resultado = bloco.ClassificaAresta(
    TAresta({ -2.0, -2.0, -2.0 }, { 2.0, 2.0, 2.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_DOUBLE_EQ(resultado[1].tInicio, 0.25);
  EXPECT_DOUBLE_EQ(resultado[1].tFim, 0.75);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_como_na_fronteira_aresta_sobre_uma_face)
{
  const TBloco bloco(2.0, 2.0, 2.0);

  const TClassificacaoAresta resultado = bloco.ClassificaAresta(
    TAresta({ -2.0, 0.0, 1.0 }, { 2.0, 0.0, 1.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::ON);
  EXPECT_EQ(resultado[2].classificacao, EClassificacao::OUT);
  EXPECT_DOUBLE_EQ(resultado[1].tInicio, 0.25);
  EXPECT_DOUBLE_EQ(resultado[1].tFim, 0.75);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_como_na_fronteira_aresta_sobre_uma_aresta_do_bloco)
{
  const TBloco bloco(2.0, 2.0, 2.0);

  const TClassificacaoAresta resultado = bloco.ClassificaAresta(
    TAresta({ 1.0, 1.0, -1.0 }, { 1.0, 1.0, 1.0 })
  );

  ASSERT_EQ(resultado.size(), 1u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::ON);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_como_fora_aresta_que_toca_apenas_um_vertice)
{
  const TBloco bloco(2.0, 2.0, 2.0);

  const TClassificacaoAresta resultado = bloco.ClassificaAresta(
    TAresta({ 0.0, 2.0, 1.0 }, { 2.0, 0.0, 1.0 })
  );

  ASSERT_EQ(resultado.size(), 1u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(BlocoTest, deve_classificar_como_dentro_aresta_interna)
{
  const TBloco bloco(4.0, 4.0, 4.0);

  const TClassificacaoAresta resultado = bloco.ClassificaAresta(
    TAresta({ -1.0, 0.5, 0.0 }, { 1.0, -0.5, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 1u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::IN);
}

//----------------------------------------------------------------------------------------------
