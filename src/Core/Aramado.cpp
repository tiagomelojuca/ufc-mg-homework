#include "Aramado.h"

#include <stdexcept>

#include "ClassificadorCSG.h"

//----------------------------------------------------------------------------------------------

std::vector<TArestaCandidata> ArestasCandidatas(
  const TArvoreCSG& arvore
)
{
  switch (arvore->Tipo()) {
    case ETipoNoCSG::PRIMITIVA:
      return arvore->Primitiva().ArestasDoContorno();

    case ETipoNoCSG::MOVIMENTO_RIGIDO: {
      // As arestas do filho são levadas ao sistema do pai pelo próprio movimento.
      const TMatriz4x4 matriz = arvore->Movimento().Matriz();

      std::vector<TArestaCandidata> arestas = ArestasCandidatas(arvore->Filho());
      for (TArestaCandidata& candidata : arestas) {
        candidata.aresta = TAresta(
          matriz.TransformaPonto(candidata.aresta.Inicio()),
          matriz.TransformaPonto(candidata.aresta.Fim())
        );
        if (candidata.pontoDaCurva) {
          candidata.pontoDaCurva = matriz.TransformaPonto(*candidata.pontoDaCurva);
        }
      }

      return arestas;
    }

    case ETipoNoCSG::OPERACAO_BOOLEANA: {
      std::vector<TArestaCandidata> arestas = ArestasCandidatas(arvore->Esquerda());
      const std::vector<TArestaCandidata> direita = ArestasCandidatas(arvore->Direita());
      arestas.insert(arestas.end(), direita.begin(), direita.end());
      return arestas;
    }
  }

  throw std::logic_error("Tipo de no desconhecido");
}

//----------------------------------------------------------------------------------------------

std::vector<TAresta> SegmentosEonS(
  const TArvoreCSG& arvore
)
{
  std::vector<TAresta> segmentos;

  for (const TArestaCandidata& candidata : ArestasCandidatas(arvore)) {
    if (candidata.pontoDaCurva) {
      if (ClassificaPonto(arvore, *candidata.pontoDaCurva) == EClassificacao::ON) {
        segmentos.push_back(candidata.aresta);
      }

      continue;
    }

    for (const TSegmentoClassificado& trecho : ClassificaAresta(arvore, candidata.aresta)) {
      if (trecho.classificacao == EClassificacao::ON) {
        segmentos.push_back(
          TAresta(candidata.aresta.Ponto(trecho.tInicio), candidata.aresta.Ponto(trecho.tFim))
        );
      }
    }
  }

  return segmentos;
}

//----------------------------------------------------------------------------------------------
