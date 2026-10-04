#ifndef ANALISE_OCTREE_H_
#define ANALISE_OCTREE_H_

#include <cstddef>
#include <vector>

#include "Octree.h"
#include "SuperficieOctree.h"

//----------------------------------------------------------------------------------------------

struct TContagemNivelOctree
{
  std::size_t cheios = 0;
  std::size_t vazios = 0;
  std::size_t parciais = 0;
};

//----------------------------------------------------------------------------------------------

class TAnaliseOctree
{
  public:
    double CalculaVolume(
      const TOctree& octree
    ) const;

    // Área do contorno das células cheias, isto é, da aproximação representada pela octree.
    double CalculaAreaSuperficial(
      const TOctree& octree
    ) const;

    double CalculaAreaSuperficial(
      const std::vector<TFace3D>& faces
    ) const;

    std::vector<TContagemNivelOctree> ContaNosPorNivel(
      const TNoOctree& raiz
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
