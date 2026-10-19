#include <gtest/gtest.h>

#include "Core/ArvoreCSG.h"
#include "Core/Esfera.h"

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

  void ExpectIdentidade(
    const TMatriz4x4& matriz
  )
  {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        EXPECT_NEAR(matriz.Elemento(i, j), i == j ? 1.0 : 0.0, TOLERANCIA);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------

TEST(MovimentoRigidoTest, deve_compor_a_rotacao_na_ordem_x_y_z)
{
  const TMovimentoRigido rotacao(ETipoMovimentoRigido::ROTACAO, { 90.0, 90.0, 0.0 });

  // Rx(90) leva (0, 1, 0) a (0, 0, 1); em seguida, Ry(90) leva (0, 0, 1) a (1, 0, 0).
  ExpectPontoProximo(rotacao.Matriz().TransformaPonto({ 0.0, 1.0, 0.0 }), { 1.0, 0.0, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(MovimentoRigidoTest, deve_desfazer_a_translacao_com_a_inversa)
{
  const TMovimentoRigido translacao(ETipoMovimentoRigido::TRANSLACAO, { 1.0, -2.0, 3.0 });

  ExpectIdentidade(translacao.Matriz() * translacao.MatrizInversa());
  ExpectIdentidade(translacao.MatrizInversa() * translacao.Matriz());
}

//----------------------------------------------------------------------------------------------

TEST(MovimentoRigidoTest, deve_desfazer_a_rotacao_com_a_inversa)
{
  const TMovimentoRigido rotacao(ETipoMovimentoRigido::ROTACAO, { 30.0, -45.0, 120.0 });

  ExpectIdentidade(rotacao.Matriz() * rotacao.MatrizInversa());
  ExpectIdentidade(rotacao.MatrizInversa() * rotacao.Matriz());
}

//----------------------------------------------------------------------------------------------

TEST(MovimentoRigidoTest, deve_levar_o_ponto_ao_sistema_do_filho)
{
  const TMovimentoRigido translacao(ETipoMovimentoRigido::TRANSLACAO, { 2.0, 0.0, 0.0 });
  const TEsfera esfera(1.0);

  const TVetor3D pontoNoFilho = translacao.LevaAoSistemaDoFilho(TVetor3D { 2.0, 0.0, 0.0 });

  ExpectPontoProximo(pontoNoFilho, { 0.0, 0.0, 0.0 });
  EXPECT_EQ(esfera.ClassificaPonto(pontoNoFilho), EClassificacao::IN);
}

//----------------------------------------------------------------------------------------------

TEST(MovimentoRigidoTest, deve_preservar_o_parametro_da_aresta_no_sistema_do_filho)
{
  const TMovimentoRigido translacao(ETipoMovimentoRigido::TRANSLACAO, { 2.0, 0.0, 0.0 });
  const TEsfera esfera(1.0);

  const TClassificacaoAresta resultado = esfera.ClassificaAresta(
    translacao.LevaAoSistemaDoFilho(TAresta({ 0.0, 0.0, 0.0 }, { 4.0, 0.0, 0.0 }))
  );

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 0.25, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 0.75, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(MovimentoRigidoTest, deve_levar_a_aresta_rotacionada_ao_sistema_do_filho)
{
  const TMovimentoRigido rotacao(ETipoMovimentoRigido::ROTACAO, { 0.0, 0.0, 90.0 });

  const TAresta arestaNoFilho = rotacao.LevaAoSistemaDoFilho(
    TAresta({ 0.0, 1.0, 0.0 }, { 0.0, 2.0, 0.0 })
  );

  ExpectPontoProximo(arestaNoFilho.Inicio(), { 1.0, 0.0, 0.0 });
  ExpectPontoProximo(arestaNoFilho.Fim(), { 2.0, 0.0, 0.0 });
}

//----------------------------------------------------------------------------------------------
