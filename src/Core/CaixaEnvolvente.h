#ifndef CAIXAENVOLVENTE_H_
#define CAIXAENVOLVENTE_H_

#include "ArvoreCSG.h"
#include "Caixa.h"

//----------------------------------------------------------------------------------------------

// Caixa alinhada aos eixos que contém o sólido da árvore. Pode ser maior que o sólido: no
// movimento rígido, é a caixa dos oito cantos transformados da caixa do filho.
TCaixa CaixaEnvolvente(
  const TArvoreCSG& arvore
);

//----------------------------------------------------------------------------------------------

#endif
