#ifndef ILUMINACAO_PHONG_H_
#define ILUMINACAO_PHONG_H_

#include "Octree.h"

//----------------------------------------------------------------------------------------------

struct TCorRGB
{
  double r;
  double g;
  double b;
};

//----------------------------------------------------------------------------------------------

struct TMaterialPhong
{
  double ambiente = 0.22;
  double difuso = 0.75;
  double especular = 0.35;
  double brilho = 24.0;
};

//----------------------------------------------------------------------------------------------

// Modelo local de Phong com uma luz pontual branca: I = ka + kd (N·L) + ks (R·V)^n.
class TIluminacaoPhong
{
  public:
    TIluminacaoPhong(
      const TCoordenada3D& posicaoLuz,
      const TCoordenada3D& direcaoObservador,
      const TMaterialPhong& material = TMaterialPhong()
    );

    // A direção do observador é constante porque a projeção é ortográfica.
    TCorRGB Calcula(
      const TCoordenada3D& ponto,
      const TCoordenada3D& normal,
      const TCorRGB& corBase
    ) const;

  private:
    TCoordenada3D posicaoLuz;
    TCoordenada3D direcaoObservador;
    TMaterialPhong material;
};

//----------------------------------------------------------------------------------------------

#endif
