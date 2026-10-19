#include "Primitiva.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

//----------------------------------------------------------------------------------------------

TClassificacaoAresta TPrimitiva::ClassificaAresta(
  const TAresta& aresta
) const
{
  std::vector<double> parametros = { 0.0, 1.0 };
  for (double t : IntersecoesComFronteira(aresta)) {
    if (t > TOLERANCIA_GEOMETRICA && t < 1.0 - TOLERANCIA_GEOMETRICA) {
      parametros.push_back(t);
    }
  }

  std::sort(parametros.begin(), parametros.end());
  parametros.erase(
    std::unique(
      parametros.begin(),
      parametros.end(),
      [](double a, double b) { return b - a <= TOLERANCIA_GEOMETRICA; }
    ),
    parametros.end()
  );

  // Entre duas interseções consecutivas a classificação não muda; basta testar o ponto médio.
  TClassificacaoAresta classificacao;
  for (std::size_t i = 0; i + 1 < parametros.size(); ++i) {
    const double tInicio = parametros[i];
    const double tFim = parametros[i + 1];
    const EClassificacao estado = ClassificaPonto(aresta.Ponto((tInicio + tFim) / 2.0));

    if (!classificacao.empty() && classificacao.back().classificacao == estado) {
      classificacao.back().tFim = tFim;
    } else {
      classificacao.push_back({ tInicio, tFim, estado });
    }
  }

  return classificacao;
}

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
