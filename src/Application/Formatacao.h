#ifndef FORMATACAO_H_
#define FORMATACAO_H_

#include <string>

#include "Core/Vetor3D.h"

//----------------------------------------------------------------------------------------------

// Número curto para exibição, com ponto decimal independente da configuração regional.
std::string FormataNumero(
  double valor
);

// Vetor no formato (x, y, z).
std::string FormataVetor(
  const TVetor3D& valores
);

//----------------------------------------------------------------------------------------------

#endif
