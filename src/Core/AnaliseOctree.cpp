#include "AnaliseOctree.h"

namespace
{
  double CalculaVolumeNo(
    const TNoOctree& no
  )
  {
    if (no.Estado() == EEstadoNoOctree::VAZIO) {
      return 0.0;
    }

    if (no.Estado() == EEstadoNoOctree::CHEIO) {
      const double lado = no.Regiao().Lado();
      return lado * lado * lado;
    }

    double volume = 0.0;

    for (std::size_t indice = 0; indice < 8; ++indice) {
      volume += CalculaVolumeNo(no.Filho(indice));
    }

    return volume;
  }

  void ContaNo(
    const TNoOctree& no,
    std::size_t nivel,
    std::vector<TContagemNivelOctree>& contagens
  )
  {
    if (contagens.size() <= nivel) {
      contagens.resize(nivel + 1);
    }

    if (no.Estado() == EEstadoNoOctree::VAZIO) {
      ++contagens[nivel].vazios;
      return;
    }

    if (no.Estado() == EEstadoNoOctree::CHEIO) {
      ++contagens[nivel].cheios;
      return;
    }

    ++contagens[nivel].parciais;
    for (std::size_t indice = 0; indice < 8; ++indice) {
      ContaNo(no.Filho(indice), nivel + 1, contagens);
    }
  }
}

//----------------------------------------------------------------------------------------------

double TAnaliseOctree::CalculaVolume(
  const TOctree& octree
) const
{
  return CalculaVolumeNo(octree.Raiz());
}

//

//----------------------------------------------------------------------------------------------

std::vector<TContagemNivelOctree> TAnaliseOctree::ContaNosPorNivel(
  const TNoOctree& raiz
) const
{
  std::vector<TContagemNivelOctree> contagens;
  ContaNo(raiz, 0, contagens);
  return contagens;
}

//----------------------------------------------------------------------------------------------
