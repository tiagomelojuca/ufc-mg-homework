#ifndef ANALISE_OCTREE_H_
#define ANALISE_OCTREE_H_

#include <cstddef>
#include <vector>

#include "Octree.h"

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

    std::vector<TContagemNivelOctree> ContaNosPorNivel(
      const TNoOctree& raiz
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
