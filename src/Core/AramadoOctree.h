#ifndef ARAMADO_OCTREE_H_
#define ARAMADO_OCTREE_H_

#include <vector>

#include "Octree.h"

//----------------------------------------------------------------------------------------------

struct TAresta3D
{
  TCoordenada3D inicio;
  TCoordenada3D fim;
};

//----------------------------------------------------------------------------------------------

class TGeradorAramadoOctree
{
  public:
    std::vector<TAresta3D> Gera(
      const TOctree& octree
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
