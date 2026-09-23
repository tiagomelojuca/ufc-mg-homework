#include <gtest/gtest.h>

#include <stdexcept>

#include "Core/Octree.h"

//----------------------------------------------------------------------------------------------

namespace
{
  class TClassificadorPorMetade : public TClassificadorOctree
  {
    public:
      EEstadoNoOctree Classifica(
        const TCubo& regiao
      ) const override
      {
        if (regiao.Lado() == 2.0) {
          return EEstadoNoOctree::PARCIAL;
        }

        return regiao.Centro().x < 0.0
          ? EEstadoNoOctree::VAZIO
          : EEstadoNoOctree::CHEIO;
      }

      bool EstaContido(
        const TCubo&
      ) const override
      {
        return true;
      }
  };

  class TClassificadorSempreParcial : public TClassificadorOctree
  {
    public:
      EEstadoNoOctree Classifica(
        const TCubo&
      ) const override
      {
        ++quantidadeChamadas;
        return EEstadoNoOctree::PARCIAL;
      }

      bool EstaContido(
        const TCubo&
      ) const override
      {
        return true;
      }

      mutable int quantidadeChamadas = 0;
  };

  class TClassificadorSempreVazio : public TClassificadorOctree
  {
    public:
      EEstadoNoOctree Classifica(
        const TCubo&
      ) const override
      {
        return EEstadoNoOctree::VAZIO;
      }

      bool EstaContido(
        const TCubo&
      ) const override
      {
        return true;
      }
  };
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_usar_a_configuracao_padrao_definida)
{
  const TOctree octree;

  EXPECT_DOUBLE_EQ(octree.Configuracao().Dominio().Centro().x, 0.0);
  EXPECT_DOUBLE_EQ(octree.Configuracao().Dominio().Centro().y, 0.0);
  EXPECT_DOUBLE_EQ(octree.Configuracao().Dominio().Centro().z, 0.0);
  EXPECT_DOUBLE_EQ(octree.Configuracao().Dominio().Lado(), 2.0);
  EXPECT_EQ(octree.Configuracao().ProfundidadeMaxima(), 5);
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_construir_nos_cheios_vazios_e_parciais)
{
  TOctree octree(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3));
  const TClassificadorPorMetade classificador;

  octree.Constroi(classificador);

  EXPECT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_FALSE(octree.Raiz().EhFolha());

  for (std::size_t indice = 0; indice < 8; ++indice) {
    const EEstadoNoOctree estadoEsperado = (indice & 1) == 0
      ? EEstadoNoOctree::VAZIO
      : EEstadoNoOctree::CHEIO;

    EXPECT_EQ(octree.Raiz().Filho(indice).Estado(), estadoEsperado);
    EXPECT_TRUE(octree.Raiz().Filho(indice).EhFolha());
  }
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_ordenar_os_filhos_por_x_mais_2z_mais_4y)
{
  TOctree octree(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 2));
  const TClassificadorPorMetade classificador;

  octree.Constroi(classificador);

  const std::array<TCoordenada3D, 8> centrosEsperados = {
    TCoordenada3D { -0.5, -0.5, -0.5 },
    TCoordenada3D {  0.5, -0.5, -0.5 },
    TCoordenada3D { -0.5, -0.5,  0.5 },
    TCoordenada3D {  0.5, -0.5,  0.5 },
    TCoordenada3D { -0.5,  0.5, -0.5 },
    TCoordenada3D {  0.5,  0.5, -0.5 },
    TCoordenada3D { -0.5,  0.5,  0.5 },
    TCoordenada3D {  0.5,  0.5,  0.5 }
  };

  for (std::size_t indice = 0; indice < centrosEsperados.size(); ++indice) {
    const TCoordenada3D& centro = octree.Raiz().Filho(indice).Regiao().Centro();

    EXPECT_DOUBLE_EQ(centro.x, centrosEsperados[indice].x);
    EXPECT_DOUBLE_EQ(centro.y, centrosEsperados[indice].y);
    EXPECT_DOUBLE_EQ(centro.z, centrosEsperados[indice].z);
    EXPECT_DOUBLE_EQ(octree.Raiz().Filho(indice).Regiao().Lado(), 1.0);
  }
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_considerar_cheio_o_no_no_limite_de_profundidade)
{
  TOctree octree(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 1));
  TClassificadorSempreParcial classificador;

  octree.Constroi(classificador);

  EXPECT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_TRUE(octree.Raiz().EhFolha());
  EXPECT_EQ(classificador.quantidadeChamadas, 0);
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_rejeitar_configuracoes_invalidas)
{
  EXPECT_THROW(TCubo({ 0.0, 0.0, 0.0 }, 0.0), std::invalid_argument);
  EXPECT_THROW(
    TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 0),
    std::invalid_argument
  );
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_rejeitar_acesso_a_filhos_de_uma_folha)
{
  const TOctree octree;

  EXPECT_THROW(octree.Raiz().Filho(0), std::logic_error);
  EXPECT_THROW(octree.Raiz().Filho(8), std::out_of_range);
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_remover_os_filhos_ao_reconstruir_com_uma_folha)
{
  TOctree octree(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3));
  const TClassificadorPorMetade classificadorParcial;
  const TClassificadorSempreVazio classificadorVazio;

  octree.Constroi(classificadorParcial);
  ASSERT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::PARCIAL);

  octree.Constroi(classificadorVazio);

  EXPECT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::VAZIO);
  EXPECT_TRUE(octree.Raiz().EhFolha());
  EXPECT_THROW(octree.Raiz().Filho(0), std::logic_error);
}

//----------------------------------------------------------------------------------------------

TEST(OctreeTest, deve_subdividir_recursivamente_ate_o_limite)
{
  TOctree octree(TConfiguracaoOctree(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3));
  TClassificadorSempreParcial classificador;

  octree.Constroi(classificador);

  EXPECT_EQ(classificador.quantidadeChamadas, 9);
  ASSERT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::PARCIAL);

  for (std::size_t filho = 0; filho < 8; ++filho) {
    ASSERT_EQ(octree.Raiz().Filho(filho).Estado(), EEstadoNoOctree::PARCIAL);

    for (std::size_t neto = 0; neto < 8; ++neto) {
      EXPECT_EQ(
        octree.Raiz().Filho(filho).Filho(neto).Estado(),
        EEstadoNoOctree::CHEIO
      );
    }
  }
}

//----------------------------------------------------------------------------------------------
