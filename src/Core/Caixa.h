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

#endif
