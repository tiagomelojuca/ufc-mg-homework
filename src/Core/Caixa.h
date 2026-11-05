#ifndef CAIXA_H_
#define CAIXA_H_

#include <algorithm>

#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

// Caixa alinhada aos eixos, de minimo a maximo em cada coordenada.
struct TCaixa
{
  TVetor3D minimo;
  TVetor3D maximo;
};

//----------------------------------------------------------------------------------------------

inline TVetor3D Centro(
  const TCaixa& caixa
)
{
  return 0.5 * (caixa.minimo + caixa.maximo);
}

//----------------------------------------------------------------------------------------------

// Menor caixa que contém as duas.
inline TCaixa UneCaixas(
  const TCaixa& a,
  const TCaixa& b
)
{
  return {
    { std::min(a.minimo.x, b.minimo.x), std::min(a.minimo.y, b.minimo.y), std::min(a.minimo.z, b.minimo.z) },
    { std::max(a.maximo.x, b.maximo.x), std::max(a.maximo.y, b.maximo.y), std::max(a.maximo.z, b.maximo.z) }
  };
}

//----------------------------------------------------------------------------------------------

// Parte comum das duas caixas. Sem parte comum, a caixa se reduz a um ponto, com volume nulo.
inline TCaixa IntersectaCaixas(
  const TCaixa& a,
  const TCaixa& b
)
{
  const TVetor3D minimo = {
    std::max(a.minimo.x, b.minimo.x), std::max(a.minimo.y, b.minimo.y), std::max(a.minimo.z, b.minimo.z)
  };
  const TVetor3D maximo = {
    std::min(a.maximo.x, b.maximo.x), std::min(a.maximo.y, b.maximo.y), std::min(a.maximo.z, b.maximo.z)
  };

  if (maximo.x < minimo.x || maximo.y < minimo.y || maximo.z < minimo.z) {
    return { minimo, minimo };
  }

  return { minimo, maximo };
}

//----------------------------------------------------------------------------------------------

#endif
