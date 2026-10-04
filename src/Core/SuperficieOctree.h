#ifndef SUPERFICIE_OCTREE_H_
#define SUPERFICIE_OCTREE_H_

#include <array>
#include <vector>

#include "Octree.h"

//----------------------------------------------------------------------------------------------

struct TFace3D
{
  std::array<TCoordenada3D, 4> vertices;
  TCoordenada3D normal;
};

//----------------------------------------------------------------------------------------------

class TGeradorSuperficieOctree
{
  public:
    // Gera as faces das folhas cheias que não estão totalmente encostadas em outra região cheia.
    std::vector<TFace3D> Gera(
      const TOctree& octree
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
