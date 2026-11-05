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

void TPrimitiva::ExigeFatoresDeEscala(
  const TVetor3D& fatores
)
{
  const char* mensagem = "Os fatores de escala devem ser finitos e positivos";
  ExigeDimensaoPositiva(fatores.x, mensagem);
  ExigeDimensaoPositiva(fatores.y, mensagem);
  ExigeDimensaoPositiva(fatores.z, mensagem);
}

//----------------------------------------------------------------------------------------------

void TPrimitiva::AcrescentaCirculo(
  std::vector<TArestaCandidata>& arestas,
  const TVetor3D& centro,
  const TVetor3D& u,
  const TVetor3D& v,
  double raio
)
{
  constexpr int SEGMENTOS = 48;
  constexpr double PI = 3.14159265358979323846;

  const auto ponto = [&](double angulo) {
    return centro + raio * std::cos(angulo) * u + raio * std::sin(angulo) * v;
  };

  for (int i = 0; i < SEGMENTOS; ++i) {
    const double inicio = 2.0 * PI * i / SEGMENTOS;
    const double fim = 2.0 * PI * (i + 1) / SEGMENTOS;
    arestas.push_back({ TAresta(ponto(inicio), ponto(fim)), ponto((inicio + fim) / 2.0) });
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
  EClassificacao classificacaoInterna,
  const TVizinhanca& vizinhancaInterna
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

  classificacao.push_back({ inicio, fim, classificacaoInterna, vizinhancaInterna });

  if (fim < 1.0 - TOLERANCIA_GEOMETRICA) {
    classificacao.push_back({ fim, 1.0, EClassificacao::OUT });
  }

  return classificacao;
}

//----------------------------------------------------------------------------------------------
