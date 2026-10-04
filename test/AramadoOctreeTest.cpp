#include <gtest/gtest.h>

#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>

#include "Core/AramadoOctree.h"
#include "Core/Bloco.h"
#include "Core/Esfera.h"
#include "Core/OperacoesOctree.h"

namespace
{
  void VerificaArestasCubo(
    const std::vector<TAresta3D>& arestas,
    std::size_t inicio,
    const TCubo& cubo
  )
  {
    ASSERT_GE(arestas.size(), inicio + 12);
    using TPonto = std::tuple<double, double, double>;
    std::set<std::pair<TPonto, TPonto>> segmentos;
    std::set<TPonto> vertices;
    const TCoordenada3D& centro = cubo.Centro();
    const double metade = cubo.Lado() / 2.0;

    for (std::size_t indice = inicio; indice < inicio + 12; ++indice) {
      const TAresta3D& aresta = arestas[indice];
      for (const TCoordenada3D& ponto : { aresta.inicio, aresta.fim }) {
        EXPECT_TRUE(ponto.x == centro.x - metade || ponto.x == centro.x + metade);
        EXPECT_TRUE(ponto.y == centro.y - metade || ponto.y == centro.y + metade);
        EXPECT_TRUE(ponto.z == centro.z - metade || ponto.z == centro.z + metade);
        vertices.emplace(ponto.x, ponto.y, ponto.z);
      }

      const int eixosDiferentes =
        (aresta.inicio.x != aresta.fim.x) +
        (aresta.inicio.y != aresta.fim.y) +
        (aresta.inicio.z != aresta.fim.z);
      EXPECT_EQ(eixosDiferentes, 1);

      TPonto primeiro(aresta.inicio.x, aresta.inicio.y, aresta.inicio.z);
      TPonto segundo(aresta.fim.x, aresta.fim.y, aresta.fim.z);
      if (segundo < primeiro) {
        std::swap(primeiro, segundo);
      }
      segmentos.emplace(primeiro, segundo);
    }

    EXPECT_EQ(vertices.size(), 8u);
    EXPECT_EQ(segmentos.size(), 12u);
  }

