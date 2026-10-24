#include <gtest/gtest.h>

#include <memory>

#include "Core/ArvoreCSG.h"
#include "Core/Bloco.h"
#include "Core/ClassificadorCSG.h"
#include "Core/Esfera.h"

namespace {
constexpr double TOLERANCIA = 1e-12;

TArvoreCSG CriaBloco(double lado) {
  return CriaPrimitiva(std::make_shared<TBloco>(lado, lado, lado));
}

TArvoreCSG CriaEsfera(double raio) {
  return CriaPrimitiva(std::make_shared<TEsfera>(raio));
}

TArvoreCSG Transladada(const TArvoreCSG &arvore, const TVetor3D &deslocamento) {
  return CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, deslocamento), arvore);
}

TArvoreCSG Uniao(const TArvoreCSG &esquerda, const TArvoreCSG &direita) {
  return CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, esquerda, direita);
}
} // namespace

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest, deve_seguir_a_tabela_da_uniao_para_pontos) {
  const EClassificacao IN = EClassificacao::IN;
  const EClassificacao ON = EClassificacao::ON;
  const EClassificacao OUT = EClassificacao::OUT;

  const auto uniao = [](EClassificacao a, EClassificacao b) {
    return CombinaClassificacoes(EOperacaoBooleana::UNIAO, a, b);
  };

  EXPECT_EQ(uniao(IN, IN), IN);
  EXPECT_EQ(uniao(IN, ON), IN);
  EXPECT_EQ(uniao(IN, OUT), IN);
  EXPECT_EQ(uniao(ON, IN), IN);
  EXPECT_EQ(uniao(ON, ON), ON);
  EXPECT_EQ(uniao(ON, OUT), ON);
  EXPECT_EQ(uniao(OUT, IN), IN);
  EXPECT_EQ(uniao(OUT, ON), ON);
  EXPECT_EQ(uniao(OUT, OUT), OUT);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest, deve_classificar_pontos_na_uniao) {
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaEsfera(1.0), {2.0, 0.0, 0.0}));

  EXPECT_EQ(ClassificaPonto(uniao, {0.0, 0.0, 0.0}), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(uniao, {2.0, 0.0, 0.0}), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(uniao, {-1.0, 0.0, 0.0}), EClassificacao::ON);
  EXPECT_EQ(ClassificaPonto(uniao, {3.0, 0.0, 0.0}), EClassificacao::ON);
  EXPECT_EQ(ClassificaPonto(uniao, {5.0, 0.0, 0.0}), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_fundir_trechos_dentro_na_uniao_de_solidos_que_se_tocam) {
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaEsfera(1.0), {2.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({-3.0, 0.0, 0.0}, {5.0, 0.0, 0.0}));

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_EQ(resultado[2].classificacao, EClassificacao::OUT);
  EXPECT_NEAR(resultado[0].tInicio, 0.0, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tInicio, 0.25, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 0.75, TOLERANCIA);
  EXPECT_NEAR(resultado[2].tFim, 1.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_manter_dois_trechos_dentro_na_uniao_de_solidos_disjuntos) {
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaEsfera(1.0), {5.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({-3.0, 0.0, 0.0}, {7.0, 0.0, 0.0}));

  ASSERT_EQ(resultado.size(), 5u);
  EXPECT_EQ(resultado[0].classificacao, EClassificacao::OUT);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_EQ(resultado[2].classificacao, EClassificacao::OUT);
  EXPECT_EQ(resultado[3].classificacao, EClassificacao::IN);
  EXPECT_EQ(resultado[4].classificacao, EClassificacao::OUT);
  EXPECT_NEAR(resultado[1].tInicio, 0.2, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 0.4, TOLERANCIA);
  EXPECT_NEAR(resultado[3].tInicio, 0.7, TOLERANCIA);
  EXPECT_NEAR(resultado[3].tFim, 0.9, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_gerar_um_unico_trecho_dentro_na_uniao_de_solidos_sobrepostos) {
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaBloco(2.0), {1.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({-3.0, 0.0, 0.0}, {3.0, 0.0, 0.0}));

  // A união ocupa x em [-1, 2]; a aresta tem comprimento 6.
  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 2.0 / 6.0, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 5.0 / 6.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_classificar_a_aresta_atraves_de_movimento_sobre_a_uniao) {
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaEsfera(1.0), {2.0, 0.0, 0.0}));
  const TArvoreCSG movida = Transladada(uniao, {10.0, 0.0, 0.0});

  // A união movida ocupa x em [9, 13]; a aresta vai de 7 a 15.
  const TClassificacaoAresta resultado =
      ClassificaAresta(movida, TAresta({7.0, 0.0, 0.0}, {15.0, 0.0, 0.0}));

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 0.25, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 0.75, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest, deve_classificar_a_aresta_em_uniao_aninhada) {
  // Três esferas unitárias em x = 0, 1,5 e 3: todas se sobrepõem em cadeia.
  const TArvoreCSG uniao = Uniao(
      Uniao(CriaEsfera(1.0), Transladada(CriaEsfera(1.0), {1.5, 0.0, 0.0})),
      Transladada(CriaEsfera(1.0), {3.0, 0.0, 0.0}));

  // A união ocupa x em [-1, 4]; a aresta vai de -2 a 5 (comprimento 7).
  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({-2.0, 0.0, 0.0}, {5.0, 0.0, 0.0}));

  ASSERT_EQ(resultado.size(), 3u);
  EXPECT_EQ(resultado[1].classificacao, EClassificacao::IN);
  EXPECT_NEAR(resultado[1].tInicio, 1.0 / 7.0, TOLERANCIA);
  EXPECT_NEAR(resultado[1].tFim, 6.0 / 7.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest, deve_cobrir_todo_o_intervalo_sem_lacunas) {
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaEsfera(1.0), {2.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({-3.0, 0.5, 0.0}, {5.0, -0.5, 0.0}));

  ASSERT_FALSE(resultado.empty());
  EXPECT_DOUBLE_EQ(resultado.front().tInicio, 0.0);
  EXPECT_DOUBLE_EQ(resultado.back().tFim, 1.0);
  for (std::size_t i = 0; i + 1 < resultado.size(); ++i) {
    EXPECT_DOUBLE_EQ(resultado[i].tFim, resultado[i + 1].tInicio);
    EXPECT_NE(resultado[i].classificacao, resultado[i + 1].classificacao);
  }
}

//----------------------------------------------------------------------------------------------
