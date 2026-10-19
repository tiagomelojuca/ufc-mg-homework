#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "Core/Esfera.h"

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_rejeitar_raio_invalido)
{
  EXPECT_THROW(TEsfera(0.0), std::invalid_argument);
  EXPECT_THROW(TEsfera(-1.0), std::invalid_argument);
  EXPECT_THROW(TEsfera(std::numeric_limits<double>::infinity()), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_ser_centrada_na_origem)
{
  const TEsfera esfera(1.0);

  EXPECT_EQ(esfera.ClassificaPonto({ 0.0, 0.0, 0.0 }), EClassificacao::IN);
  EXPECT_EQ(esfera.ClassificaPonto({ 1.0, 0.0, 0.0 }), EClassificacao::ON);
  EXPECT_EQ(esfera.ClassificaPonto({ 0.0, -1.0, 0.0 }), EClassificacao::ON);
  EXPECT_EQ(esfera.ClassificaPonto({ 0.0, 0.0, 1.5 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_aresta_que_atravessa_a_esfera)
{
  const TEsfera esfera(1.0);

  const TClassificacaoAresta resultado = esfera.ClassificaAresta(
    TAresta({ -2.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_EQ(resultado[2].classificacao, EClassificacao::OUT);
  EXPECT_DOUBLE_EQ(resultado[0].tInicio, 0.0);
  EXPECT_DOUBLE_EQ(resultado[1].tInicio, 0.25);
  EXPECT_DOUBLE_EQ(resultado[1].tFim, 0.75);
  EXPECT_DOUBLE_EQ(resultado[2].tFim, 1.0);
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_como_fora_aresta_sem_intersecao)
{
  const TEsfera esfera(1.0);

  const TClassificacaoAresta resultado = esfera.ClassificaAresta(
    TAresta({ -2.0, 2.0, 0.0 }, { 2.0, 2.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 1u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_como_fora_aresta_tangente)
{
  const TEsfera esfera(1.0);

  const TClassificacaoAresta resultado = esfera.ClassificaAresta(
    TAresta({ -2.0, 1.0, 0.0 }, { 2.0, 1.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 1u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_aresta_que_comeca_dentro)
{
  const TEsfera esfera(1.0);

  const TClassificacaoAresta resultado = esfera.ClassificaAresta(
    TAresta({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 2.0 })
  );

  ASSERT_EQ(resultado.size(), 2u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::IN);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::OUT);
  EXPECT_DOUBLE_EQ(resultado[0].tFim, 0.5);
}

//----------------------------------------------------------------------------------------------

TEST(EsferaTest, deve_classificar_como_dentro_aresta_interna)
{
  const TEsfera esfera(2.0);

  const TClassificacaoAresta resultado = esfera.ClassificaAresta(
    TAresta({ -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 1u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::IN);
}

//----------------------------------------------------------------------------------------------
