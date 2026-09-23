#include "Bloco.h"

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

  bool EstaFora(
    const TIntervalo& regiao,
    const TIntervalo& bloco
  )
  {
    return regiao.maximo <= bloco.minimo || regiao.minimo >= bloco.maximo;
  }

  bool EstaDentro(
    const TIntervalo& regiao,
    const TIntervalo& bloco
  )
  {
    return regiao.minimo >= bloco.minimo && regiao.maximo <= bloco.maximo;
  }
}

//----------------------------------------------------------------------------------------------

TBloco::TBloco(
  const TCoordenada3D& centro,
  double ladoX,
  double ladoY,
  double ladoZ
) :
  centro(centro),
  ladoX(ladoX),
  ladoY(ladoY),
  ladoZ(ladoZ)
{
  if (ladoX <= 0.0 || ladoY <= 0.0 || ladoZ <= 0.0) {
    throw std::invalid_argument("Os lados do bloco devem ser positivos");
  }
}

//----------------------------------------------------------------------------------------------

const TCoordenada3D& TBloco::Centro() const
{
  return centro;
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

TClassificadorBlocoOctree::TClassificadorBlocoOctree(
  const TBloco& bloco
) :
  bloco(bloco)
{
}

//----------------------------------------------------------------------------------------------

EEstadoNoOctree TClassificadorBlocoOctree::Classifica(
  const TCubo& regiao
) const
{
  const TIntervalo regiaoX = CriaIntervalo(regiao.Centro().x, regiao.Lado());
  const TIntervalo regiaoY = CriaIntervalo(regiao.Centro().y, regiao.Lado());
  const TIntervalo regiaoZ = CriaIntervalo(regiao.Centro().z, regiao.Lado());
  const TIntervalo blocoX = CriaIntervalo(bloco.Centro().x, bloco.LadoX());
  const TIntervalo blocoY = CriaIntervalo(bloco.Centro().y, bloco.LadoY());
  const TIntervalo blocoZ = CriaIntervalo(bloco.Centro().z, bloco.LadoZ());

  if (
    EstaFora(regiaoX, blocoX) ||
    EstaFora(regiaoY, blocoY) ||
    EstaFora(regiaoZ, blocoZ)
  ) {
    return EEstadoNoOctree::VAZIO;
  }

  if (
    EstaDentro(regiaoX, blocoX) &&
    EstaDentro(regiaoY, blocoY) &&
    EstaDentro(regiaoZ, blocoZ)
  ) {
    return EEstadoNoOctree::CHEIO;
  }

  return EEstadoNoOctree::PARCIAL;
}

//----------------------------------------------------------------------------------------------

bool TClassificadorBlocoOctree::EstaContido(
  const TCubo& dominio
) const
{
  const TIntervalo dominioX = CriaIntervalo(dominio.Centro().x, dominio.Lado());
  const TIntervalo dominioY = CriaIntervalo(dominio.Centro().y, dominio.Lado());
  const TIntervalo dominioZ = CriaIntervalo(dominio.Centro().z, dominio.Lado());
  const TIntervalo blocoX = CriaIntervalo(bloco.Centro().x, bloco.LadoX());
  const TIntervalo blocoY = CriaIntervalo(bloco.Centro().y, bloco.LadoY());
  const TIntervalo blocoZ = CriaIntervalo(bloco.Centro().z, bloco.LadoZ());

  return
    EstaDentro(blocoX, dominioX) &&
    EstaDentro(blocoY, dominioY) &&
    EstaDentro(blocoZ, dominioZ);
}

//----------------------------------------------------------------------------------------------
