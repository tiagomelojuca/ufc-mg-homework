#include "Cilindro.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace
{
  // Intervalo de t em que a aresta está em um dos semiespaços; normal é a do plano ou da
  // superfície que contém a aresta inteira, ou nula se ela não está sobre a fronteira.
  struct TIntervaloSemiespaco
  {
    double tEntrada;
    double tSaida;
    TVetor3D normal;
  };

  //--------------------------------------------------------------------------------------------

  const double INFINITO = std::numeric_limits<double>::infinity();
  const TVetor3D NORMAL_NULA = { 0.0, 0.0, 0.0 };

  //--------------------------------------------------------------------------------------------

  // H2 ∩ H3: dois testes raio–plano, com os planos z = 0 e z = altura.
  TIntervaloSemiespaco IntersectaFaixaZ(
    double origem,
    double direcao,
    double altura
  )
  {
    // Aresta paralela às bases: inteira dentro da faixa, inteira fora ou sobre uma das bases.
    if (direcao == 0.0) {
      if (origem < -TOLERANCIA_GEOMETRICA || origem > altura + TOLERANCIA_GEOMETRICA) {
        return { INFINITO, -INFINITO, NORMAL_NULA };
      }

      if (std::abs(origem) <= TOLERANCIA_GEOMETRICA) {
        return { -INFINITO, INFINITO, { 0.0, 0.0, -1.0 } };
      }

      if (std::abs(origem - altura) <= TOLERANCIA_GEOMETRICA) {
        return { -INFINITO, INFINITO, { 0.0, 0.0, 1.0 } };
      }

      return { -INFINITO, INFINITO, NORMAL_NULA };
    }

    const double t1 = -origem / direcao;
    const double t2 = (altura - origem) / direcao;

    return { std::min(t1, t2), std::max(t1, t2), NORMAL_NULA };
  }

  //--------------------------------------------------------------------------------------------

  // H1: substituindo E(t) = p + t d em x² + y² = r²:
  // (dx² + dy²) t² + 2 (px dx + py dy) t + (px² + py² - r²) = 0.
  TIntervaloSemiespaco IntersectaSuperficieLateral(
    const TVetor3D& p,
    const TVetor3D& d,
    double raio
  )
  {
    const double a = d.x * d.x + d.y * d.y;
    const double b = 2.0 * (p.x * d.x + p.y * d.y);
    const double c = p.x * p.x + p.y * p.y - raio * raio;

    // Aresta paralela ao eixo (a menos de arredondamento): fica inteira dentro, fora ou sobre a
    // superfície lateral, conforme a sua distância ao eixo.
    if (a <= TOLERANCIA_GEOMETRICA * TOLERANCIA_GEOMETRICA * ProdutoEscalar(d, d)) {
      const double distanciaAoEixo = std::sqrt(p.x * p.x + p.y * p.y);
      if (distanciaAoEixo - raio > TOLERANCIA_GEOMETRICA) {
        return { INFINITO, -INFINITO, NORMAL_NULA };
      }

      if (distanciaAoEixo - raio >= -TOLERANCIA_GEOMETRICA) {
        return { -INFINITO, INFINITO, { p.x / distanciaAoEixo, p.y / distanciaAoEixo, 0.0 } };
      }

      return { -INFINITO, INFINITO, NORMAL_NULA };
    }

    // Sem solução, a reta não intercepta; com raiz dupla, apenas tangencia.
    const double discriminante = b * b - 4.0 * a * c;
    if (discriminante <= 0.0) {
      return { INFINITO, -INFINITO, NORMAL_NULA };
    }

    const double raizDiscriminante = std::sqrt(discriminante);
    return {
      (-b - raizDiscriminante) / (2.0 * a),
      (-b + raizDiscriminante) / (2.0 * a),
      NORMAL_NULA
    };
  }
}

//----------------------------------------------------------------------------------------------

