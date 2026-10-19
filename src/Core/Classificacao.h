#ifndef CLASSIFICACAO_H_
#define CLASSIFICACAO_H_

#include <vector>

#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

// Distância abaixo da qual um ponto é considerado sobre a fronteira de um sólido.
constexpr double TOLERANCIA_GEOMETRICA = 1e-9;

//----------------------------------------------------------------------------------------------

enum class EClassificacao
{
  IN,
  ON,
  OUT
};

//----------------------------------------------------------------------------------------------

// Aresta finita orientada: E(t) = p1 + t (p2 - p1), com t em [0, 1].
class TAresta
{
  public:
    TAresta(
      const TVetor3D& inicio,
      const TVetor3D& fim
    );

    const TVetor3D& Inicio() const;
    const TVetor3D& Fim() const;
    TVetor3D Direcao() const;

    TVetor3D Ponto(
      double t
    ) const;

  private:
    TVetor3D inicio;
    TVetor3D fim;
};

//----------------------------------------------------------------------------------------------

struct TSegmentoClassificado
{
  double tInicio;
  double tFim;
  EClassificacao classificacao;
};

// Partes da aresta em ordem de t, sem sobreposição, cobrindo todo o intervalo [0, 1].
using TClassificacaoAresta = std::vector<TSegmentoClassificado>;

//----------------------------------------------------------------------------------------------

#endif
