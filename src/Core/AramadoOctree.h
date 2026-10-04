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

struct TEstruturaAramadaOctree
{
  std::vector<TAresta3D> parciais;
  std::vector<TAresta3D> cheias;
  std::vector<TAresta3D> vazias;
};

//----------------------------------------------------------------------------------------------

class TGeradorAramadoOctree
{
  public:
    std::vector<TAresta3D> Gera(
      const TOctree& octree
    ) const;

    TEstruturaAramadaOctree GeraEstrutura(
      const TNoOctree& raiz,
      int niveis
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
