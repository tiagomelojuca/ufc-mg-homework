#include "ClassificadorCSG.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace {
EClassificacao Uniao(EClassificacao esquerda, EClassificacao direita) {
  if (esquerda == EClassificacao::IN || direita == EClassificacao::IN) {
    return EClassificacao::IN;
  }

  if (esquerda == EClassificacao::ON || direita == EClassificacao::ON) {
    return EClassificacao::ON;
  }

  return EClassificacao::OUT;
}

// Classe do trecho da aresta que contém o parâmetro t.
EClassificacao ClasseEm(const TClassificacaoAresta &classificacao, double t) {
  for (const TSegmentoClassificado &segmento : classificacao) {
    if (t >= segmento.tInicio && t <= segmento.tFim) {
      return segmento.classificacao;
    }
  }

  throw std::logic_error("Classificacao de aresta nao cobre o parametro t");
}

// Todos os parâmetros em que alguma das classificações muda, em ordem crescente
// e sem repetições (dentro da tolerância), sempre incluindo 0 e 1.
std::vector<double> CortesOrdenados(const TClassificacaoAresta &esquerda,
                                    const TClassificacaoAresta &direita) {
  std::vector<double> todos = {0.0, 1.0};
  for (const TClassificacaoAresta *classificacao : {&esquerda, &direita}) {
    for (const TSegmentoClassificado &segmento : *classificacao) {
      todos.push_back(std::clamp(segmento.tInicio, 0.0, 1.0));
      todos.push_back(std::clamp(segmento.tFim, 0.0, 1.0));
    }
  }

  std::sort(todos.begin(), todos.end());

  std::vector<double> cortes;
  for (double t : todos) {
    if (cortes.empty() || t - cortes.back() > TOLERANCIA_GEOMETRICA) {
      cortes.push_back(t);
    }
  }

  cortes.back() = 1.0;

  return cortes;
}
} // namespace

//----------------------------------------------------------------------------------------------

EClassificacao CombinaClassificacoes(EOperacaoBooleana operacao,
                                     EClassificacao esquerda,
                                     EClassificacao direita) {
  switch (operacao) {
  case EOperacaoBooleana::UNIAO:
    return Uniao(esquerda, direita);

  case EOperacaoBooleana::INTERSECAO:
  case EOperacaoBooleana::DIFERENCA:
    throw std::logic_error("Operacao booleana ainda nao implementada");
 
  }
  
  throw std::logic_error("Operacao booleana desconhecida");
}

//----------------------------------------------------------------------------------------------

TClassificacaoAresta CombinaArestas(EOperacaoBooleana operacao,
                                    const TClassificacaoAresta &esquerda,
                                    const TClassificacaoAresta &direita) {
  const std::vector<double> cortes = CortesOrdenados(esquerda, direita);

  TClassificacaoAresta resultado;
  for (std::size_t i = 0; i + 1 < cortes.size(); ++i) {
    const double inicio = cortes[i];
    const double fim = cortes[i + 1];

    // Dentro de um trecho nenhuma das duas classificações muda; o ponto médio
    // basta.
    const double meio = (inicio + fim) / 2.0;
    const EClassificacao classe = CombinaClassificacoes(
        operacao, ClasseEm(esquerda, meio), ClasseEm(direita, meio));

    // Funde trechos vizinhos de mesma classe, mantendo a lista minimal.
    if (!resultado.empty() && resultado.back().classificacao == classe) {
      resultado.back().tFim = fim;
    } else {
      resultado.push_back({inicio, fim, classe});
    }
  }

  return resultado;
}

//----------------------------------------------------------------------------------------------

EClassificacao ClassificaPonto(const TArvoreCSG &arvore,
                               const TVetor3D &ponto) {
  switch (arvore->Tipo()) {
  case ETipoNoCSG::PRIMITIVA:
    return arvore->Primitiva().ClassificaPonto(ponto);

  case ETipoNoCSG::MOVIMENTO_RIGIDO:
    return ClassificaPonto(arvore->Filho(),
                           arvore->Movimento().LevaAoSistemaDoFilho(ponto));

  case ETipoNoCSG::OPERACAO_BOOLEANA:
    return CombinaClassificacoes(arvore->Operacao(),
                                 ClassificaPonto(arvore->Esquerda(), ponto),
                                 ClassificaPonto(arvore->Direita(), ponto));

   case ETipoNoCSG::ESCALA:
        return ClassificaPonto(
          arvore->Filho(),
          arvore->Escala().LevaAoSistemaDoFilho(ponto)
        );  
  }

  throw std::logic_error("Tipo de no desconhecido");
}

//----------------------------------------------------------------------------------------------

TClassificacaoAresta ClassificaAresta(const TArvoreCSG &arvore,
                                      const TAresta &aresta) {
  switch (arvore->Tipo()) {
  case ETipoNoCSG::PRIMITIVA:
    return arvore->Primitiva().ClassificaAresta(aresta);

  case ETipoNoCSG::MOVIMENTO_RIGIDO:
    // A transformação é afim: o t da aresta original é o mesmo no sistema do
    // filho.
    return ClassificaAresta(arvore->Filho(),
                            arvore->Movimento().LevaAoSistemaDoFilho(aresta));

  case ETipoNoCSG::OPERACAO_BOOLEANA:
    return CombinaArestas(arvore->Operacao(),
                          ClassificaAresta(arvore->Esquerda(), aresta),
                          ClassificaAresta(arvore->Direita(), aresta));
     case ETipoNoCSG::ESCALA:
      return ClassificaAresta(
        arvore->Filho(),
        arvore->Escala().LevaAoSistemaDoFilho(aresta)
      );
  }

  throw std::logic_error("Tipo de no desconhecido");
}

//----------------------------------------------------------------------------------------------
