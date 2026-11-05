#include <gtest/gtest.h>

#include <memory>
#include <vector>

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

TArvoreCSG CriaBloco(double ladoX, double ladoY, double ladoZ) {
  return CriaPrimitiva(std::make_shared<TBloco>(ladoX, ladoY, ladoZ));
}

TArvoreCSG Rotacionada(const TArvoreCSG &arvore, const TVetor3D &angulos) {
  return CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, angulos), arvore);
}

TArvoreCSG Escalada(const TArvoreCSG &arvore, const TVetor3D &fatores) {
  return CriaEscala(TEscala(fatores), arvore);
}

// Confere as classes e os limites de cada trecho, na ordem de t.
void EsperaTrechos(const TClassificacaoAresta &resultado,
                   const std::vector<TSegmentoClassificado> &esperados) {
  ASSERT_EQ(resultado.size(), esperados.size());
  for (std::size_t i = 0; i < esperados.size(); ++i) {
    EXPECT_EQ(resultado[i].classificacao, esperados[i].classificacao);
    EXPECT_NEAR(resultado[i].tInicio, esperados[i].tInicio, TOLERANCIA);
    EXPECT_NEAR(resultado[i].tFim, esperados[i].tFim, TOLERANCIA);
  }
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

TEST(ClassificadorCSGTest,
     deve_levar_ao_interior_a_face_comum_de_blocos_encostados) {
  // As faces x = 1 dos dois blocos se tocam com normais opostas.
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaBloco(2.0), {2.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({1.0, -3.0, 0.0}, {1.0, 3.0, 0.0}));

  EsperaTrechos(resultado, {{0.0, 1.0 / 3.0, EClassificacao::OUT},
                            {1.0 / 3.0, 2.0 / 3.0, EClassificacao::IN},
                            {2.0 / 3.0, 1.0, EClassificacao::OUT}});
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_manter_na_fronteira_faces_sobrepostas_do_mesmo_lado) {
  // As faces y = 1 dos dois blocos se sobrepõem com a mesma normal.
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaBloco(2.0), {1.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({-3.0, 1.0, 0.0}, {3.0, 1.0, 0.0}));

  // A face ocupa x em [-1, 2]; a aresta tem comprimento 6.
  EsperaTrechos(resultado, {{0.0, 2.0 / 6.0, EClassificacao::OUT},
                            {2.0 / 6.0, 5.0 / 6.0, EClassificacao::ON},
                            {5.0 / 6.0, 1.0, EClassificacao::OUT}});
  EXPECT_EQ(resultado[1].vizinhanca.size(), 1u);
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_fundir_faces_coplanares_vizinhas_em_um_trecho_na_fronteira) {
  const TArvoreCSG uniao =
      Uniao(CriaBloco(2.0), Transladada(CriaBloco(2.0), {2.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({-3.0, 1.0, 0.0}, {5.0, 1.0, 0.0}));

  EsperaTrechos(resultado, {{0.0, 0.25, EClassificacao::OUT},
                            {0.25, 0.75, EClassificacao::ON},
                            {0.75, 1.0, EClassificacao::OUT}});
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_manter_na_fronteira_o_canto_concavo_e_fechar_com_o_quarto_bloco) {
  // Os blocos ocupam quadrantes em torno da reta x = y = 1, paralela a z.
  const TArvoreCSG emL =
      Uniao(Uniao(CriaBloco(2.0), Transladada(CriaBloco(2.0), {2.0, 0.0, 0.0})),
            Transladada(CriaBloco(2.0), {0.0, 2.0, 0.0}));
  const TArvoreCSG quadrado =
      Uniao(emL, Transladada(CriaBloco(2.0), {2.0, 2.0, 0.0}));

  const TAresta aresta({1.0, 1.0, -3.0}, {1.0, 1.0, 3.0});

  // Três quadrantes cobrem 270 graus em torno da aresta: ela continua na
  // fronteira.
  EsperaTrechos(ClassificaAresta(emL, aresta),
                {{0.0, 1.0 / 3.0, EClassificacao::OUT},
                 {1.0 / 3.0, 2.0 / 3.0, EClassificacao::ON},
                 {2.0 / 3.0, 1.0, EClassificacao::OUT}});

  // O quarto quadrante fecha a volta: a aresta passa ao interior.
  EsperaTrechos(ClassificaAresta(quadrado, aresta),
                {{0.0, 1.0 / 3.0, EClassificacao::OUT},
                 {1.0 / 3.0, 2.0 / 3.0, EClassificacao::IN},
                 {2.0 / 3.0, 1.0, EClassificacao::OUT}});
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_levar_as_normais_atraves_da_rotacao) {
  // O bloco 2 x 4 x 2 girado 90 graus em z ocupa x em [1, 5]: sua face x = 1
  // vem de uma face y do sistema do filho.
  const TArvoreCSG uniao = Uniao(
      CriaBloco(2.0),
      Transladada(Rotacionada(CriaBloco(2.0, 4.0, 2.0), {0.0, 0.0, 90.0}),
                  {3.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({1.0, -3.0, 0.0}, {1.0, 3.0, 0.0}));

  EsperaTrechos(resultado, {{0.0, 1.0 / 3.0, EClassificacao::OUT},
                            {1.0 / 3.0, 2.0 / 3.0, EClassificacao::IN},
                            {2.0 / 3.0, 1.0, EClassificacao::OUT}});
}

//----------------------------------------------------------------------------------------------

TEST(ClassificadorCSGTest,
     deve_levar_as_normais_atraves_da_escala) {
  // O bloco escalado ocupa x em [1, 3] e y em [-3, 3]; só a parte da face
  // x = 1 encostada no primeiro bloco passa ao interior.
  const TArvoreCSG uniao = Uniao(
      CriaBloco(2.0),
      Transladada(Escalada(CriaBloco(2.0), {1.0, 3.0, 1.0}), {2.0, 0.0, 0.0}));

  const TClassificacaoAresta resultado =
      ClassificaAresta(uniao, TAresta({1.0, -3.0, 0.0}, {1.0, 3.0, 0.0}));

  EsperaTrechos(resultado, {{0.0, 1.0 / 3.0, EClassificacao::ON},
                            {1.0 / 3.0, 2.0 / 3.0, EClassificacao::IN},
                            {2.0 / 3.0, 1.0, EClassificacao::ON}});
  ASSERT_EQ(resultado[0].vizinhanca.size(), 1u);
  ASSERT_EQ(resultado[0].vizinhanca[0].size(), 1u);
  EXPECT_NEAR(resultado[0].vizinhanca[0][0].x, -1.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------
