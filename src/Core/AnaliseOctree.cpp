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
}

//----------------------------------------------------------------------------------------------

double TAnaliseOctree::CalculaVolume(
  const TOctree& octree
) const
{
  return CalculaVolumeNo(octree.Raiz());
}

//----------------------------------------------------------------------------------------------
