#ifndef CLASSIFICACAO_H_
#define CLASSIFICACAO_H_

#include <optional>
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

// Vizinhança de um trecho ON: as direções que, a partir de um ponto do trecho, entram no sólido.
// Cada cone é a interseção dos semiespaços n · v <= 0, um para cada normal exterior das faces
// que contêm o trecho; a vizinhança é a união dos cones.
using TConeVizinhanca = std::vector<TVetor3D>;
using TVizinhanca = std::vector<TConeVizinhanca>;

//----------------------------------------------------------------------------------------------

// Aresta do contorno de uma primitiva, candidata ao aramado. Uma curva é aproximada por cordas, que
// passam por dentro do sólido; pontoDaCurva é o ponto da curva no meio da corda, este sim sobre a
// fronteira. Arestas retas não têm esse ponto.
struct TArestaCandidata
{
  TAresta aresta;
  std::optional<TVetor3D> pontoDaCurva;
};

//----------------------------------------------------------------------------------------------

struct TSegmentoClassificado
{
  double tInicio;
  double tFim;
  EClassificacao classificacao;

  // Preenchida apenas em trechos ON; vazia quando a vizinhança é desconhecida.
  TVizinhanca vizinhanca = {};
};

// Partes da aresta em ordem de t, sem sobreposição, cobrindo todo o intervalo [0, 1].
using TClassificacaoAresta = std::vector<TSegmentoClassificado>;

//----------------------------------------------------------------------------------------------

#endif
