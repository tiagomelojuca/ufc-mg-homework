#include "Esfera.h"

#include <cmath>

//----------------------------------------------------------------------------------------------

TEsfera::TEsfera(
  double raio
) :
  raio(raio)
{
  ExigeDimensaoPositiva(raio, "O raio da esfera deve ser positivo");
}

//----------------------------------------------------------------------------------------------

double TEsfera::Raio() const
{
  return raio;
}

//----------------------------------------------------------------------------------------------

ETipoPrimitiva TEsfera::Tipo() const
{
  return ETipoPrimitiva::ESFERA;
}

//----------------------------------------------------------------------------------------------

EClassificacao TEsfera::ClassificaPonto(
  const TVetor3D& ponto
) const
{
  return ClassificaPorDistancia(std::sqrt(ProdutoEscalar(ponto, ponto)) - raio);
}

//----------------------------------------------------------------------------------------------

std::vector<double> TEsfera::IntersecoesComFronteira(
  const TAresta& aresta
) const
{
  // Substituindo E(t) = p + t d em x² + y² + z² = r²: (d·d) t² + 2 (p·d) t + (p·p - r²) = 0.
  const TVetor3D p = aresta.Inicio();
  const TVetor3D d = aresta.Direcao();

  const double a = ProdutoEscalar(d, d);
  const double b = 2.0 * ProdutoEscalar(p, d);
  const double c = ProdutoEscalar(p, p) - raio * raio;

  if (a == 0.0) {
    return {};
  }

  const double discriminante = b * b - 4.0 * a * c;
  if (discriminante < 0.0) {
    return {};
  }

  const double raizDiscriminante = std::sqrt(discriminante);
  return {
    (-b - raizDiscriminante) / (2.0 * a),
    (-b + raizDiscriminante) / (2.0 * a)
  };
}

//----------------------------------------------------------------------------------------------
