#include <gtest/gtest.h>

#include <stdexcept>

#include "Core/AnaliseOctree.h"
#include "Core/Cilindro.h"

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_rejeitar_raio_ou_altura_nao_positivos)
{
  EXPECT_THROW(TCilindro({ 0.0, 0.0, 0.0 }, 0.0, 1.0), std::invalid_argument);
  EXPECT_THROW(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, -1.0), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_classificar_regioes_vazias_cheias_e_parciais)
{
  const TClassificadorCilindroOctree classificador(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, 1.0));

  EXPECT_EQ(classificador.Classifica(TCubo({ 0.0, 0.0, 0.0 }, 0.5)), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(classificador.Classifica(TCubo({ 0.8, 0.0, 0.0 }, 0.2)), EEstadoNoOctree::VAZIO);
  EXPECT_EQ(classificador.Classifica(TCubo({ 0.0, 0.8, 0.0 }, 0.2)), EEstadoNoOctree::VAZIO);
  EXPECT_EQ(classificador.Classifica(TCubo({ 0.5, 0.0, 0.0 }, 0.4)), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(classificador.Classifica(TCubo({ 0.0, 0.5, 0.0 }, 0.4)), EEstadoNoOctree::PARCIAL);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_tratar_contato_apenas_pela_fronteira_como_regiao_vazia)
{
  const TClassificadorCilindroOctree classificador(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, 1.0));

  EXPECT_EQ(classificador.Classifica(TCubo({ 0.0, 0.75, 0.0 }, 0.5)), EEstadoNoOctree::VAZIO);
  EXPECT_EQ(classificador.Classifica(TCubo({ 0.75, 0.0, 0.0 }, 0.5)), EEstadoNoOctree::VAZIO);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_classificar_uma_quina_fora_do_disco_como_vazia)
{
  const TClassificadorCilindroOctree classificador(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, 1.0));

  // A célula está dentro do quadrado que envolve o disco, mas toda fora dele.
  EXPECT_EQ(classificador.Classifica(TCubo({ 0.45, 0.0, 0.45 }, 0.1)), EEstadoNoOctree::VAZIO);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_respeitar_o_eixo_escolhido)
{
  const TCubo regiaoAlongadaEmX({ 0.8, 0.0, 0.0 }, 0.2);

  EXPECT_EQ(TClassificadorCilindroOctree(TCilindro({ 0.0, 0.0, 0.0 }, 0.3, 2.0, EEixo::Y)).Classifica(regiaoAlongadaEmX), EEstadoNoOctree::VAZIO);
  EXPECT_EQ(TClassificadorCilindroOctree(TCilindro({ 0.0, 0.0, 0.0 }, 0.3, 2.0, EEixo::X)).Classifica(regiaoAlongadaEmX), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(TClassificadorCilindroOctree(TCilindro({ 0.0, 0.0, 0.0 }, 0.3, 2.0, EEixo::Z)).Classifica(regiaoAlongadaEmX), EEstadoNoOctree::VAZIO);
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_aproximar_o_volume_com_a_profundidade)
{
  const TClassificadorCilindroOctree classificador(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, 1.0));
  TOctree grossa(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 5));
  TOctree fina(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 7));
  grossa.Constroi(classificador);
  fina.Constroi(classificador);

  // pi r² h = 0,785...; a célula terminal cheia superestima o volume, cada vez menos.
  const TAnaliseOctree analise;
  EXPECT_GT(analise.CalculaVolume(fina), 0.785);
  EXPECT_LT(analise.CalculaVolume(fina), analise.CalculaVolume(grossa));
}

//----------------------------------------------------------------------------------------------

TEST(CilindroTest, deve_rejeitar_um_cilindro_fora_do_dominio)
{
  TOctree octree;

  EXPECT_THROW(octree.Constroi(TClassificadorCilindroOctree(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, 2.5))), std::invalid_argument);
  EXPECT_THROW(octree.Constroi(TClassificadorCilindroOctree(TCilindro({ 0.8, 0.0, 0.0 }, 0.5, 1.0))), std::invalid_argument);
  EXPECT_NO_THROW(octree.Constroi(TClassificadorCilindroOctree(TCilindro({ 0.0, 0.0, 0.0 }, 1.0, 2.0, EEixo::Z))));
}

//----------------------------------------------------------------------------------------------
