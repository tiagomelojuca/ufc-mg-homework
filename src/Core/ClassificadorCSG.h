#ifndef CLASSIFICADORCSG_H_
#define CLASSIFICADORCSG_H_

#define CLASSIFICADORCSG_H_
#include "ArvoreCSG.h"
#include "Classificacao.h"
#include "Vetor3D.h"

EClassificacao CombinaClassificacoes(EOperacaoBooleana operacao,
                                     EClassificacao esquerda,
                                     EClassificacao direita);

TClassificacaoAresta CombinaArestas(EOperacaoBooleana operacao,
                                    const TClassificacaoAresta &esquerda,
                                    const TClassificacaoAresta &direita);
EClassificacao ClassificaPonto(const TArvoreCSG &arvore, const TVetor3D &ponto);

TClassificacaoAresta ClassificaAresta(const TArvoreCSG &arvore,
                                      const TAresta &aresta);

#endif // !CLASSIFICADORCSG_H_