  TNoOctree CriaRaizVaziaSubdividida(
    const TConfiguracaoOctree& configuracao
  )
  {
    TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);
    for (std::size_t indice = 0; indice < 8; ++indice) {
      raiz.Filho(indice).DefineEstado(EEstadoNoOctree::VAZIO);
    }
    return raiz;
  }
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_ignorar_uma_arvore_vazia)
{
  const TConfiguracaoOctree configuracao;
  const TOctree octree(configuracao, TNoOctree(configuracao.Dominio(), EEstadoNoOctree::VAZIO));

  EXPECT_TRUE(TGeradorAramadoOctree().Gera(octree).empty());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_gerar_as_doze_arestas_da_raiz_cheia)
{
  const TOctree octree;
  const auto arestas = TGeradorAramadoOctree().Gera(octree);

  ASSERT_EQ(arestas.size(), 12u);
  VerificaArestasCubo(arestas, 0, octree.Raiz().Regiao());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_usar_o_centro_e_lado_do_dominio_configurado)
{
  const TOctree octree(TConfiguracaoOctree(TCubo({ 2.0, -3.0, 4.0 }, 4.0), 2));
  const auto arestas = TGeradorAramadoOctree().Gera(octree);

  ASSERT_EQ(arestas.size(), 12u);
  VerificaArestasCubo(arestas, 0, octree.Raiz().Regiao());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_percorrer_os_oito_octantes_sem_desenhar_o_pai)
{
  const TConfiguracaoOctree configuracao;
  const TOctree octree(configuracao, TNoOctree(configuracao.Dominio(), EEstadoNoOctree::PARCIAL));
  const auto arestas = TGeradorAramadoOctree().Gera(octree);

  ASSERT_EQ(arestas.size(), 96u);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    VerificaArestasCubo(arestas, indice * 12, octree.Raiz().Filho(indice).Regiao());
  }
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_ignorar_filhos_vazios_e_visitar_profundidades_diferentes)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz = CriaRaizVaziaSubdividida(configuracao);
  raiz.Filho(1).DefineEstado(EEstadoNoOctree::CHEIO);
  raiz.Filho(6).DefineEstado(EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(6).Filho(indice).DefineEstado(
      indice == 3 ? EEstadoNoOctree::CHEIO : EEstadoNoOctree::VAZIO
    );
  }
  const TOctree octree(configuracao, std::move(raiz));
  const auto arestas = TGeradorAramadoOctree().Gera(octree);

  ASSERT_EQ(arestas.size(), 24u);
  VerificaArestasCubo(arestas, 0, octree.Raiz().Filho(1).Regiao());
  VerificaArestasCubo(arestas, 12, octree.Raiz().Filho(6).Filho(3).Regiao());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_desenhar_um_bloco_sem_consultar_a_primitiva)
{
  TOctree octree;
  octree.Constroi(TClassificadorBlocoOctree(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0)));
  const auto arestas = TGeradorAramadoOctree().Gera(octree);

  ASSERT_EQ(arestas.size(), 12u);
  VerificaArestasCubo(arestas, 0, TCubo({ -0.5, -0.5, -0.5 }, 1.0));
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_desenhar_a_aproximacao_terminal_da_esfera)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  TOctree octree(configuracao);
  octree.Constroi(TClassificadorEsferaOctree(TEsfera({ 0.0, 0.0, 0.0 }, 0.75)));
  const auto arestas = TGeradorAramadoOctree().Gera(octree);

  ASSERT_EQ(arestas.size(), 96u);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    VerificaArestasCubo(arestas, indice * 12, configuracao.Dominio().Octante(indice));
  }
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_desenhar_a_arvore_resultante_da_uniao)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raizPrimeira = CriaRaizVaziaSubdividida(configuracao);
  TNoOctree raizSegunda = CriaRaizVaziaSubdividida(configuracao);
  raizPrimeira.Filho(0).DefineEstado(EEstadoNoOctree::CHEIO);
  raizSegunda.Filho(7).DefineEstado(EEstadoNoOctree::CHEIO);
  const TOctree primeira(configuracao, std::move(raizPrimeira));
  const TOctree segunda(configuracao, std::move(raizSegunda));
  const TOctree resultado = TOperacoesBooleanasOctree().Uniao(primeira, segunda);
  const auto arestas = TGeradorAramadoOctree().Gera(resultado);

  ASSERT_EQ(arestas.size(), 24u);
  VerificaArestasCubo(arestas, 0, configuracao.Dominio().Octante(0));
  VerificaArestasCubo(arestas, 12, configuracao.Dominio().Octante(7));
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_desenhar_as_celulas_transformadas_pela_escala)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3);
  TNoOctree raiz = CriaRaizVaziaSubdividida(configuracao);
  raiz.Filho(0).DefineEstado(EEstadoNoOctree::CHEIO);
  const TOctree octree(configuracao, std::move(raiz));
  const TOctree resultado = TOperacoesGeometricasOctree().Escala(octree, 0.5);
  const auto arestas = TGeradorAramadoOctree().Gera(resultado);

  ASSERT_EQ(arestas.size(), 12u);
  VerificaArestasCubo(arestas, 0, TCubo({ -0.25, -0.25, -0.25 }, 0.5));
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_gerar_somente_a_raiz_na_estrutura_com_zero_niveis)
{
  const TConfiguracaoOctree configuracao;
  const TOctree octree(configuracao, CriaRaizVaziaSubdividida(configuracao));
  const auto estrutura = TGeradorAramadoOctree().GeraEstrutura(octree.Raiz(), 0);

  ASSERT_EQ(estrutura.parciais.size(), 12u);
  EXPECT_TRUE(estrutura.cheias.empty());
  EXPECT_TRUE(estrutura.vazias.empty());
  VerificaArestasCubo(estrutura.parciais, 0, octree.Raiz().Regiao());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_separar_os_nos_da_estrutura_por_estado)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz = CriaRaizVaziaSubdividida(configuracao);
  raiz.Filho(1).DefineEstado(EEstadoNoOctree::CHEIO);
  raiz.Filho(6).DefineEstado(EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(6).Filho(indice).DefineEstado(
      indice == 3 ? EEstadoNoOctree::CHEIO : EEstadoNoOctree::VAZIO
    );
  }
  const TOctree octree(configuracao, std::move(raiz));
  const auto estrutura = TGeradorAramadoOctree().GeraEstrutura(octree.Raiz(), 2);

  ASSERT_EQ(estrutura.parciais.size(), 24u);
  ASSERT_EQ(estrutura.cheias.size(), 24u);
  ASSERT_EQ(estrutura.vazias.size(), 13u * 12u);
  VerificaArestasCubo(estrutura.parciais, 0, octree.Raiz().Regiao());
  VerificaArestasCubo(estrutura.parciais, 12, octree.Raiz().Filho(6).Regiao());
  VerificaArestasCubo(estrutura.cheias, 0, octree.Raiz().Filho(1).Regiao());
  VerificaArestasCubo(estrutura.cheias, 12, octree.Raiz().Filho(6).Filho(3).Regiao());
  VerificaArestasCubo(estrutura.vazias, 0, octree.Raiz().Filho(0).Regiao());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_limitar_a_estrutura_aos_niveis_pedidos)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz = CriaRaizVaziaSubdividida(configuracao);
  raiz.Filho(6).DefineEstado(EEstadoNoOctree::PARCIAL);
  const TOctree octree(configuracao, std::move(raiz));
  const auto estrutura = TGeradorAramadoOctree().GeraEstrutura(octree.Raiz(), 1);

  ASSERT_EQ(estrutura.parciais.size(), 24u);
  EXPECT_TRUE(estrutura.cheias.empty());
  EXPECT_EQ(estrutura.vazias.size(), 7u * 12u);
  VerificaArestasCubo(estrutura.parciais, 12, octree.Raiz().Filho(6).Regiao());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_gerar_a_estrutura_a_partir_de_um_no_interno)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz = CriaRaizVaziaSubdividida(configuracao);
  raiz.Filho(2).DefineEstado(EEstadoNoOctree::PARCIAL);
  const TOctree octree(configuracao, std::move(raiz));
  const TNoOctree& no = octree.Raiz().Filho(2);
  const auto estrutura = TGeradorAramadoOctree().GeraEstrutura(no, 5);

  ASSERT_EQ(estrutura.parciais.size(), 12u);
  ASSERT_EQ(estrutura.cheias.size(), 96u);
  EXPECT_TRUE(estrutura.vazias.empty());
  VerificaArestasCubo(estrutura.parciais, 0, no.Regiao());
  for (std::size_t indice = 0; indice < 8; ++indice) {
    VerificaArestasCubo(estrutura.cheias, indice * 12, no.Filho(indice).Regiao());
  }
}

//----------------------------------------------------------------------------------------------

TEST(AramadoOctreeTest, deve_rejeitar_quantidade_negativa_de_niveis_na_estrutura)
{
  const TOctree octree;

  EXPECT_THROW(TGeradorAramadoOctree().GeraEstrutura(octree.Raiz(), -1), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------
