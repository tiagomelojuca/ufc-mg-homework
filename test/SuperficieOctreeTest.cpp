#include <gtest/gtest.h>

#include <cmath>
#include <utility>

#include "Core/Bloco.h"
#include "Core/SuperficieOctree.h"

namespace
{
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

  double Produto(
    const TCoordenada3D& a,
    const TCoordenada3D& b
  )
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  void VerificaFace(
    const TFace3D& face,
    const TCubo& cubo
  )
  {
    const double metade = cubo.Lado() / 2.0;
    const TCoordenada3D& centro = cubo.Centro();
    for (const TCoordenada3D& vertice : face.vertices) {
      const TCoordenada3D relativo = { vertice.x - centro.x, vertice.y - centro.y, vertice.z - centro.z };
      EXPECT_DOUBLE_EQ(Produto(relativo, face.normal), metade);
      EXPECT_DOUBLE_EQ(std::abs(relativo.x), metade);
      EXPECT_DOUBLE_EQ(std::abs(relativo.y), metade);
      EXPECT_DOUBLE_EQ(std::abs(relativo.z), metade);
    }

    // A orientação anti-horária vista de fora produz uma normal no mesmo sentido da face.
    const TCoordenada3D& a = face.vertices[0];
    const TCoordenada3D& b = face.vertices[1];
    const TCoordenada3D& c = face.vertices[2];
    const TCoordenada3D ab = { b.x - a.x, b.y - a.y, b.z - a.z };
    const TCoordenada3D ac = { c.x - a.x, c.y - a.y, c.z - a.z };
    const TCoordenada3D normal = { ab.y * ac.z - ab.z * ac.y, ab.z * ac.x - ab.x * ac.z, ab.x * ac.y - ab.y * ac.x };
    EXPECT_GT(Produto(normal, face.normal), 0.0);
  }
}

//----------------------------------------------------------------------------------------------

TEST(SuperficieOctreeTest, deve_ignorar_uma_arvore_vazia)
{
  const TConfiguracaoOctree configuracao;
  const TOctree octree(configuracao, TNoOctree(configuracao.Dominio(), EEstadoNoOctree::VAZIO));

  EXPECT_TRUE(TGeradorSuperficieOctree().Gera(octree).empty());
}

//----------------------------------------------------------------------------------------------

TEST(SuperficieOctreeTest, deve_gerar_as_seis_faces_da_raiz_cheia)
{
  const TOctree octree;
  const auto faces = TGeradorSuperficieOctree().Gera(octree);

  ASSERT_EQ(faces.size(), 6u);
  for (const TFace3D& face : faces) {
    VerificaFace(face, octree.Raiz().Regiao());
  }
}

//----------------------------------------------------------------------------------------------

TEST(SuperficieOctreeTest, deve_omitir_as_faces_internas_entre_folhas_cheias)
{
  const TConfiguracaoOctree configuracao;
  const TOctree octree(configuracao, TNoOctree(configuracao.Dominio(), EEstadoNoOctree::PARCIAL));
  const auto faces = TGeradorSuperficieOctree().Gera(octree);

  // Oito octantes cheios formam um cubo: cada um expõe apenas as três faces externas.
  ASSERT_EQ(faces.size(), 24u);
  for (std::size_t indice = 0; indice < faces.size(); ++indice) {
    VerificaFace(faces[indice], octree.Raiz().Filho(indice / 3).Regiao());
  }
}

//----------------------------------------------------------------------------------------------

TEST(SuperficieOctreeTest, deve_omitir_somente_a_face_compartilhada_por_dois_vizinhos)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz = CriaRaizVaziaSubdividida(configuracao);
  raiz.Filho(0).DefineEstado(EEstadoNoOctree::CHEIO);
  raiz.Filho(1).DefineEstado(EEstadoNoOctree::CHEIO);
  const TOctree octree(configuracao, std::move(raiz));
  const auto faces = TGeradorSuperficieOctree().Gera(octree);

  ASSERT_EQ(faces.size(), 10u);
  for (const TFace3D& face : faces) {
    EXPECT_FALSE(face.normal.x > 0.0 && face.vertices[0].x == 0.0);
    EXPECT_FALSE(face.normal.x < 0.0 && face.vertices[0].x == 0.0);
  }
}

//----------------------------------------------------------------------------------------------

TEST(SuperficieOctreeTest, deve_manter_a_face_encostada_em_um_vizinho_menor)
{
  const TConfiguracaoOctree configuracao;
  TNoOctree raiz = CriaRaizVaziaSubdividida(configuracao);
  raiz.Filho(0).DefineEstado(EEstadoNoOctree::CHEIO);
  raiz.Filho(1).DefineEstado(EEstadoNoOctree::PARCIAL);
  for (std::size_t indice = 0; indice < 8; ++indice) {
    raiz.Filho(1).Filho(indice).DefineEstado(indice == 0 ? EEstadoNoOctree::CHEIO : EEstadoNoOctree::VAZIO);
  }
  const TOctree octree(configuracao, std::move(raiz));
  const auto faces = TGeradorSuperficieOctree().Gera(octree);

  // O octante grande mantém as seis faces; o pequeno omite a face encostada no grande.
  ASSERT_EQ(faces.size(), 11u);
  for (std::size_t indice = 0; indice < 6; ++indice) {
    VerificaFace(faces[indice], octree.Raiz().Filho(0).Regiao());
  }
  for (std::size_t indice = 6; indice < faces.size(); ++indice) {
    VerificaFace(faces[indice], octree.Raiz().Filho(1).Filho(0).Regiao());
    EXPECT_FALSE(faces[indice].normal.x < 0.0);
  }
}

//----------------------------------------------------------------------------------------------

TEST(SuperficieOctreeTest, deve_gerar_somente_o_contorno_de_um_bloco_alinhado)
{
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3);
  TOctree octree(configuracao);
  octree.Constroi(TClassificadorBlocoOctree(TBloco({ 0.0, 0.0, 0.0 }, 1.0, 1.0, 1.0)));
  const auto faces = TGeradorSuperficieOctree().Gera(octree);

  // O bloco ocupa 4 x 4 x 4 células de lado 0,25: cada lado externo tem 16 faces.
  EXPECT_EQ(faces.size(), 6u * 16u);
}

//----------------------------------------------------------------------------------------------
