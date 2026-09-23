#include "Octree.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace
{
  int CalculaProfundidade(
    const TNoOctree& no
  )
  {
    if (no.EhFolha()) {
      return 1;
    }

    int maiorProfundidadeFilho = 0;

    for (std::size_t indice = 0; indice < 8; ++indice) {
      maiorProfundidadeFilho = std::max(
        maiorProfundidadeFilho,
        CalculaProfundidade(no.Filho(indice))
      );
    }

    return maiorProfundidadeFilho + 1;
  }
}

//----------------------------------------------------------------------------------------------

TCubo::TCubo(
  const TCoordenada3D& centro,
  double lado
) :
  centro(centro),
  lado(lado)
{
  if (lado <= 0.0) {
    throw std::invalid_argument("O lado do cubo deve ser positivo");
  }
}

//----------------------------------------------------------------------------------------------

const TCoordenada3D& TCubo::Centro() const
{
  return centro;
}

//----------------------------------------------------------------------------------------------

double TCubo::Lado() const
{
  return lado;
}

//----------------------------------------------------------------------------------------------

TCubo TCubo::Octante(
  std::size_t indice
) const
{
  if (indice >= 8) {
    throw std::out_of_range("O indice do octante deve estar entre 0 e 7");
  }

  const double ladoFilho = lado / 2.0;
  const double deslocamento = lado / 4.0;
  const double sinalX = (indice & 1) == 0 ? -1.0 : 1.0;
  const double sinalZ = (indice & 2) == 0 ? -1.0 : 1.0;
  const double sinalY = (indice & 4) == 0 ? -1.0 : 1.0;

  return TCubo(
    {
      centro.x + sinalX * deslocamento,
      centro.y + sinalY * deslocamento,
      centro.z + sinalZ * deslocamento
    },
    ladoFilho
  );
}

//----------------------------------------------------------------------------------------------

TNoOctree::TNoOctree(
  const TCubo& regiao,
  EEstadoNoOctree estado
) :
  estado(EEstadoNoOctree::CHEIO),
  regiao(regiao)
{
  DefineEstado(estado);
}

//----------------------------------------------------------------------------------------------

EEstadoNoOctree TNoOctree::Estado() const
{
  return estado;
}

//----------------------------------------------------------------------------------------------

const TCubo& TNoOctree::Regiao() const
{
  return regiao;
}

//----------------------------------------------------------------------------------------------

bool TNoOctree::EhFolha() const
{
  return estado != EEstadoNoOctree::PARCIAL;
}

//----------------------------------------------------------------------------------------------

const TNoOctree& TNoOctree::Filho(
  std::size_t indice
) const
{
  if (indice >= filhos.size()) {
    throw std::out_of_range("O indice do filho deve estar entre 0 e 7");
  }

  if (filhos[indice] == nullptr) {
    throw std::logic_error("Um no folha nao possui filhos");
  }

  return *filhos[indice];
}

//----------------------------------------------------------------------------------------------

TNoOctree& TNoOctree::Filho(
  std::size_t indice
)
{
  if (indice >= filhos.size()) {
    throw std::out_of_range("O indice do filho deve estar entre 0 e 7");
  }

  if (filhos[indice] == nullptr) {
    throw std::logic_error("Um no folha nao possui filhos");
  }

  return *filhos[indice];
}

//----------------------------------------------------------------------------------------------

void TNoOctree::DefineEstado(
  EEstadoNoOctree novoEstado
)
{
  if (novoEstado == EEstadoNoOctree::PARCIAL) {
    Subdivide();
    return;
  }

  estado = novoEstado;
  for (std::unique_ptr<TNoOctree>& filho : filhos) {
    filho.reset();
  }
}

//----------------------------------------------------------------------------------------------

void TNoOctree::Subdivide()
{
  estado = EEstadoNoOctree::PARCIAL;

  for (std::size_t indice = 0; indice < filhos.size(); ++indice) {
    filhos[indice] = std::make_unique<TNoOctree>(regiao.Octante(indice));
  }
}

//----------------------------------------------------------------------------------------------

TConfiguracaoOctree::TConfiguracaoOctree() :
  dominio({ 0.0, 0.0, 0.0 }, 2.0),
  profundidadeMaxima(5)
{
}

//----------------------------------------------------------------------------------------------

TConfiguracaoOctree::TConfiguracaoOctree(
  const TCubo& dominio,
  int profundidadeMaxima
) :
  dominio(dominio),
  profundidadeMaxima(profundidadeMaxima)
{
  if (profundidadeMaxima < 1) {
    throw std::invalid_argument("A profundidade maxima deve ser pelo menos 1");
  }
}

//----------------------------------------------------------------------------------------------

const TCubo& TConfiguracaoOctree::Dominio() const
{
  return dominio;
}

//----------------------------------------------------------------------------------------------

int TConfiguracaoOctree::ProfundidadeMaxima() const
{
  return profundidadeMaxima;
}

//----------------------------------------------------------------------------------------------

TOctree::TOctree(
  const TConfiguracaoOctree& configuracao
) :
  configuracao(configuracao),
  raiz(configuracao.Dominio())
{
}

//----------------------------------------------------------------------------------------------

TOctree::TOctree(
  const TConfiguracaoOctree& configuracao,
  TNoOctree&& raiz
) :
  configuracao(configuracao),
  raiz(std::move(raiz))
{
  const TCubo& dominio = configuracao.Dominio();
  const TCubo& regiaoRaiz = this->raiz.Regiao();

  if (
    regiaoRaiz.Centro().x != dominio.Centro().x ||
    regiaoRaiz.Centro().y != dominio.Centro().y ||
    regiaoRaiz.Centro().z != dominio.Centro().z ||
    regiaoRaiz.Lado() != dominio.Lado()
  ) {
    throw std::invalid_argument("A raiz deve representar o dominio da octree");
  }

  if (CalculaProfundidade(this->raiz) > configuracao.ProfundidadeMaxima()) {
    throw std::invalid_argument("A raiz excede a profundidade maxima da octree");
  }
}

//----------------------------------------------------------------------------------------------

const TConfiguracaoOctree& TOctree::Configuracao() const
{
  return configuracao;
}

//----------------------------------------------------------------------------------------------

const TNoOctree& TOctree::Raiz() const
{
  return raiz;
}

//----------------------------------------------------------------------------------------------

void TOctree::Constroi(
  const TClassificadorOctree& classificador
)
{
  if (!classificador.EstaContido(configuracao.Dominio())) {
    throw std::invalid_argument("A primitiva deve estar contida no dominio da octree");
  }

  raiz.DefineEstado(EEstadoNoOctree::CHEIO);
  ConstroiNo(raiz, classificador, configuracao.ProfundidadeMaxima());
}

//----------------------------------------------------------------------------------------------

void TOctree::ConstroiNo(
  TNoOctree& no,
  const TClassificadorOctree& classificador,
  int profundidade
)
{
  EEstadoNoOctree estado = EEstadoNoOctree::CHEIO;

  if (profundidade > 1) {
    estado = classificador.Classifica(no.Regiao());
  }

  no.DefineEstado(estado);

  if (estado == EEstadoNoOctree::PARCIAL) {
    for (std::size_t indice = 0; indice < 8; ++indice) {
      ConstroiNo(no.Filho(indice), classificador, profundidade - 1);
    }
  }
}

//----------------------------------------------------------------------------------------------
