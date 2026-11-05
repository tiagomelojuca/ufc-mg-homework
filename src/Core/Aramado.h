#ifndef ARAMADO_H_
#define ARAMADO_H_

#include <vector>

#include "ArvoreCSG.h"
#include "Classificacao.h"

//----------------------------------------------------------------------------------------------

// Arestas do contorno de todas as primitivas da árvore, levadas ao sistema da raiz pelos
// movimentos rígidos acima de cada uma.
std::vector<TArestaCandidata> ArestasCandidatas(
  const TArvoreCSG& arvore
);

// Segmentos EonS: as partes das arestas candidatas que estão sobre a fronteira do sólido. Uma
// aresta reta é classificada por Tree_M e mantém os trechos ON; a corda de uma curva é mantida
// quando o ponto da curva no meio dela é ON.
std::vector<TAresta> SegmentosEonS(
  const TArvoreCSG& arvore
);

//----------------------------------------------------------------------------------------------

#endif
