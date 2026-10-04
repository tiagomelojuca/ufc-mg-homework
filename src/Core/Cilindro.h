#ifndef CILINDRO_H_
#define CILINDRO_H_

#include "Octree.h"

//----------------------------------------------------------------------------------------------

enum class EEixo
{
  X,
  Y,
  Z
};

//----------------------------------------------------------------------------------------------

class TCilindro
{
  public:
    TCilindro(
      const TCoordenada3D& centro,
      double raio,
      double altura,
      EEixo eixo = EEixo::Y
    );

    const TCoordenada3D& Centro() const;
    double Raio() const;
    double Altura() const;
    EEixo Eixo() const;

  private:
    TCoordenada3D centro;
    double raio;
    double altura;
    EEixo eixo;
};

//----------------------------------------------------------------------------------------------

class TClassificadorCilindroOctree : public TClassificadorOctree
{
  public:
    explicit TClassificadorCilindroOctree(
      const TCilindro& cilindro
    );

    EEstadoNoOctree Classifica(
      const TCubo& regiao
    ) const override;

    bool EstaContido(
      const TCubo& dominio
    ) const override;

  private:
    TCilindro cilindro;
};

//----------------------------------------------------------------------------------------------

#endif
