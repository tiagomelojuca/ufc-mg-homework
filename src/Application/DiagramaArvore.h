#ifndef DIAGRAMAARVORE_H_
#define DIAGRAMAARVORE_H_

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "Core/ArvoreCSG.h"

//----------------------------------------------------------------------------------------------

// Texto de um nó em duas linhas: o tipo do nó e os seus parâmetros, que podem faltar.
struct TRotuloNo
{
  std::string titulo;
  std::string detalhe;
};

//----------------------------------------------------------------------------------------------

struct TNoDiagrama
{
  ETipoNoCSG tipo;
  TRotuloNo rotulo;

  // Profundidade do nó, com a raiz no nível 0.
  int nivel;

  // Posição horizontal em colunas: cada folha ocupa uma coluna, e cada nó interno fica no meio
  // dos seus filhos.
  double coluna;

  // Índice do pai na lista de nós; vazio na raiz.
  std::optional<std::size_t> pai;
};

//----------------------------------------------------------------------------------------------

// Disposição da árvore para exibição, com os nós em pré-ordem. Uma subárvore compartilhada
// aparece uma vez em cada lugar em que é usada.
struct TDiagramaArvore
{
  std::vector<TNoDiagrama> nos;
  int colunas;
  int niveis;
};

//----------------------------------------------------------------------------------------------

TDiagramaArvore MontaDiagrama(
  const TArvoreCSG& arvore
);

// Texto do nó: a primitiva com os seus parâmetros, a operação booleana com o seu código no arquivo
// ou o movimento rígido com os seus valores.
TRotuloNo RotuloDoNo(
  const TNoCSG& no
);

//----------------------------------------------------------------------------------------------

#endif
