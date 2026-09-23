#ifndef ANALISE_OCTREE_H_
#define ANALISE_OCTREE_H_

#include "Octree.h"

//----------------------------------------------------------------------------------------------

class TAnaliseOctree
{
  public:
    double CalculaVolume(
      const TOctree& octree
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
