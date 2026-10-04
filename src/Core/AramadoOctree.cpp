#include "AramadoOctree.h"

#include <array>
#include <stdexcept>

namespace
{
  void AdicionaArestasCubo(
    const TCubo& cubo,
    std::vector<TAresta3D>& arestas
  )
  {
    const TCoordenada3D& centro = cubo.Centro();
    const double metadeLado = cubo.Lado() / 2.0;
    std::array<TCoordenada3D, 8> vertices;

    // A ordem dos vértices acompanha os octantes: x + 2z + 4y.
    for (std::size_t indice = 0; indice < vertices.size(); ++indice) {
      vertices[indice] = {
        centro.x + ((indice & 1) == 0 ? -metadeLado : metadeLado),
        centro.y + ((indice & 4) == 0 ? -metadeLado : metadeLado),
        centro.z + ((indice & 2) == 0 ? -metadeLado : metadeLado)
      };
    }

    const std::array<std::array<std::size_t, 2>, 12> extremos = {{
      { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
      { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
      { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    }};

    for (const auto& extremo : extremos) {
      arestas.push_back({ vertices[extremo[0]], vertices[extremo[1]] });
    }
  }

  void GeraAramadoNo(
    const TNoOctree& no,
    std::vector<TAresta3D>& arestas
  )
  {
    if (no.Estado() == EEstadoNoOctree::VAZIO) {
      return;
    }

    if (no.Estado() == EEstadoNoOctree::CHEIO) {
      AdicionaArestasCubo(no.Regiao(), arestas);
      return;
    }

    for (std::size_t indice = 0; indice < 8; ++indice) {
      GeraAramadoNo(no.Filho(indice), arestas);
    }
  }

  void GeraEstruturaNo(
    const TNoOctree& no,
    int niveisRestantes,
    TEstruturaAramadaOctree& estrutura
  )
  {
    if (no.Estado() == EEstadoNoOctree::VAZIO) {
      AdicionaArestasCubo(no.Regiao(), estrutura.vazias);
      return;
    }

    if (no.Estado() == EEstadoNoOctree::CHEIO) {
      AdicionaArestasCubo(no.Regiao(), estrutura.cheias);
      return;
    }

    AdicionaArestasCubo(no.Regiao(), estrutura.parciais);
    if (niveisRestantes == 0) {
      return;
    }

    for (std::size_t indice = 0; indice < 8; ++indice) {
      GeraEstruturaNo(no.Filho(indice), niveisRestantes - 1, estrutura);
    }
  }
}

//----------------------------------------------------------------------------------------------

std::vector<TAresta3D> TGeradorAramadoOctree::Gera(
  const TOctree& octree
) const
{
  std::vector<TAresta3D> arestas;
  GeraAramadoNo(octree.Raiz(), arestas);
  return arestas;
}

//

//----------------------------------------------------------------------------------------------

TEstruturaAramadaOctree TGeradorAramadoOctree::GeraEstrutura(
  const TNoOctree& raiz,
  int niveis
) const
{
  if (niveis < 0) {
    throw std::invalid_argument("A quantidade de níveis não pode ser negativa");
  }

  TEstruturaAramadaOctree estrutura;
  GeraEstruturaNo(raiz, niveis, estrutura);
  return estrutura;
}

//----------------------------------------------------------------------------------------------
