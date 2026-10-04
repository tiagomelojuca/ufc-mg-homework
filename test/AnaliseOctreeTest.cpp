#include <gtest/gtest.h>

#include <utility>

#include "Core/AnaliseOctree.h"
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

  TOctree CriaOctreeComOctanteCheio(
    const TConfiguracaoOctree& configuracao,
    std::size_t octante
  )
  {
    TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);

    for (std::size_t indice = 0; indice < 8; ++indice) {
      raiz.Filho(indice).DefineEstado(
        indice == octante
          ? EEstadoNoOctree::CHEIO
          : EEstadoNoOctree::VAZIO
      );
    }

    return TOctree(configuracao, std::move(raiz));
  }
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_volume_zero_para_arvore_vazia)
{
  const TOctree octree = CriaOctreeFolha(EEstadoNoOctree::VAZIO);
  const TAnaliseOctree analise;

  EXPECT_DOUBLE_EQ(analise.CalculaVolume(octree), 0.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_o_volume_de_uma_folha_cheia)
{
  const TOctree octree = CriaOctreeFolha(EEstadoNoOctree::CHEIO);
  const TAnaliseOctree analise;

  EXPECT_DOUBLE_EQ(analise.CalculaVolume(octree), 8.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_o_volume_de_um_octante_cheio)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree octree = CriaOctreeComOctanteCheio(configuracao, 0);
  const TAnaliseOctree analise;

  EXPECT_DOUBLE_EQ(analise.CalculaVolume(octree), 1.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_somar_folhas_cheias_em_profundidades_diferentes)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3);
  TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);

  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(indice).DefineEstado(EEstadoNoOctree::VAZIO);
  }

  raiz.Filho(0).DefineEstado(EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(0).Filho(indice).DefineEstado(
      indice == 0 ? EEstadoNoOctree::CHEIO : EEstadoNoOctree::VAZIO
    );
  }
  raiz.Filho(1).DefineEstado(EEstadoNoOctree::CHEIO);

  const TOctree octree(configuracao, std::move(raiz));
  const TAnaliseOctree analise;

  EXPECT_DOUBLE_EQ(analise.CalculaVolume(octree), 1.125);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_o_mesmo_volume_antes_da_compactacao)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree compactada = CriaOctreeFolha(
    EEstadoNoOctree::CHEIO,
    configuracao
  );
  TNoOctree raizSubdividida(
    configuracao.Dominio(),
    EEstadoNoOctree::PARCIAL
  );
  const TOctree subdividida(configuracao, std::move(raizSubdividida));
  const TAnaliseOctree analise;

  EXPECT_DOUBLE_EQ(
    analise.CalculaVolume(subdividida),
    analise.CalculaVolume(compactada)
  );
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_respeitar_o_dominio_configurado)
{
  const TConfiguracaoOctree configuracao(TCubo({ 2.0, 3.0, 4.0 }, 4.0), 3);
  const TOctree octree = CriaOctreeFolha(
    EEstadoNoOctree::CHEIO,
    configuracao
  );
  const TAnaliseOctree analise;

  EXPECT_DOUBLE_EQ(analise.CalculaVolume(octree), 64.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_o_volume_apos_uma_uniao)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2);
  const TOctree primeira = CriaOctreeComOctanteCheio(configuracao, 0);
  const TOctree segunda = CriaOctreeComOctanteCheio(configuracao, 1);
  const TOperacoesBooleanasOctree operacoes;
  const TAnaliseOctree analise;

  const TOctree uniao = operacoes.Uniao(primeira, segunda);

  EXPECT_DOUBLE_EQ(analise.CalculaVolume(uniao), 2.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_o_volume_apos_uma_escala_alinhada)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3);
  const TOctree octree = CriaOctreeFolha(
    EEstadoNoOctree::CHEIO,
    configuracao
  );
  const TOperacoesGeometricasOctree operacoes;
  const TAnaliseOctree analise;

  const TOctree escalada = operacoes.Escala(octree, 0.5);

  EXPECT_DOUBLE_EQ(analise.CalculaVolume(escalada), 1.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_contar_uma_folha_no_nivel_zero)
{
  const TOctree octree = CriaOctreeFolha(EEstadoNoOctree::VAZIO);
  const auto contagens = TAnaliseOctree().ContaNosPorNivel(octree.Raiz());

  ASSERT_EQ(contagens.size(), 1u);
  EXPECT_EQ(contagens[0].vazios, 1u);
  EXPECT_EQ(contagens[0].cheios, 0u);
  EXPECT_EQ(contagens[0].parciais, 0u);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_contar_os_nos_de_cada_nivel_por_estado)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(indice).DefineEstado(indice < 3 ? EEstadoNoOctree::VAZIO : EEstadoNoOctree::CHEIO);
  }
  raiz.Filho(7).DefineEstado(EEstadoNoOctree::PARCIAL);
  raiz.Filho(7).Filho(0).DefineEstado(EEstadoNoOctree::VAZIO);
  const TOctree octree(configuracao, std::move(raiz));
  const auto contagens = TAnaliseOctree().ContaNosPorNivel(octree.Raiz());

  ASSERT_EQ(contagens.size(), 3u);
  EXPECT_EQ(contagens[0].parciais, 1u);
  EXPECT_EQ(contagens[1].vazios, 3u);
  EXPECT_EQ(contagens[1].cheios, 4u);
  EXPECT_EQ(contagens[1].parciais, 1u);
  EXPECT_EQ(contagens[2].vazios, 1u);
  EXPECT_EQ(contagens[2].cheios, 7u);
  EXPECT_EQ(contagens[2].parciais, 0u);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_contar_os_niveis_a_partir_de_um_no_interno)
{
  const TOctree octree = CriaOctreeComOctanteCheio(TConfiguracaoOctree(), 4);
  const auto contagens = TAnaliseOctree().ContaNosPorNivel(octree.Raiz().Filho(4));

  ASSERT_EQ(contagens.size(), 1u);
  EXPECT_EQ(contagens[0].cheios, 1u);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_a_area_superficial_de_um_cubo)
{
  const TAnaliseOctree analise;

  EXPECT_DOUBLE_EQ(analise.CalculaAreaSuperficial(CriaOctreeFolha(EEstadoNoOctree::CHEIO)), 24.0);
  EXPECT_DOUBLE_EQ(analise.CalculaAreaSuperficial(CriaOctreeFolha(EEstadoNoOctree::VAZIO)), 0.0);
  EXPECT_DOUBLE_EQ(analise.CalculaAreaSuperficial(CriaOctreeComOctanteCheio(TConfiguracaoOctree(), 5)), 6.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_desconsiderar_faces_internas_na_area_superficial)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(indice).DefineEstado(indice < 2 ? EEstadoNoOctree::CHEIO : EEstadoNoOctree::VAZIO);
  }
  const TOctree octree(configuracao, std::move(raiz));

  // Dois cubos unitários lado a lado formam uma caixa 2 x 1 x 1.
  EXPECT_DOUBLE_EQ(TAnaliseOctree().CalculaAreaSuperficial(octree), 10.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_descontar_somente_a_parte_encostada_em_um_vizinho_menor)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(indice).DefineEstado(EEstadoNoOctree::VAZIO);
  }
  raiz.Filho(0).DefineEstado(EEstadoNoOctree::CHEIO);
  raiz.Filho(1).DefineEstado(EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(1).Filho(indice).DefineEstado(indice == 0 ? EEstadoNoOctree::CHEIO : EEstadoNoOctree::VAZIO);
  }
  const TOctree octree(configuracao, std::move(raiz));

  // Cubo de lado 1 (área 6) mais cubo de lado 0,5 (área 1,5), menos duas vezes o contato de 0,25.
  EXPECT_DOUBLE_EQ(TAnaliseOctree().CalculaAreaSuperficial(octree), 7.0);
}

//----------------------------------------------------------------------------------------------

TEST(AnaliseOctreeTest, deve_calcular_a_mesma_area_antes_e_depois_da_compactacao)
{
  const TConfiguracaoOctree configuracao;
  const TOctree subdividida(configuracao, TNoOctree(configuracao.Dominio(), EEstadoNoOctree::PARCIAL));

  EXPECT_DOUBLE_EQ(TAnaliseOctree().CalculaAreaSuperficial(subdividida), 24.0);
}

//----------------------------------------------------------------------------------------------
