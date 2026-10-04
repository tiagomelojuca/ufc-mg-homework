#ifndef OPERACOES_OCTREE_H_
#define OPERACOES_OCTREE_H_

#include "Octree.h"

//----------------------------------------------------------------------------------------------

class TOperacoesBooleanasOctree
{
  public:
    TOctree Uniao(
      const TOctree& primeira,
      const TOctree& segunda
    ) const;

    TOctree Intersecao(
      const TOctree& primeira,
      const TOctree& segunda
    ) const;
};

//----------------------------------------------------------------------------------------------

class TOperacoesGeometricasOctree
{
  public:
    TOctree Escala(
      const TOctree& octree,
      double fator
    ) const;

    TOctree Translada(
      const TOctree& octree,
      const TCoordenada3D& deslocamento
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
