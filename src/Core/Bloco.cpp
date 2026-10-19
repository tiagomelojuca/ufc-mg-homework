#include "Bloco.h"

#include <algorithm>
#include <cmath>

namespace
{
  void AcrescentaIntersecoesComPlanos(
    double origem,
    double direcao,
    double meioLado,
    std::vector<double>& intersecoes
  )
  {
    if (direcao == 0.0) {
      return;
    }

    intersecoes.push_back((-meioLado - origem) / direcao);
    intersecoes.push_back((meioLado - origem) / direcao);
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

std::vector<double> TBloco::IntersecoesComFronteira(
  const TAresta& aresta
) const
{
  const TVetor3D origem = aresta.Inicio();
  const TVetor3D direcao = aresta.Direcao();

  std::vector<double> intersecoes;
  AcrescentaIntersecoesComPlanos(origem.x, direcao.x, ladoX / 2.0, intersecoes);
  AcrescentaIntersecoesComPlanos(origem.y, direcao.y, ladoY / 2.0, intersecoes);
  AcrescentaIntersecoesComPlanos(origem.z, direcao.z, ladoZ / 2.0, intersecoes);

  return intersecoes;
}

//----------------------------------------------------------------------------------------------
