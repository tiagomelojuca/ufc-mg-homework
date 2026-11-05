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

    // +1 ou -1 quando a aresta está sobre o plano em +meioLado ou em -meioLado; 0 caso contrário.
    double ladoDoPlano;
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
        return { infinito, -infinito, 0.0 };
      }

      const bool sobrePlano = distancia >= -TOLERANCIA_GEOMETRICA;
      return { -infinito, infinito, sobrePlano ? (origem > 0.0 ? 1.0 : -1.0) : 0.0 };
    }

    const double t1 = (-meioLado - origem) / direcao;
    const double t2 = (meioLado - origem) / direcao;

    return { std::min(t1, t2), std::max(t1, t2), 0.0 };
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

  const TVetor3D eixos[] = { { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 } };

  double tEntrada = -std::numeric_limits<double>::infinity();
  double tSaida = std::numeric_limits<double>::infinity();

  // Normais exteriores das faces que contêm a aresta: uma sobre a face, duas sobre um lado do bloco.
  TConeVizinhanca normais;

  for (int i = 0; i < 3; ++i) {
    tEntrada = std::max(tEntrada, faixas[i].tEntrada);
    tSaida = std::min(tSaida, faixas[i].tSaida);
    if (faixas[i].ladoDoPlano != 0.0) {
      normais.push_back(faixas[i].ladoDoPlano * eixos[i]);
    }
  }

  if (normais.empty()) {
    return ClassificaPorIntervalo(
      tEntrada,
      tSaida,
      EClassificacao::IN
    );
  }

  return ClassificaPorIntervalo(
    tEntrada,
    tSaida,
    EClassificacao::ON,
    { normais }
  );
}

//----------------------------------------------------------------------------------------------

TVetor3D TBloco::Normal(
  const TVetor3D& ponto
) const
{
  // A face do ponto é a do eixo em que ele mais se aproxima do plano (ou mais o excede).
  const double excessos[] = {
    std::abs(ponto.x) - ladoX / 2.0,
    std::abs(ponto.y) - ladoY / 2.0,
    std::abs(ponto.z) - ladoZ / 2.0
  };

  if (excessos[0] >= excessos[1] && excessos[0] >= excessos[2]) {
    return { ponto.x >= 0.0 ? 1.0 : -1.0, 0.0, 0.0 };
  }

  if (excessos[1] >= excessos[2]) {
    return { 0.0, ponto.y >= 0.0 ? 1.0 : -1.0, 0.0 };
  }

  return { 0.0, 0.0, ponto.z >= 0.0 ? 1.0 : -1.0 };
}

//----------------------------------------------------------------------------------------------

std::shared_ptr<const TPrimitiva> TBloco::Escalada(
  const TVetor3D& fatores
) const
{
  ExigeFatoresDeEscala(fatores);

  return std::make_shared<TBloco>(ladoX * fatores.x, ladoY * fatores.y, ladoZ * fatores.z);
}

//----------------------------------------------------------------------------------------------

TCaixa TBloco::CaixaCanonica() const
{
  const TVetor3D meiosLados = { ladoX / 2.0, ladoY / 2.0, ladoZ / 2.0 };
  return { -1.0 * meiosLados, meiosLados };
}

//----------------------------------------------------------------------------------------------

std::vector<TArestaCandidata> TBloco::ArestasDoContorno() const
{
  const TCaixa caixa = CaixaCanonica();
  const auto canto = [&caixa](int indice) {
    return TVetor3D {
      (indice & 1) ? caixa.maximo.x : caixa.minimo.x,
      (indice & 2) ? caixa.maximo.y : caixa.minimo.y,
      (indice & 4) ? caixa.maximo.z : caixa.minimo.z
    };
  };

  // Os doze lados ligam cantos cujos índices diferem em um único bit, isto é, em um só eixo.
  std::vector<TArestaCandidata> arestas;
  for (int indice = 0; indice < 8; ++indice) {
    for (int bit : { 1, 2, 4 }) {
      if ((indice & bit) == 0) {
        arestas.push_back({ TAresta(canto(indice), canto(indice | bit)), std::nullopt });
      }
    }
  }

  return arestas;
}

//----------------------------------------------------------------------------------------------