TCilindro::TCilindro(
  double raio,
  double altura
) :
  raio(raio),
  altura(altura)
{
  ExigeDimensaoPositiva(raio, "O raio do cilindro deve ser positivo");
  ExigeDimensaoPositiva(altura, "A altura do cilindro deve ser positiva");
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

ETipoPrimitiva TCilindro::Tipo() const
{
  return ETipoPrimitiva::CILINDRO;
}

//----------------------------------------------------------------------------------------------

EClassificacao TCilindro::ClassificaPonto(
  const TVetor3D& ponto
) const
{
  // O ponto está fora se exceder qualquer um dos três semiespaços.
  const double excesso = std::max({
    std::sqrt(ponto.x * ponto.x + ponto.y * ponto.y) - raio,
    -ponto.z,
    ponto.z - altura
  });

  return ClassificaPorDistancia(excesso);
}

//----------------------------------------------------------------------------------------------

TClassificacaoAresta TCilindro::ClassificaAresta(
  const TAresta& aresta
) const
{
  const TIntervaloSemiespaco intervalos[] = {
    IntersectaFaixaZ(aresta.Inicio().z, aresta.Direcao().z, altura),
    IntersectaSuperficieLateral(aresta.Inicio(), aresta.Direcao(), raio)
  };

  double tEntrada = -INFINITO;
  double tSaida = INFINITO;

  // Normais exteriores das superfícies que contêm a aresta inteira.
  TConeVizinhanca normais;

  for (const TIntervaloSemiespaco& intervalo : intervalos) {
    tEntrada = std::max(tEntrada, intervalo.tEntrada);
    tSaida = std::min(tSaida, intervalo.tSaida);
    if (Norma(intervalo.normal) > 0.0) {
      normais.push_back(intervalo.normal);
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

TVetor3D TCilindro::Normal(
  const TVetor3D& ponto
) const
{
  // A superfície do ponto é aquela de que ele mais se aproxima (ou que mais excede).
  const double distanciaAoEixo = std::sqrt(ponto.x * ponto.x + ponto.y * ponto.y);
  const double excessoLateral = distanciaAoEixo - raio;
  const double excessoBase = -ponto.z;
  const double excessoTopo = ponto.z - altura;

  if (excessoLateral >= excessoBase && excessoLateral >= excessoTopo && distanciaAoEixo > 0.0) {
    return { ponto.x / distanciaAoEixo, ponto.y / distanciaAoEixo, 0.0 };
  }

  return excessoBase >= excessoTopo ? TVetor3D { 0.0, 0.0, -1.0 } : TVetor3D { 0.0, 0.0, 1.0 };
}

//----------------------------------------------------------------------------------------------

std::shared_ptr<const TPrimitiva> TCilindro::Escalada(
  const TVetor3D& fatores
) const
{
  ExigeFatoresDeEscala(fatores);

  if (fatores.x != fatores.y) {
    throw std::invalid_argument("O cilindro admite apenas a mesma escala em x e y");
  }

  return std::make_shared<TCilindro>(raio * fatores.x, altura * fatores.z);
}

//----------------------------------------------------------------------------------------------

TCaixa TCilindro::CaixaCanonica() const
{
  return { { -raio, -raio, 0.0 }, { raio, raio, altura } };
}

//----------------------------------------------------------------------------------------------

std::vector<TArestaCandidata> TCilindro::ArestasDoContorno() const
{
  constexpr double PI = 3.14159265358979323846;
  std::vector<TArestaCandidata> arestas;

  // As bordas das bases são as arestas próprias do cilindro.
  AcrescentaCirculo(arestas, { 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, raio);
  AcrescentaCirculo(arestas, { 0.0, 0.0, altura }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, raio);

  // Oito geratrizes retas, a cada 45 graus, desenham a superfície lateral.
  for (int i = 0; i < 8; ++i) {
    const double angulo = PI * i / 4.0;
    const double x = raio * std::cos(angulo);
    const double y = raio * std::sin(angulo);
    arestas.push_back({ TAresta({ x, y, 0.0 }, { x, y, altura }), std::nullopt });
  }

  return arestas;
}

//----------------------------------------------------------------------------------------------
