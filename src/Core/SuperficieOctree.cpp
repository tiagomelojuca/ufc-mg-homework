#include "SuperficieOctree.h"

#include <cmath>

namespace
{
  bool Contem(
    const TCubo& cubo,
    const TCoordenada3D& ponto
  )
  {
    const double metade = cubo.Lado() / 2.0;
    return std::abs(ponto.x - cubo.Centro().x) <= metade &&
      std::abs(ponto.y - cubo.Centro().y) <= metade &&
      std::abs(ponto.z - cubo.Centro().z) <= metade;
  }

  // Localiza o nó que cobre a região vizinha de mesmo lado ou, se ela não existir na árvore, uma folha maior.
  const TNoOctree* LocalizaVizinho(
    const TNoOctree& raiz,
    const TCoordenada3D& centro,
    double lado
  )
  {
    if (!Contem(raiz.Regiao(), centro)) {
      return nullptr;
    }

    const TNoOctree* no = &raiz;
    while (!no->EhFolha() && no->Regiao().Lado() > lado) {
      const TCoordenada3D& centroNo = no->Regiao().Centro();
      // A ordem dos octantes segue x + 2z + 4y.
      const std::size_t indice =
        (centro.x >= centroNo.x ? 1 : 0) +
        (centro.z >= centroNo.z ? 2 : 0) +
        (centro.y >= centroNo.y ? 4 : 0);
      no = &no->Filho(indice);
    }
    return no;
  }

  void AdicionaFace(
    const TCoordenada3D& centroFace,
    double lado,
    const TCoordenada3D& n,
    std::vector<TFace3D>& faces
  )
  {
    const double m = lado / 2.0;
    // Dois eixos tangentes à face, escolhidos para manter a orientação anti-horária vista de fora.
    const TCoordenada3D u = n.x != 0.0 ? TCoordenada3D { 0.0, n.x, 0.0 } : (n.y != 0.0 ? TCoordenada3D { 0.0, 0.0, n.y } : TCoordenada3D { n.z, 0.0, 0.0 });
    const TCoordenada3D v = { n.y * u.z - n.z * u.y, n.z * u.x - n.x * u.z, n.x * u.y - n.y * u.x };
    const auto vertice = [&](double a, double b) {
      return TCoordenada3D {
        centroFace.x + (u.x * a + v.x * b) * m,
        centroFace.y + (u.y * a + v.y * b) * m,
        centroFace.z + (u.z * a + v.z * b) * m
      };
    };
    faces.push_back({ { vertice(-1.0, -1.0), vertice(1.0, -1.0), vertice(1.0, 1.0), vertice(-1.0, 1.0) }, n });
  }

  // Emite as partes da face voltada para o vizinho que não estão encostadas em células cheias.
  void AdicionaPartesExpostas(
    const TNoOctree& vizinho,
    const TCoordenada3D& n,
    std::vector<TFace3D>& faces
  )
  {
    if (vizinho.Estado() == EEstadoNoOctree::CHEIO) {
      return;
    }

    const TCubo& regiao = vizinho.Regiao();
    if (vizinho.Estado() == EEstadoNoOctree::VAZIO) {
      const double m = regiao.Lado() / 2.0;
      const TCoordenada3D centroFace = {
        regiao.Centro().x - n.x * m,
        regiao.Centro().y - n.y * m,
        regiao.Centro().z - n.z * m
      };
      AdicionaFace(centroFace, regiao.Lado(), n, faces);
      return;
    }

    // Somente os quatro filhos do lado voltado para a face tocam nela.
    const std::size_t bitEixo = n.x != 0.0 ? 1 : (n.z != 0.0 ? 2 : 4);
    const double sentido = n.x + n.y + n.z;
    for (std::size_t indice = 0; indice < 8; ++indice) {
      const bool superior = (indice & bitEixo) != 0;
      if (superior == (sentido < 0.0)) {
        AdicionaPartesExpostas(vizinho.Filho(indice), n, faces);
      }
    }
  }

  void AdicionaFacesCubo(
    const TNoOctree& raiz,
    const TCubo& cubo,
    std::vector<TFace3D>& faces
  )
  {
    const TCoordenada3D& c = cubo.Centro();
    const double lado = cubo.Lado();
    const std::array<TCoordenada3D, 6> normais = {{
      { 1.0, 0.0, 0.0 }, { -1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 }, { 0.0, -1.0, 0.0 },
      { 0.0, 0.0, 1.0 }, { 0.0, 0.0, -1.0 }
    }};

    for (const TCoordenada3D& n : normais) {
      const TCoordenada3D centroVizinho = { c.x + n.x * lado, c.y + n.y * lado, c.z + n.z * lado };
      const TNoOctree* vizinho = LocalizaVizinho(raiz, centroVizinho, lado);
      if (vizinho == nullptr || (vizinho->EhFolha() && vizinho->Estado() == EEstadoNoOctree::VAZIO)) {
        AdicionaFace({ c.x + n.x * lado / 2.0, c.y + n.y * lado / 2.0, c.z + n.z * lado / 2.0 }, lado, n, faces);
      } else if (vizinho->Estado() == EEstadoNoOctree::PARCIAL) {
        AdicionaPartesExpostas(*vizinho, n, faces);
      }
    }
  }

  void GeraSuperficieNo(
    const TNoOctree& raiz,
    const TNoOctree& no,
    std::vector<TFace3D>& faces
  )
  {
    if (no.Estado() == EEstadoNoOctree::VAZIO) {
      return;
    }

    if (no.Estado() == EEstadoNoOctree::CHEIO) {
      AdicionaFacesCubo(raiz, no.Regiao(), faces);
      return;
    }

    for (std::size_t indice = 0; indice < 8; ++indice) {
      GeraSuperficieNo(raiz, no.Filho(indice), faces);
    }
  }
}

//----------------------------------------------------------------------------------------------

std::vector<TFace3D> TGeradorSuperficieOctree::Gera(
  const TOctree& octree
) const
{
  std::vector<TFace3D> faces;
  GeraSuperficieNo(octree.Raiz(), octree.Raiz(), faces);
  return faces;
}

//----------------------------------------------------------------------------------------------
