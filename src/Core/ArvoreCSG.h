#ifndef ARVORECSG_H_
#define ARVORECSG_H_

#include <memory>

#include "Primitiva.h"
#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

enum class ETipoNoCSG
{
  PRIMITIVA,
  OPERACAO_BOOLEANA,
  MOVIMENTO_RIGIDO
};

//----------------------------------------------------------------------------------------------

// Os valores são os códigos usados no arquivo.
enum class EOperacaoBooleana : char
{
  UNIAO = 'U',
  INTERSECAO = 'I',
  DIFERENCA = 'D'
};

//----------------------------------------------------------------------------------------------

// Os valores são os códigos usados no arquivo.
enum class ETipoMovimentoRigido : char
{
  TRANSLACAO = 'T',
  ROTACAO = 'R'
};

//----------------------------------------------------------------------------------------------

class TMovimentoRigido
{
  public:
    // Na translação, os valores são os deslocamentos; na rotação, os ângulos em graus.
    TMovimentoRigido(
      ETipoMovimentoRigido tipo,
      const TVetor3D& valores
    );

    ETipoMovimentoRigido Tipo() const;
    const TVetor3D& Valores() const;

  private:
    ETipoMovimentoRigido tipo;
    TVetor3D valores;
};

//----------------------------------------------------------------------------------------------

class TNoCSG;

// Uma árvore é representada pelo seu nó raiz. Os nós são imutáveis e podem ser
// compartilhados entre árvores, de modo que uma operação nunca altera suas entradas.
using TArvoreCSG = std::shared_ptr<const TNoCSG>;

//----------------------------------------------------------------------------------------------

class TNoCSG
{
  public:
    explicit TNoCSG(
      std::shared_ptr<const TPrimitiva> primitiva
    );

    TNoCSG(
      EOperacaoBooleana operacao,
      TArvoreCSG esquerda,
      TArvoreCSG direita
    );

    TNoCSG(
      const TMovimentoRigido& movimento,
      TArvoreCSG filho
    );

    TNoCSG(
      const TNoCSG&
    ) = delete;

    TNoCSG& operator=(
      const TNoCSG&
    ) = delete;

    ETipoNoCSG Tipo() const;

    const TPrimitiva& Primitiva() const;

    EOperacaoBooleana Operacao() const;
    const TArvoreCSG& Esquerda() const;
    const TArvoreCSG& Direita() const;

    const TMovimentoRigido& Movimento() const;
    const TArvoreCSG& Filho() const;

  private:
    void ExigeTipo(
      ETipoNoCSG tipoEsperado
    ) const;

    ETipoNoCSG tipo;
    std::shared_ptr<const TPrimitiva> primitiva;
    EOperacaoBooleana operacao;
    TMovimentoRigido movimento;
    TArvoreCSG esquerda;
    TArvoreCSG direita;
};

//----------------------------------------------------------------------------------------------

TArvoreCSG CriaPrimitiva(
  std::shared_ptr<const TPrimitiva> primitiva
);

TArvoreCSG CriaOperacaoBooleana(
  EOperacaoBooleana operacao,
  TArvoreCSG esquerda,
  TArvoreCSG direita
);

TArvoreCSG CriaMovimentoRigido(
  const TMovimentoRigido& movimento,
  TArvoreCSG filho
);

//----------------------------------------------------------------------------------------------

#endif
