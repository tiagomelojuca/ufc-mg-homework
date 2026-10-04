#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "Core/IluminacaoPhong.h"

namespace
{
  const TCorRGB BRANCO = { 1.0, 1.0, 1.0 };

  TMaterialPhong Material(
    double ambiente,
    double difuso,
    double especular,
    double brilho = 10.0
  )
  {
    TMaterialPhong material;
    material.ambiente = ambiente;
    material.difuso = difuso;
    material.especular = especular;
    material.brilho = brilho;
    return material;
  }
}

//----------------------------------------------------------------------------------------------

TEST(IluminacaoPhongTest, deve_usar_somente_a_componente_ambiente_de_costas_para_a_luz)
{
  const TIluminacaoPhong iluminacao({ 0.0, 5.0, 0.0 }, { 0.0, 0.0, 1.0 }, Material(0.2, 0.7, 0.5));
  const TCorRGB cor = iluminacao.Calcula({ 0.0, 0.0, 0.0 }, { 0.0, -1.0, 0.0 }, { 0.5, 1.0, 0.25 });

  EXPECT_DOUBLE_EQ(cor.r, 0.1);
  EXPECT_DOUBLE_EQ(cor.g, 0.2);
  EXPECT_DOUBLE_EQ(cor.b, 0.05);
}

//----------------------------------------------------------------------------------------------

TEST(IluminacaoPhongTest, deve_somar_a_difusa_proporcional_ao_cosseno_entre_normal_e_luz)
{
  const TIluminacaoPhong iluminacao({ 1.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 }, Material(0.1, 0.8, 0.0));
  const TCorRGB cor = iluminacao.Calcula({ 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, BRANCO);

  // A luz chega a 45 graus da normal.
  EXPECT_NEAR(cor.r, 0.1 + 0.8 * std::sqrt(0.5), 1e-12);
  EXPECT_DOUBLE_EQ(cor.r, cor.g);
}

//----------------------------------------------------------------------------------------------

TEST(IluminacaoPhongTest, deve_mostrar_o_brilho_especular_somente_na_direcao_refletida)
{
  const TMaterialPhong material = Material(0.0, 0.0, 1.0, 20.0);
  const TIluminacaoPhong refletindoParaObservador({ -1.0, 1.0, 0.0 }, { 1.0, 1.0, 0.0 }, material);
  const TIluminacaoPhong refletindoParaLonge({ -1.0, 1.0, 0.0 }, { -1.0, 1.0, 0.0 }, material);

  EXPECT_NEAR(refletindoParaObservador.Calcula({ 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, BRANCO).r, 1.0, 1e-12);
  EXPECT_NEAR(refletindoParaLonge.Calcula({ 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, BRANCO).r, 0.0, 1e-12);
}

//----------------------------------------------------------------------------------------------

TEST(IluminacaoPhongTest, deve_variar_com_a_posicao_do_ponto_na_mesma_face)
{
  const TIluminacaoPhong iluminacao({ 0.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 }, Material(0.0, 1.0, 0.0));

  const double abaixoDaLuz = iluminacao.Calcula({ 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, BRANCO).r;
  const double afastado = iluminacao.Calcula({ 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, BRANCO).r;
  EXPECT_DOUBLE_EQ(abaixoDaLuz, 1.0);
  EXPECT_LT(afastado, abaixoDaLuz);
}

//----------------------------------------------------------------------------------------------

TEST(IluminacaoPhongTest, deve_limitar_a_cor_e_rejeitar_vetores_nulos)
{
  const TIluminacaoPhong iluminacao({ 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }, Material(1.0, 1.0, 1.0));
  const TCorRGB cor = iluminacao.Calcula({ 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, BRANCO);

  EXPECT_DOUBLE_EQ(cor.r, 1.0);
  EXPECT_THROW(TIluminacaoPhong({ 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0 }), std::invalid_argument);
  EXPECT_THROW(iluminacao.Calcula({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, BRANCO), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------
