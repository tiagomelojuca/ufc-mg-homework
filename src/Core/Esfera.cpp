#include "Esfera.h"

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
}

//----------------------------------------------------------------------------------------------

TEsfera::TEsfera(
  const TCoordenada3D& centro,
  double raio
) :
  centro(centro),
  raio(raio)
{
  if (raio <= 0.0) {
    throw std::invalid_argument("O raio da esfera deve ser positivo");
  }
}

//----------------------------------------------------------------------------------------------

const TCoordenada3D& TEsfera::Centro() const
{
  return centro;
}

//----------------------------------------------------------------------------------------------

double TEsfera::Raio() const
{
  return raio;
}

//----------------------------------------------------------------------------------------------

TClassificadorEsferaOctree::TClassificadorEsferaOctree(
  const TEsfera& esfera
) :
  esfera(esfera)
{
}

//----------------------------------------------------------------------------------------------

EEstadoNoOctree TClassificadorEsferaOctree::Classifica(
  const TCubo& regiao
) const
{
  const TIntervalo regiaoX = CriaIntervalo(regiao.Centro().x, regiao.Lado());
  const TIntervalo regiaoY = CriaIntervalo(regiao.Centro().y, regiao.Lado());
  const TIntervalo regiaoZ = CriaIntervalo(regiao.Centro().z, regiao.Lado());

  const double distanciaX = DistanciaAoIntervalo(esfera.Centro().x, regiaoX);
  const double distanciaY = DistanciaAoIntervalo(esfera.Centro().y, regiaoY);
  const double distanciaZ = DistanciaAoIntervalo(esfera.Centro().z, regiaoZ);
  const double menorDistanciaQuadrada =
    distanciaX * distanciaX +
    distanciaY * distanciaY +
    distanciaZ * distanciaZ;
  const double raioQuadrado = esfera.Raio() * esfera.Raio();

  if (menorDistanciaQuadrada >= raioQuadrado) {
    return EEstadoNoOctree::VAZIO;
  }

  const double maiorDistanciaX = MaiorDistanciaAoIntervalo(esfera.Centro().x, regiaoX);
  const double maiorDistanciaY = MaiorDistanciaAoIntervalo(esfera.Centro().y, regiaoY);
  const double maiorDistanciaZ = MaiorDistanciaAoIntervalo(esfera.Centro().z, regiaoZ);
  const double maiorDistanciaQuadrada =
    maiorDistanciaX * maiorDistanciaX +
    maiorDistanciaY * maiorDistanciaY +
    maiorDistanciaZ * maiorDistanciaZ;

  if (maiorDistanciaQuadrada <= raioQuadrado) {
    return EEstadoNoOctree::CHEIO;
  }

  return EEstadoNoOctree::PARCIAL;
}

//----------------------------------------------------------------------------------------------

bool TClassificadorEsferaOctree::EstaContido(
  const TCubo& dominio
) const
{
  const TIntervalo dominioX = CriaIntervalo(dominio.Centro().x, dominio.Lado());
  const TIntervalo dominioY = CriaIntervalo(dominio.Centro().y, dominio.Lado());
  const TIntervalo dominioZ = CriaIntervalo(dominio.Centro().z, dominio.Lado());
  const TIntervalo esferaX = {
    esfera.Centro().x - esfera.Raio(),
    esfera.Centro().x + esfera.Raio()
  };
  const TIntervalo esferaY = {
    esfera.Centro().y - esfera.Raio(),
    esfera.Centro().y + esfera.Raio()
  };
  const TIntervalo esferaZ = {
    esfera.Centro().z - esfera.Raio(),
    esfera.Centro().z + esfera.Raio()
  };

  return
    EstaDentro(esferaX, dominioX) &&
    EstaDentro(esferaY, dominioY) &&
    EstaDentro(esferaZ, dominioZ);
}

//----------------------------------------------------------------------------------------------
