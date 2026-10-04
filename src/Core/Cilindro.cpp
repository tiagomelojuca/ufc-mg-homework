#include "Cilindro.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace
{
  struct TIntervalo
  {
    double minimo;
    double maximo;
  };

  // Coordenadas reorganizadas como (eixo do cilindro, primeiro eixo radial, segundo eixo radial).
  struct TCoordenadasCilindro
  {
    double axial;
    double radialA;
    double radialB;
  };

  TCoordenadasCilindro Reorganiza(
    const TCoordenada3D& ponto,
    EEixo eixo
  )
  {
    switch (eixo) {
      case EEixo::X: return { ponto.x, ponto.y, ponto.z };
      case EEixo::Y: return { ponto.y, ponto.x, ponto.z };
      default: return { ponto.z, ponto.x, ponto.y };
    }
  }

  TIntervalo CriaIntervalo(
    double centro,
    double lado
  )
  {
    const double meioLado = lado / 2.0;
    return { centro - meioLado, centro + meioLado };
  }

  double DistanciaAoIntervalo(
    double coordenada,
    const TIntervalo& intervalo
  )
  {
    if (coordenada < intervalo.minimo) {
      return intervalo.minimo - coordenada;
    }

    if (coordenada > intervalo.maximo) {
      return coordenada - intervalo.maximo;
    }

    return 0.0;
  }

  double MaiorDistanciaAoIntervalo(
    double coordenada,
    const TIntervalo& intervalo
  )
  {
    return std::max(
      std::abs(coordenada - intervalo.minimo),
      std::abs(coordenada - intervalo.maximo)
    );
  }

  bool EstaDentro(
    const TIntervalo& interno,
    const TIntervalo& externo
  )
  {
    return interno.minimo >= externo.minimo && interno.maximo <= externo.maximo;
  }

  bool EstaFora(
    const TIntervalo& primeiro,
    const TIntervalo& segundo
  )
  {
    return primeiro.maximo <= segundo.minimo || primeiro.minimo >= segundo.maximo;
  }
}

//----------------------------------------------------------------------------------------------

TCilindro::TCilindro(
  const TCoordenada3D& centro,
  double raio,
  double altura,
  EEixo eixo
) :
  centro(centro),
  raio(raio),
  altura(altura),
  eixo(eixo)
{
  if (raio <= 0.0) {
    throw std::invalid_argument("O raio do cilindro deve ser positivo");
  }

  if (altura <= 0.0) {
    throw std::invalid_argument("A altura do cilindro deve ser positiva");
  }
}

//----------------------------------------------------------------------------------------------

const TCoordenada3D& TCilindro::Centro() const
{
  return centro;
}

//----------------------------------------------------------------------------------------------

double TCilindro::Raio() const
{
  return raio;
}

//----------------------------------------------------------------------------------------------

double TCilindro::Altura() const
{
  return altura;
}

//----------------------------------------------------------------------------------------------

EEixo TCilindro::Eixo() const
{
  return eixo;
}

//----------------------------------------------------------------------------------------------

TClassificadorCilindroOctree::TClassificadorCilindroOctree(
  const TCilindro& cilindro
) :
  cilindro(cilindro)
{
}

//----------------------------------------------------------------------------------------------

EEstadoNoOctree TClassificadorCilindroOctree::Classifica(
  const TCubo& regiao
) const
{
  const TCoordenadasCilindro centroRegiao = Reorganiza(regiao.Centro(), cilindro.Eixo());
  const TCoordenadasCilindro centroCilindro = Reorganiza(cilindro.Centro(), cilindro.Eixo());
  const TIntervalo regiaoAxial = CriaIntervalo(centroRegiao.axial, regiao.Lado());
  const TIntervalo regiaoA = CriaIntervalo(centroRegiao.radialA, regiao.Lado());
  const TIntervalo regiaoB = CriaIntervalo(centroRegiao.radialB, regiao.Lado());
  const TIntervalo cilindroAxial = CriaIntervalo(centroCilindro.axial, cilindro.Altura());
  const double raioQuadrado = cilindro.Raio() * cilindro.Raio();

  // A seção transversal da célula é um quadrado; comparamos o disco com o ponto mais próximo e o mais distante.
  const double menorA = DistanciaAoIntervalo(centroCilindro.radialA, regiaoA);
  const double menorB = DistanciaAoIntervalo(centroCilindro.radialB, regiaoB);
  if (EstaFora(regiaoAxial, cilindroAxial) || menorA * menorA + menorB * menorB >= raioQuadrado) {
    return EEstadoNoOctree::VAZIO;
  }

  const double maiorA = MaiorDistanciaAoIntervalo(centroCilindro.radialA, regiaoA);
  const double maiorB = MaiorDistanciaAoIntervalo(centroCilindro.radialB, regiaoB);
  if (EstaDentro(regiaoAxial, cilindroAxial) && maiorA * maiorA + maiorB * maiorB <= raioQuadrado) {
    return EEstadoNoOctree::CHEIO;
  }

  return EEstadoNoOctree::PARCIAL;
}

//----------------------------------------------------------------------------------------------

bool TClassificadorCilindroOctree::EstaContido(
  const TCubo& dominio
) const
{
  const TCoordenadasCilindro centroDominio = Reorganiza(dominio.Centro(), cilindro.Eixo());
  const TCoordenadasCilindro centroCilindro = Reorganiza(cilindro.Centro(), cilindro.Eixo());
  const double diametro = 2.0 * cilindro.Raio();

  return
    EstaDentro(CriaIntervalo(centroCilindro.axial, cilindro.Altura()), CriaIntervalo(centroDominio.axial, dominio.Lado())) &&
    EstaDentro(CriaIntervalo(centroCilindro.radialA, diametro), CriaIntervalo(centroDominio.radialA, dominio.Lado())) &&
    EstaDentro(CriaIntervalo(centroCilindro.radialB, diametro), CriaIntervalo(centroDominio.radialB, dominio.Lado()));
}

//----------------------------------------------------------------------------------------------
