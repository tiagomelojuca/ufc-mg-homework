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

  const TNoOctree* LocalizaFolha(
    const TNoOctree& raiz,
    const TCoordenada3D& ponto
  )
  {
    if (!Contem(raiz.Regiao(), ponto)) {
      return nullptr;
    }

    const TNoOctree* no = &raiz;
    while (!no->EhFolha()) {
      const TCoordenada3D& centro = no->Regiao().Centro();
      // A ordem dos octantes segue x + 2z + 4y.
      const std::size_t indice =
        (ponto.x >= centro.x ? 1 : 0) +
        (ponto.z >= centro.z ? 2 : 0) +
        (ponto.y >= centro.y ? 4 : 0);
      no = &no->Filho(indice);
    }
    return no;
  }

  bool FaceEncoberta(
    const TNoOctree& raiz,
    const TCubo& cubo,
    const TCoordenada3D& normal
  )
  {
    // Um vizinho cheio com lado maior ou igual cobre a face inteira, pois as regiões são alinhadas.
    const double distancia = cubo.Lado() * 0.75;
    const TCoordenada3D vizinho = {
      cubo.Centro().x + normal.x * distancia,
      cubo.Centro().y + normal.y * distancia,
      cubo.Centro().z + normal.z * distancia
    };
    const TNoOctree* folha = LocalizaFolha(raiz, vizinho);
    return folha != nullptr &&
      folha->Estado() == EEstadoNoOctree::CHEIO &&
      folha->Regiao().Lado() >= cubo.Lado();
  }

  void AdicionaFacesCubo(
    const TNoOctree& raiz,
    const TCubo& cubo,
    std::vector<TFace3D>& faces
  )
  {
    const TCoordenada3D& c = cubo.Centro();
    const double m = cubo.Lado() / 2.0;
    const std::array<TCoordenada3D, 6> normais = {{
      { 1.0, 0.0, 0.0 }, { -1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 }, { 0.0, -1.0, 0.0 },
      { 0.0, 0.0, 1.0 }, { 0.0, 0.0, -1.0 }
    }};

    for (const TCoordenada3D& n : normais) {
      if (FaceEncoberta(raiz, cubo, n)) {
        continue;
      }

      // Dois eixos tangentes à face, escolhidos para manter a orientação anti-horária vista de fora.
      const TCoordenada3D u = n.x != 0.0 ? TCoordenada3D { 0.0, n.x, 0.0 } : (n.y != 0.0 ? TCoordenada3D { 0.0, 0.0, n.y } : TCoordenada3D { n.z, 0.0, 0.0 });
      const TCoordenada3D v = { n.y * u.z - n.z * u.y, n.z * u.x - n.x * u.z, n.x * u.y - n.y * u.x };
      const TCoordenada3D centroFace = { c.x + n.x * m, c.y + n.y * m, c.z + n.z * m };
      const auto vertice = [&](double a, double b) {
        return TCoordenada3D {
          centroFace.x + (u.x * a + v.x * b) * m,
          centroFace.y + (u.y * a + v.y * b) * m,
          centroFace.z + (u.z * a + v.z * b) * m
        };
      };
      faces.push_back({ { vertice(-1.0, -1.0), vertice(1.0, -1.0), vertice(1.0, 1.0), vertice(-1.0, 1.0) }, n });
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
