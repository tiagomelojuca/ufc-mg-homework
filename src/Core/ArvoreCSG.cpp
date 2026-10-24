#include "ArvoreCSG.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace
{
  const TMovimentoRigido MOVIMENTO_NULO(ETipoMovimentoRigido::TRANSLACAO, { 0.0, 0.0, 0.0 });
  const TEscala ESCALA_UNITARIA({ 1.0, 1.0, 1.0 });
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

TMatriz4x4 TMovimentoRigido::Matriz() const
{
  if (tipo == ETipoMovimentoRigido::TRANSLACAO) {
    return TMatriz4x4::Translacao(valores);
  }

  return
    TMatriz4x4::RotacaoZ(valores.z) *
    TMatriz4x4::RotacaoY(valores.y) *
    TMatriz4x4::RotacaoX(valores.x);
}

TEscala::TEscala(
  const TVetor3D& fatores
) :
  fatores(fatores)
{
  const bool finitos = std::isfinite(fatores.x) && std::isfinite(fatores.y) && std::isfinite(fatores.z);
  if (!finitos || fatores.x == 0.0 || fatores.y == 0.0 || fatores.z == 0.0) {
    throw std::invalid_argument("Os fatores de escala devem ser finitos e diferentes de zero");
  }
}

const TVetor3D& TEscala::Fatores() const
{
  return fatores;
}

TMatriz4x4 TEscala::Matriz() const
{
  return TMatriz4x4::Escala(fatores);
}

TMatriz4x4 TEscala::MatrizInversa() const
{
  return TMatriz4x4::Escala({ 1.0 / fatores.x, 1.0 / fatores.y, 1.0 / fatores.z });
}

TVetor3D TEscala::LevaAoSistemaDoFilho(
  const TVetor3D& ponto
) const
{
  return MatrizInversa().TransformaPonto(ponto);
}

TAresta TEscala::LevaAoSistemaDoFilho(
  const TAresta& aresta
) const
{
  const TMatriz4x4 inversa = MatrizInversa();
  return TAresta(inversa.TransformaPonto(aresta.Inicio()), inversa.TransformaPonto(aresta.Fim()));
}
//----------------------------------------------------------------------------------------------

TMatriz4x4 TMovimentoRigido::MatrizInversa() const
{
  if (tipo == ETipoMovimentoRigido::TRANSLACAO) {
    return TMatriz4x4::Translacao(-1.0 * valores);
  }

  // A inversa de Rz · Ry · Rx desfaz as rotações em ordem contrária e com ângulos opostos.
  return
    TMatriz4x4::RotacaoX(-valores.x) *
    TMatriz4x4::RotacaoY(-valores.y) *
    TMatriz4x4::RotacaoZ(-valores.z);
}

//----------------------------------------------------------------------------------------------

TVetor3D TMovimentoRigido::LevaAoSistemaDoFilho(
  const TVetor3D& ponto
) const
{
  return MatrizInversa().TransformaPonto(ponto);
}

//----------------------------------------------------------------------------------------------

// A transformação é afim, então o ponto de parâmetro t continua com o mesmo t no sistema do
// filho; a classificação obtida lá vale sem conversão para a aresta original.
TAresta TMovimentoRigido::LevaAoSistemaDoFilho(
  const TAresta& aresta
) const
{
  const TMatriz4x4 inversa = MatrizInversa();
  return TAresta(inversa.TransformaPonto(aresta.Inicio()), inversa.TransformaPonto(aresta.Fim()));
}

//----------------------------------------------------------------------------------------------

TNoCSG::TNoCSG(
  std::shared_ptr<const TPrimitiva> primitiva
) :
  tipo(ETipoNoCSG::PRIMITIVA),
  primitiva(std::move(primitiva)),
  operacao(EOperacaoBooleana::UNIAO),
  movimento(MOVIMENTO_NULO),
  escala(ESCALA_UNITARIA)
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
  escala(ESCALA_UNITARIA),
  esquerda(std::move(esquerda)),
  direita(std::move(direita))
{
  ExigeSubarvore(this->esquerda);
  ExigeSubarvore(this->direita);
}
TNoCSG::TNoCSG(
  const TEscala& escala,
  TArvoreCSG filho
) :
  tipo(ETipoNoCSG::ESCALA),
  operacao(EOperacaoBooleana::UNIAO),
  movimento(MOVIMENTO_NULO),
  escala(escala),
  esquerda(std::move(filho))
{
  ExigeSubarvore(esquerda);
}
//----------------------------------------------------------------------------------------------

TNoCSG::TNoCSG(
  const TMovimentoRigido& movimento,
  TArvoreCSG filho
) :
  tipo(ETipoNoCSG::MOVIMENTO_RIGIDO),
  operacao(EOperacaoBooleana::UNIAO),
  movimento(movimento),
  escala(ESCALA_UNITARIA),
  esquerda(std::move(filho))
{
  ExigeSubarvore(esquerda);
  
}
const TEscala& TNoCSG::Escala() const
{
  ExigeTipo(ETipoNoCSG::ESCALA);
  return escala;
}

TArvoreCSG CriaEscala(
  const TEscala& escala,
  TArvoreCSG filho
)
{
  return std::make_shared<const TNoCSG>(escala, std::move(filho));
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
   if (tipo != ETipoNoCSG::MOVIMENTO_RIGIDO && tipo != ETipoNoCSG::ESCALA) {
    throw std::logic_error("Operacao invalida para este tipo de no");
  }

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
