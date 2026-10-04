#include "IluminacaoPhong.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace
{
  double Produto(
    const TCoordenada3D& a,
    const TCoordenada3D& b
  )
  {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  TCoordenada3D Normaliza(
    const TCoordenada3D& vetor
  )
  {
    const double norma = std::sqrt(Produto(vetor, vetor));
    if (norma == 0.0 || !std::isfinite(norma)) {
      throw std::invalid_argument("O vetor deve ser finito e não nulo");
    }
    return { vetor.x / norma, vetor.y / norma, vetor.z / norma };
  }
}

//----------------------------------------------------------------------------------------------

TIluminacaoPhong::TIluminacaoPhong(
  const TCoordenada3D& posicaoLuz,
  const TCoordenada3D& direcaoObservador,
  const TMaterialPhong& material
) :
  posicaoLuz(posicaoLuz),
  direcaoObservador(Normaliza(direcaoObservador)),
  material(material)
{
}

//----------------------------------------------------------------------------------------------

TCorRGB TIluminacaoPhong::Calcula(
  const TCoordenada3D& ponto,
  const TCoordenada3D& normal,
  const TCorRGB& corBase
) const
{
  const TCoordenada3D n = Normaliza(normal);
  const TCoordenada3D paraLuz = { posicaoLuz.x - ponto.x, posicaoLuz.y - ponto.y, posicaoLuz.z - ponto.z };
  const TCoordenada3D l = Normaliza(paraLuz);
  const double difusa = std::max(0.0, Produto(n, l));

  double especular = 0.0;
  if (difusa > 0.0) {
    // R = 2 (N·L) N - L
    const TCoordenada3D r = { 2.0 * difusa * n.x - l.x, 2.0 * difusa * n.y - l.y, 2.0 * difusa * n.z - l.z };
    especular = std::pow(std::max(0.0, Produto(r, direcaoObservador)), material.brilho);
  }

  const double intensidade = material.ambiente + material.difuso * difusa;
  const double brilho = material.especular * especular;
  return {
    std::min(1.0, corBase.r * intensidade + brilho),
    std::min(1.0, corBase.g * intensidade + brilho),
    std::min(1.0, corBase.b * intensidade + brilho)
  };
}

//----------------------------------------------------------------------------------------------
