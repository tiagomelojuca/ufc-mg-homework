#include <gtest/gtest.h>

#include <stdexcept>

#include "Core/Cilindro.h"

namespace
{
  constexpr double TOLERANCIA = 1e-12;

  //--------------------------------------------------------------------------------------------

  void EsperaVetor(
    const TVetor3D& obtido,
    const TVetor3D& esperado
  )
  {
    EXPECT_NEAR(obtido.x, esperado.x, TOLERANCIA);
    EXPECT_NEAR(obtido.y, esperado.y, TOLERANCIA);
    EXPECT_NEAR(obtido.z, esperado.z, TOLERANCIA);
  }
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_rejeitar_dimensoes_invalidas)
{
  EXPECT_THROW(TCilindro(0.0, 1.0), std::invalid_argument);
  EXPECT_THROW(TCilindro(1.0, -1.0), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_ter_eixo_z_e_base_em_z_zero)
{
  const TCilindro cilindro(1.0, 2.0);

  EXPECT_EQ(cilindro.ClassificaPonto({ 0.0, 0.0, 1.0 }), EClassificacao::IN);
  EXPECT_EQ(cilindro.ClassificaPonto({ 1.0, 0.0, 1.0 }), EClassificacao::ON);
  EXPECT_EQ(cilindro.ClassificaPonto({ 0.0, 0.0, 0.0 }), EClassificacao::ON);
  EXPECT_EQ(cilindro.ClassificaPonto({ 0.0, 0.0, 2.0 }), EClassificacao::ON);
  EXPECT_EQ(cilindro.ClassificaPonto({ 0.0, 0.0, -0.1 }), EClassificacao::OUT);
  EXPECT_EQ(cilindro.ClassificaPonto({ 0.8, 0.8, 1.0 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_classificar_aresta_que_atravessa_a_superficie_lateral)
{
  const TCilindro cilindro(1.0, 2.0);

  const TClassificacaoAresta resultado = cilindro.ClassificaAresta(
    TAresta({ -2.0, 0.0, 1.0 }, { 2.0, 0.0, 1.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 0.25, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 0.75, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_classificar_aresta_que_atravessa_as_bases)
{
  const TCilindro cilindro(1.0, 2.0);

  const TClassificacaoAresta resultado = cilindro.ClassificaAresta(
    TAresta({ 0.0, 0.0, -1.0 }, { 0.0, 0.0, 3.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 0.25, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 0.75, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_classificar_aresta_obliqua_que_sai_pela_lateral)
{
  const TCilindro cilindro(1.0, 2.0);

  // Parte de dentro e sai pela lateral em x = 1, na metade do caminho.
  const TClassificacaoAresta resultado = cilindro.ClassificaAresta(
    TAresta({ 0.0, 0.0, 0.5 }, { 2.0, 0.0, 2.5 })
  );

  ASSERT_EQ(resultado.size(), 2u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[0].tFim, 0.5, TOLERANCIA);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_dar_a_normal_radial_para_aresta_sobre_a_lateral)
{
  const TCilindro cilindro(1.0, 2.0);

  const TClassificacaoAresta resultado = cilindro.ClassificaAresta(
    TAresta({ 0.0, 1.0, -1.0 }, { 0.0, 1.0, 3.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::ON);
  EXPECT_NEAR(resultado[1].tInicio, 0.25, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 0.75, TOLERANCIA);
  ASSERT_EQ(resultado[1].vizinhanca.size(), 1u);
  ASSERT_EQ(resultado[1].vizinhanca[0].size(), 1u);
  EsperaVetor(resultado[1].vizinhanca[0][0], { 0.0, 1.0, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_dar_a_normal_da_base_para_aresta_sobre_ela)
{
  const TCilindro cilindro(1.0, 2.0);

  const TClassificacaoAresta resultado = cilindro.ClassificaAresta(
    TAresta({ -2.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 })
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::ON);
  ASSERT_EQ(resultado[1].vizinhanca.size(), 1u);
  EsperaVetor(resultado[1].vizinhanca[0][0], { 0.0, 0.0, -1.0 });
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_classificar_como_fora_aresta_tangente_ou_paralela_ao_eixo_por_fora)
{
  const TCilindro cilindro(1.0, 2.0);

  const TClassificacaoAresta tangente =
    cilindro.ClassificaAresta(TAresta({ 1.0, -2.0, 1.0 }, { 1.0, 2.0, 1.0 }));
  const TClassificacaoAresta paralela =
    cilindro.ClassificaAresta(TAresta({ 2.0, 0.0, -1.0 }, { 2.0, 0.0, 3.0 }));

  ASSERT_EQ(tangente.size(), 1u);
  EXPECT_EQ(tangente[0].classificacao, EClassificacao::OUT);
  ASSERT_EQ(paralela.size(), 1u);
  EXPECT_EQ(paralela[0].classificacao, EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_dar_a_normal_da_superficie_mais_proxima)
{
  const TCilindro cilindro(1.0, 2.0);

  EsperaVetor(cilindro.Normal({ 0.6, 0.8, 1.0 }), { 0.6, 0.8, 0.0 });
  EsperaVetor(cilindro.Normal({ 0.2, 0.1, 2.0 }), { 0.0, 0.0, 1.0 });
  EsperaVetor(cilindro.Normal({ 0.3, 0.2, 0.0 }), { 0.0, 0.0, -1.0 });
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_escalar_raio_por_x_e_y_e_altura_por_z)
{
  const TCilindro cilindro(1.0, 2.0);

  const auto escalado = std::dynamic_pointer_cast<const TCilindro>(cilindro.Escalada({ 2.0, 2.0, 3.0 }));

  ASSERT_NE(escalado, nullptr);
  EXPECT_DOUBLE_EQ(escalado->Raio(), 2.0);
  EXPECT_DOUBLE_EQ(escalado->Altura(), 6.0);
  EXPECT_THROW(cilindro.Escalada({ 2.0, 1.0, 1.0 }), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_ter_a_caixa_canonica_da_base_ao_topo)
{
  const TCaixa caixa = TCilindro(1.5, 2.0).CaixaCanonica();

  EsperaVetor(caixa.minimo, { -1.5, -1.5, 0.0 });
  EsperaVetor(caixa.maximo, { 1.5, 1.5, 2.0 });
}

//----------------------------------------------------------------------------------------------
