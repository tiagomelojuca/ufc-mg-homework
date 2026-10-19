#include <gtest/gtest.h>

#include "Core/Matriz4x4.h"

namespace
{
  constexpr double TOLERANCIA = 1e-12;

  void ExpectPontoProximo(
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

TEST(Matriz4x4Test, deve_manter_o_ponto_com_a_identidade)
{
  ExpectPontoProximo(
    TMatriz4x4::Identidade().TransformaPonto({ 1.0, 2.0, 3.0 }),
    { 1.0, 2.0, 3.0 }
  );
}

//----------------------------------------------------------------------------------------------

TEST(Matriz4x4Test, deve_transladar_o_ponto)
{
  ExpectPontoProximo(
    TMatriz4x4::Translacao({ 1.0, -2.0, 3.0 }).TransformaPonto({ 1.0, 1.0, 1.0 }),
    { 2.0, -1.0, 4.0 }
  );
}

//----------------------------------------------------------------------------------------------

TEST(Matriz4x4Test, deve_rotacionar_pela_regra_da_mao_direita)
{
  ExpectPontoProximo(TMatriz4x4::RotacaoX(90.0).TransformaPonto({ 0.0, 1.0, 0.0 }), { 0.0, 0.0, 1.0 });
  ExpectPontoProximo(TMatriz4x4::RotacaoY(90.0).TransformaPonto({ 0.0, 0.0, 1.0 }), { 1.0, 0.0, 0.0 });
  ExpectPontoProximo(TMatriz4x4::RotacaoZ(90.0).TransformaPonto({ 1.0, 0.0, 0.0 }), { 0.0, 1.0, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(Matriz4x4Test, deve_aplicar_primeiro_a_matriz_da_direita)
{
  const TMatriz4x4 transladaDepoisRotaciona =
    TMatriz4x4::RotacaoZ(90.0) * TMatriz4x4::Translacao({ 1.0, 0.0, 0.0 });

  ExpectPontoProximo(transladaDepoisRotaciona.TransformaPonto({ 0.0, 0.0, 0.0 }), { 0.0, 1.0, 0.0 });
}

//----------------------------------------------------------------------------------------------
