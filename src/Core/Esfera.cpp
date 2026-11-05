#include "Esfera.h"

#include <cmath>
#include <stdexcept>

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

TClassificacaoAresta TEsfera::ClassificaAresta(
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
    return { { 0.0, 1.0, ClassificaPonto(p) } };
  }

  // Sem solução, a aresta não intercepta; com raiz dupla, apenas tangencia.
  const double discriminante = b * b - 4.0 * a * c;
  if (discriminante <= 0.0) {
    return { { 0.0, 1.0, EClassificacao::OUT } };
  }

  // Com duas raízes, a reta está dentro da esfera entre elas.
  const double raizDiscriminante = std::sqrt(discriminante);
  const double t1 = (-b - raizDiscriminante) / (2.0 * a);
  const double t2 = (-b + raizDiscriminante) / (2.0 * a);

  return ClassificaPorIntervalo(t1, t2, EClassificacao::IN);
}

//----------------------------------------------------------------------------------------------

TVetor3D TEsfera::Normal(
  const TVetor3D& ponto
) const
{
  return Normaliza(ponto);
}

//----------------------------------------------------------------------------------------------

std::shared_ptr<const TPrimitiva> TEsfera::Escalada(
  const TVetor3D& fatores
) const
{
  ExigeFatoresDeEscala(fatores);

  // Com fatores diferentes o resultado seria um elipsoide, que não é uma esfera.
  if (fatores.x != fatores.y || fatores.y != fatores.z) {
    throw std::invalid_argument("A esfera admite apenas escala uniforme");
  }

  return std::make_shared<TEsfera>(raio * fatores.x);
}

//----------------------------------------------------------------------------------------------

TCaixa TEsfera::CaixaCanonica() const
{
  return { { -raio, -raio, -raio }, { raio, raio, raio } };
}

//----------------------------------------------------------------------------------------------

std::vector<TArestaCandidata> TEsfera::ArestasDoContorno() const
{
  constexpr double PI = 3.14159265358979323846;
  std::vector<TArestaCandidata> arestas;

  // Seis meridianos, a cada 30 graus em torno do eixo z.
  for (int i = 0; i < 6; ++i) {
    const double angulo = PI * i / 6.0;
    AcrescentaCirculo(
      arestas,
      { 0.0, 0.0, 0.0 },
      { std::cos(angulo), std::sin(angulo), 0.0 },
      { 0.0, 0.0, 1.0 },
      raio
    );
  }

  // Paralelos nas latitudes de -60 a 60 graus.
  for (int latitude = -60; latitude <= 60; latitude += 30) {
    const double angulo = latitude * PI / 180.0;
    AcrescentaCirculo(
      arestas,
      { 0.0, 0.0, raio * std::sin(angulo) },
      { 1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      raio * std::cos(angulo)
    );
  }

  return arestas;
}

//----------------------------------------------------------------------------------------------
