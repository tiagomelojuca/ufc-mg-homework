#ifndef ESFERA_H_
#define ESFERA_H_

#include "Octree.h"

//----------------------------------------------------------------------------------------------

class TEsfera
{
  public:
    TEsfera(
      const TCoordenada3D& centro,
      double raio
    );

    const TCoordenada3D& Centro() const;
    double Raio() const;

  private:
    TCoordenada3D centro;
    double raio;
};

//----------------------------------------------------------------------------------------------

class TClassificadorEsferaOctree : public TClassificadorOctree
{
  public:
    explicit TClassificadorEsferaOctree(
      const TEsfera& esfera
    );

    EEstadoNoOctree Classifica(
      const TCubo& regiao
    ) const override;

    bool EstaContido(
      const TCubo& dominio
    ) const override;

  private:
    TEsfera esfera;
};

//----------------------------------------------------------------------------------------------

#endif
