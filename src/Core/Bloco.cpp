#include "Bloco.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
  // Intervalo de t em que a aresta está na faixa entre dois planos paralelos do bloco.
  struct TIntervaloFaixa
  {
    double tEntrada;
    double tSaida;
    bool sobrePlano;
  };

  TIntervaloFaixa IntersectaFaixa(
    double origem,
    double direcao,
    double meioLado
  )
  {
    const double infinito = std::numeric_limits<double>::infinity();

    // Aresta paralela aos dois planos: fica inteira dentro, inteira fora ou sobre um deles.
    if (direcao == 0.0) {
      const double distancia = std::abs(origem) - meioLado;
      if (distancia > TOLERANCIA_GEOMETRICA) {
        return { infinito, -infinito, false };
      }

      return { -infinito, infinito, distancia >= -TOLERANCIA_GEOMETRICA };
    }

    const double t1 = (-meioLado - origem) / direcao;
    const double t2 = (meioLado - origem) / direcao;

    return { std::min(t1, t2), std::max(t1, t2), false };
  }
}

//----------------------------------------------------------------------------------------------

TBloco::TBloco(
  double ladoX,
  double ladoY,
  double ladoZ
) :
  ladoX(ladoX),
  ladoY(ladoY),
  ladoZ(ladoZ)
{
  ExigeDimensaoPositiva(ladoX, "Os lados do bloco devem ser positivos");
  ExigeDimensaoPositiva(ladoY, "Os lados do bloco devem ser positivos");
  ExigeDimensaoPositiva(ladoZ, "Os lados do bloco devem ser positivos");
}

//----------------------------------------------------------------------------------------------

double TBloco::LadoX() const
{
  return ladoX;
}

//----------------------------------------------------------------------------------------------

double TBloco::LadoY() const
{
  return ladoY;
}

//----------------------------------------------------------------------------------------------

double TBloco::LadoZ() const
{
  return ladoZ;
}

//----------------------------------------------------------------------------------------------

ETipoPrimitiva TBloco::Tipo() const
{
  return ETipoPrimitiva::BLOCO;
}

//----------------------------------------------------------------------------------------------

EClassificacao TBloco::ClassificaPonto(
  const TVetor3D& ponto
) const
{
  // O bloco é a interseção das três faixas |x| <= ladoX/2, |y| <= ladoY/2 e |z| <= ladoZ/2;
  // o ponto está fora se exceder qualquer uma delas.
  const double excesso = std::max({
    std::abs(ponto.x) - ladoX / 2.0,
    std::abs(ponto.y) - ladoY / 2.0,
    std::abs(ponto.z) - ladoZ / 2.0
  });

  return ClassificaPorDistancia(excesso);
}

//----------------------------------------------------------------------------------------------

TClassificacaoAresta TBloco::ClassificaAresta(
  const TAresta& aresta
) const
{
  // O bloco é a interseção de três faixas, cada uma limitada por dois dos seis planos das faces.
  const TVetor3D origem = aresta.Inicio();
  const TVetor3D direcao = aresta.Direcao();

  const TIntervaloFaixa faixas[] = {
    IntersectaFaixa(origem.x, direcao.x, ladoX / 2.0),
    IntersectaFaixa(origem.y, direcao.y, ladoY / 2.0),
    IntersectaFaixa(origem.z, direcao.z, ladoZ / 2.0)
  };

  double tEntrada = -std::numeric_limits<double>::infinity();
  double tSaida = std::numeric_limits<double>::infinity();
  bool sobreFace = false;

  for (const TIntervaloFaixa& faixa : faixas) {
    tEntrada = std::max(tEntrada, faixa.tEntrada);
    tSaida = std::min(tSaida, faixa.tSaida);
    sobreFace = sobreFace || faixa.sobrePlano;
  }

  return ClassificaPorIntervalo(
    tEntrada,
    tSaida,
    sobreFace ? EClassificacao::ON : EClassificacao::IN
  );
}

//----------------------------------------------------------------------------------------------
