#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>
#include <utility>

#include "Core/OperacoesOctree.h"

namespace
{
  TOctree CriaOctreeFolha(
    EEstadoNoOctree estado,
    const TConfiguracaoOctree& configuracao = TConfiguracaoOctree()
  )
  {
    return TOctree(
      configuracao,
      TNoOctree(configuracao.Dominio(), estado)
    );
  }

  TOctree CriaOctreeComFilhosCheios(
    const TConfiguracaoOctree& configuracao,
    std::size_t primeiro,
    std::size_t ultimo
  )
  {
    TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);

    for (std::size_t indice = 0; indice < 8; ++indice) {
      raiz.Filho(indice).DefineEstado(
        indice >= primeiro && indice <= ultimo
          ? EEstadoNoOctree::CHEIO
          : EEstadoNoOctree::VAZIO
      );
    }

    return TOctree(configuracao, std::move(raiz));
  }

  EEstadoNoOctree EstadoNoPonto(
    const TOctree& octree,
    const TCoordenada3D& ponto
  )
  {
    const TNoOctree* no = &octree.Raiz();

    while (!no->EhFolha()) {
      const TCoordenada3D& centro = no->Regiao().Centro();
      const std::size_t x = ponto.x < centro.x ? 0 : 1;
      const std::size_t y = ponto.y < centro.y ? 0 : 1;
      const std::size_t z = ponto.z < centro.z ? 0 : 1;
      no = &no->Filho(x + 2 * z + 4 * y);
    }

    return no->Estado();
  }

  bool NosIguais(
    const TNoOctree& primeiro,
    const TNoOctree& segundo
  )
  {
    if (primeiro.Estado() != segundo.Estado()) {
      return false;
    }

    if (primeiro.EhFolha()) {
      return true;
    }

    for (std::size_t indice = 0; indice < 8; ++indice) {
      if (!NosIguais(primeiro.Filho(indice), segundo.Filho(indice))) {
        return false;
      }
    }

    return true;
  }
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_aplicar_as_regras_de_uniao_entre_folhas)
{
  const TOperacoesBooleanasOctree operacoes;
  const TOctree cheia = CriaOctreeFolha(EEstadoNoOctree::CHEIO);
  const TOctree vazia = CriaOctreeFolha(EEstadoNoOctree::VAZIO);

  EXPECT_EQ(operacoes.Uniao(cheia, cheia).Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(operacoes.Uniao(cheia, vazia).Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(operacoes.Uniao(vazia, cheia).Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(operacoes.Uniao(vazia, vazia).Raiz().Estado(), EEstadoNoOctree::VAZIO);
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_unir_nos_parciais_recursivamente)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree primeira = CriaOctreeComFilhosCheios(configuracao, 0, 0);
  const TOctree segunda = CriaOctreeComFilhosCheios(configuracao, 1, 1);
  const TOperacoesBooleanasOctree operacoes;

  const TOctree resultado = operacoes.Uniao(primeira, segunda);

  ASSERT_EQ(resultado.Raiz().Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(resultado.Raiz().Filho(0).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(resultado.Raiz().Filho(1).Estado(), EEstadoNoOctree::CHEIO);

  for (std::size_t indice = 2; indice < 8; ++indice) {
    EXPECT_EQ(resultado.Raiz().Filho(indice).Estado(), EEstadoNoOctree::VAZIO);
  }
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_copiar_a_subarvore_quando_o_outro_no_for_vazio)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree parcial = CriaOctreeComFilhosCheios(configuracao, 3, 3);
  const TOctree vazia = CriaOctreeFolha(EEstadoNoOctree::VAZIO, configuracao);
  const TOperacoesBooleanasOctree operacoes;

  const TOctree resultado = operacoes.Uniao(vazia, parcial);

  ASSERT_EQ(resultado.Raiz().Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(resultado.Raiz().Filho(3).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(parcial.Raiz().Filho(3).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(vazia.Raiz().Estado(), EEstadoNoOctree::VAZIO);
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_compactar_um_resultado_homogeneo)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree primeira = CriaOctreeComFilhosCheios(configuracao, 0, 3);
  const TOctree segunda = CriaOctreeComFilhosCheios(configuracao, 4, 7);
  const TOperacoesBooleanasOctree operacoes;

  const TOctree resultado = operacoes.Uniao(primeira, segunda);

  EXPECT_EQ(resultado.Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_TRUE(resultado.Raiz().EhFolha());
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_compactar_uma_subarvore_copiada)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);
  const TOctree naoCompactada(configuracao, std::move(raiz));
  const TOctree vazia = CriaOctreeFolha(EEstadoNoOctree::VAZIO, configuracao);
  const TOperacoesBooleanasOctree operacoes;

  const TOctree resultado = operacoes.Uniao(vazia, naoCompactada);

  EXPECT_EQ(resultado.Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_TRUE(resultado.Raiz().EhFolha());
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_ser_comutativa)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree primeira = CriaOctreeComFilhosCheios(configuracao, 0, 2);
  const TOctree segunda = CriaOctreeComFilhosCheios(configuracao, 2, 4);
  const TOperacoesBooleanasOctree operacoes;

  const TOctree primeiraComSegunda = operacoes.Uniao(primeira, segunda);
  const TOctree segundaComPrimeira = operacoes.Uniao(segunda, primeira);

  EXPECT_TRUE(NosIguais(primeiraComSegunda.Raiz(), segundaComPrimeira.Raiz()));
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_aceitar_profundidades_diferentes)
{
  const TCubo dominio({ 0.0, 0.0, 0.0 }, 2.0);
  const TConfiguracaoOctree configuracaoRasa(dominio, 2);
  const TConfiguracaoOctree configuracaoProfunda(dominio, 4);
  const TOctree rasa = CriaOctreeComFilhosCheios(configuracaoRasa, 0, 0);
  TNoOctree raizProfunda(dominio, EEstadoNoOctree::PARCIAL);

  for (std::size_t indice = 0; indice < 8; ++indice) {
    raizProfunda.Filho(indice).DefineEstado(EEstadoNoOctree::VAZIO);
  }

  raizProfunda.Filho(1).DefineEstado(EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raizProfunda.Filho(1).Filho(indice).DefineEstado(
      indice == 0 ? EEstadoNoOctree::CHEIO : EEstadoNoOctree::VAZIO
    );
  }

  const TOctree profunda(configuracaoProfunda, std::move(raizProfunda));
  const TOperacoesBooleanasOctree operacoes;

  const TOctree resultado = operacoes.Uniao(rasa, profunda);

  EXPECT_EQ(resultado.Configuracao().ProfundidadeMaxima(), 4);
  EXPECT_EQ(resultado.Raiz().Filho(0).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(resultado.Raiz().Filho(1).Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(resultado.Raiz().Filho(1).Filho(0).Estado(), EEstadoNoOctree::CHEIO);
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesBooleanasOctreeTest, deve_rejeitar_dominios_diferentes)
{
  const TOctree primeira = CriaOctreeFolha(EEstadoNoOctree::VAZIO);
  const TConfiguracaoOctree outraConfiguracao(
    TCubo({ 1.0, 0.0, 0.0 }, 2.0),
    5
  );
  const TOctree segunda = CriaOctreeFolha(
    EEstadoNoOctree::VAZIO,
    outraConfiguracao
  );
  const TOperacoesBooleanasOctree operacoes;

  EXPECT_THROW(operacoes.Uniao(primeira, segunda), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesGeometricasOctreeTest, deve_rejeitar_fator_nao_positivo)
{
  const TOctree octree;
  const TOperacoesGeometricasOctree operacoes;

  EXPECT_THROW(operacoes.Escala(octree, 0.0), std::invalid_argument);
  EXPECT_THROW(operacoes.Escala(octree, -1.0), std::invalid_argument);
  EXPECT_THROW(
    operacoes.Escala(octree, std::numeric_limits<double>::infinity()),
    std::invalid_argument
  );
  EXPECT_THROW(
    operacoes.Escala(octree, std::numeric_limits<double>::quiet_NaN()),
    std::invalid_argument
  );
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesGeometricasOctreeTest, deve_preservar_a_arvore_na_escala_identidade)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree octree = CriaOctreeComFilhosCheios(configuracao, 2, 2);
  const TOperacoesGeometricasOctree operacoes;

  const TOctree resultado = operacoes.Escala(octree, 1.0);

  ASSERT_EQ(resultado.Raiz().Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(resultado.Raiz().Filho(2).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(octree.Raiz().Filho(2).Estado(), EEstadoNoOctree::CHEIO);
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesGeometricasOctreeTest, deve_manter_vazia_uma_octree_sem_volume)
{
  const TOctree vazia = CriaOctreeFolha(EEstadoNoOctree::VAZIO);
  const TOperacoesGeometricasOctree operacoes;

  const TOctree resultado = operacoes.Escala(vazia, 10.0);

  EXPECT_EQ(resultado.Raiz().Estado(), EEstadoNoOctree::VAZIO);
  EXPECT_TRUE(resultado.Raiz().EhFolha());
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesGeometricasOctreeTest, deve_contrair_em_relacao_a_origem)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 4);
  const TOctree cheia = CriaOctreeFolha(EEstadoNoOctree::CHEIO, configuracao);
  const TOperacoesGeometricasOctree operacoes;

  const TOctree resultado = operacoes.Escala(cheia, 0.5);

  EXPECT_EQ(
    EstadoNoPonto(resultado, { 0.25, 0.25, 0.25 }),
    EEstadoNoOctree::CHEIO
  );
  EXPECT_EQ(
    EstadoNoPonto(resultado, { 0.75, 0.75, 0.75 }),
    EEstadoNoOctree::VAZIO
  );
  EXPECT_EQ(cheia.Raiz().Estado(), EEstadoNoOctree::CHEIO);
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesGeometricasOctreeTest, deve_escalar_uma_folha_fora_da_origem)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3);
  const TOctree octree = CriaOctreeComFilhosCheios(configuracao, 0, 0);
  const TOperacoesGeometricasOctree operacoes;

  const TOctree resultado = operacoes.Escala(octree, 0.5);

  EXPECT_EQ(
    EstadoNoPonto(resultado, { -0.25, -0.25, -0.25 }),
    EEstadoNoOctree::CHEIO
  );
  EXPECT_EQ(
    EstadoNoPonto(resultado, { -0.75, -0.75, -0.75 }),
    EEstadoNoOctree::VAZIO
  );
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesGeometricasOctreeTest, deve_expandir_uma_regiao_contida)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3);
  TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);

  for (std::size_t octante = 0; octante < 8; ++octante) {
    raiz.Filho(octante).DefineEstado(EEstadoNoOctree::PARCIAL);

    for (std::size_t filho = 0; filho < 8; ++filho) {
      raiz.Filho(octante).Filho(filho).DefineEstado(
        filho == (7 - octante)
          ? EEstadoNoOctree::CHEIO
          : EEstadoNoOctree::VAZIO
      );
    }
  }

  const TOctree octree(configuracao, std::move(raiz));
  const TOperacoesGeometricasOctree operacoes;

  const TOctree resultado = operacoes.Escala(octree, 2.0);

  EXPECT_EQ(resultado.Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_TRUE(resultado.Raiz().EhFolha());
}

//----------------------------------------------------------------------------------------------

TEST(OperacoesGeometricasOctreeTest, deve_rejeitar_resultado_fora_do_dominio)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree octree = CriaOctreeComFilhosCheios(configuracao, 1, 1);
  const TOperacoesGeometricasOctree operacoes;

  EXPECT_THROW(operacoes.Escala(octree, 2.0), std::invalid_argument);
  EXPECT_EQ(octree.Raiz().Filho(1).Estado(), EEstadoNoOctree::CHEIO);
}

//----------------------------------------------------------------------------------------------
