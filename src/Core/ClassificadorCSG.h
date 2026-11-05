#ifndef CLASSIFICADORCSG_H_
#define CLASSIFICADORCSG_H_

#include "ArvoreCSG.h"
#include "Classificacao.h"
#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

EClassificacao CombinaClassificacoes(
  EOperacaoBooleana operacao,
  EClassificacao esquerda,
  EClassificacao direita
);

// Combine_M regularizado: aplica a operação trecho a trecho e decide os trechos ON dos dois lados
// pela vizinhança, em torno da aresta.
TClassificacaoAresta CombinaArestas(
  EOperacaoBooleana operacao,
  const TClassificacaoAresta& esquerda,
  const TClassificacaoAresta& direita,
  const TAresta& aresta
);

EClassificacao ClassificaPonto(
  const TArvoreCSG& arvore,
  const TVetor3D& ponto
);

// Tree_M: classificação da aresta contra a árvore, por divisão e conquista.
TClassificacaoAresta ClassificaAresta(
  const TArvoreCSG& arvore,
  const TAresta& aresta
);

//----------------------------------------------------------------------------------------------

#endif
