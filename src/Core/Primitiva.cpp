#include "Primitiva.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

//----------------------------------------------------------------------------------------------

void TPrimitiva::ExigeDimensaoPositiva(
  double valor,
  const char* mensagem
)
{
  if (!std::isfinite(valor) || valor <= 0.0) {
    throw std::invalid_argument(mensagem);
  }
}

//----------------------------------------------------------------------------------------------

EClassificacao TPrimitiva::ClassificaPorDistancia(
  double distanciaComSinal
)
{
  if (distanciaComSinal < -TOLERANCIA_GEOMETRICA) {
    return EClassificacao::IN;
  }

  if (distanciaComSinal > TOLERANCIA_GEOMETRICA) {
    return EClassificacao::OUT;
  }

  return EClassificacao::ON;
}

//----------------------------------------------------------------------------------------------

TClassificacaoAresta TPrimitiva::ClassificaPorIntervalo(
  double tEntrada,
  double tSaida,
  EClassificacao classificacaoInterna
)
{
  const double inicio = std::max(tEntrada, 0.0);
  const double fim = std::min(tSaida, 1.0);

  // Um intervalo vazio ou reduzido a um ponto (tangência) não tem comprimento: a aresta fica fora.
  if (fim - inicio <= TOLERANCIA_GEOMETRICA) {
    return { { 0.0, 1.0, EClassificacao::OUT } };
  }

  TClassificacaoAresta classificacao;
  if (inicio > TOLERANCIA_GEOMETRICA) {
    classificacao.push_back({ 0.0, inicio, EClassificacao::OUT });
  }

  classificacao.push_back({ inicio, fim, classificacaoInterna });

  if (fim < 1.0 - TOLERANCIA_GEOMETRICA) {
    classificacao.push_back({ fim, 1.0, EClassificacao::OUT });
  }

  return classificacao;
}

//----------------------------------------------------------------------------------------------
