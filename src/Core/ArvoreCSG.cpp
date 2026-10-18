#include "ArvoreCSG.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace
{
  const TMovimentoRigido MOVIMENTO_NULO(ETipoMovimentoRigido::TRANSLACAO, { 0.0, 0.0, 0.0 });

  void ExigeSubarvore(
    const TArvoreCSG& subarvore
  )
  {
    if (subarvore == nullptr) {
      throw std::invalid_argument("A subarvore nao pode ser vazia");
    }
  }
}

//----------------------------------------------------------------------------------------------

TMovimentoRigido::TMovimentoRigido(
  ETipoMovimentoRigido tipo,
  const TVetor3D& valores
) :
  tipo(tipo),
  valores(valores)
{
  if (!std::isfinite(valores.x) || !std::isfinite(valores.y) || !std::isfinite(valores.z)) {
    throw std::invalid_argument("Os valores do movimento rigido devem ser finitos");
  }
}

//----------------------------------------------------------------------------------------------

ETipoMovimentoRigido TMovimentoRigido::Tipo() const
{
  return tipo;
}

//----------------------------------------------------------------------------------------------

const TVetor3D& TMovimentoRigido::Valores() const
{
  return valores;
}

//----------------------------------------------------------------------------------------------

TNoCSG::TNoCSG(
  std::shared_ptr<const TPrimitiva> primitiva
) :
  tipo(ETipoNoCSG::PRIMITIVA),
  primitiva(std::move(primitiva)),
  operacao(EOperacaoBooleana::UNIAO),
  movimento(MOVIMENTO_NULO)
{
  if (this->primitiva == nullptr) {
    throw std::invalid_argument("A primitiva nao pode ser vazia");
  }
}

//----------------------------------------------------------------------------------------------

TNoCSG::TNoCSG(
  EOperacaoBooleana operacao,
  TArvoreCSG esquerda,
  TArvoreCSG direita
) :
  tipo(ETipoNoCSG::OPERACAO_BOOLEANA),
  operacao(operacao),
  movimento(MOVIMENTO_NULO),
  esquerda(std::move(esquerda)),
  direita(std::move(direita))
{
  ExigeSubarvore(this->esquerda);
  ExigeSubarvore(this->direita);
}

//----------------------------------------------------------------------------------------------

TNoCSG::TNoCSG(
  const TMovimentoRigido& movimento,
  TArvoreCSG filho
) :
  tipo(ETipoNoCSG::MOVIMENTO_RIGIDO),
  operacao(EOperacaoBooleana::UNIAO),
  movimento(movimento),
  esquerda(std::move(filho))
{
  ExigeSubarvore(esquerda);
}

//----------------------------------------------------------------------------------------------

ETipoNoCSG TNoCSG::Tipo() const
{
  return tipo;
}

//----------------------------------------------------------------------------------------------

const TPrimitiva& TNoCSG::Primitiva() const
{
  ExigeTipo(ETipoNoCSG::PRIMITIVA);
  return *primitiva;
}

//----------------------------------------------------------------------------------------------

EOperacaoBooleana TNoCSG::Operacao() const
{
  ExigeTipo(ETipoNoCSG::OPERACAO_BOOLEANA);
  return operacao;
}

//----------------------------------------------------------------------------------------------

const TArvoreCSG& TNoCSG::Esquerda() const
{
  ExigeTipo(ETipoNoCSG::OPERACAO_BOOLEANA);
  return esquerda;
}

//----------------------------------------------------------------------------------------------

const TArvoreCSG& TNoCSG::Direita() const
{
  ExigeTipo(ETipoNoCSG::OPERACAO_BOOLEANA);
  return direita;
}

//----------------------------------------------------------------------------------------------

const TMovimentoRigido& TNoCSG::Movimento() const
{
  ExigeTipo(ETipoNoCSG::MOVIMENTO_RIGIDO);
  return movimento;
}

//----------------------------------------------------------------------------------------------

const TArvoreCSG& TNoCSG::Filho() const
{
  ExigeTipo(ETipoNoCSG::MOVIMENTO_RIGIDO);
  return esquerda;
}

//----------------------------------------------------------------------------------------------

void TNoCSG::ExigeTipo(
  ETipoNoCSG tipoEsperado
) const
{
  if (tipo != tipoEsperado) {
    throw std::logic_error("Operacao invalida para este tipo de no");
  }
}

//----------------------------------------------------------------------------------------------

TArvoreCSG CriaPrimitiva(
  std::shared_ptr<const TPrimitiva> primitiva
)
{
  return std::make_shared<const TNoCSG>(std::move(primitiva));
}

//----------------------------------------------------------------------------------------------

TArvoreCSG CriaOperacaoBooleana(
  EOperacaoBooleana operacao,
  TArvoreCSG esquerda,
  TArvoreCSG direita
)
{
  return std::make_shared<const TNoCSG>(operacao, std::move(esquerda), std::move(direita));
}

//----------------------------------------------------------------------------------------------

TArvoreCSG CriaMovimentoRigido(
  const TMovimentoRigido& movimento,
  TArvoreCSG filho
)
{
  return std::make_shared<const TNoCSG>(movimento, std::move(filho));
}

//----------------------------------------------------------------------------------------------
