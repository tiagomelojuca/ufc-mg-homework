#ifndef BLOCO_H_
#define BLOCO_H_

#include "Octree.h"

//----------------------------------------------------------------------------------------------

class TBloco
{
  public:
    TBloco(
      const TCoordenada3D& centro,
      double ladoX,
      double ladoY,
      double ladoZ
    );

    const TCoordenada3D& Centro() const;
    double LadoX() const;
    double LadoY() const;
    double LadoZ() const;

  private:
    TCoordenada3D centro;
    double ladoX;
    double ladoY;
    double ladoZ;
};

//----------------------------------------------------------------------------------------------

class TClassificadorBlocoOctree : public TClassificadorOctree
{
  public:
    explicit TClassificadorBlocoOctree(
      const TBloco& bloco
    );

    EEstadoNoOctree Classifica(
      const TCubo& regiao
    ) const override;

    bool EstaContido(
      const TCubo& dominio
    ) const override;

  private:
    TBloco bloco;
};

//----------------------------------------------------------------------------------------------

#endif
