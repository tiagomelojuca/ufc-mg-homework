#ifndef ARVORECSG_H_
#define ARVORECSG_H_

#include <memory>

#include "Classificacao.h"
#include "Matriz4x4.h"
#include "Primitiva.h"
#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

enum class ETipoNoCSG
{
  PRIMITIVA,
  OPERACAO_BOOLEANA,
  MOVIMENTO_RIGIDO,
  ESCALA
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

    // Leva do sistema do filho ao sistema do pai. A rotação é R = Rz · Ry · Rx.
    TMatriz4x4 Matriz() const;

    // Leva do sistema do pai ao sistema do filho, desfazendo o movimento.
    TMatriz4x4 MatrizInversa() const;

    TVetor3D LevaAoSistemaDoFilho(
      const TVetor3D& ponto
    ) const;

    TAresta LevaAoSistemaDoFilho(
      const TAresta& aresta
    ) const;

  private:
    ETipoMovimentoRigido tipo;
    TVetor3D valores;
};
class TEscala
{
  public:
    // Os fatores devem ser finitos e diferentes de zero (negativo espelha o eixo).
    explicit TEscala(
      const TVetor3D& fatores
    );

    const TVetor3D& Fatores() const;

    // Leva do sistema do filho ao sistema do pai.
    TMatriz4x4 Matriz() const;

    // Leva do sistema do pai ao sistema do filho.
    TMatriz4x4 MatrizInversa() const;

    TVetor3D LevaAoSistemaDoFilho(
      const TVetor3D& ponto
    ) const;

    TAresta LevaAoSistemaDoFilho(
      const TAresta& aresta
    ) const;

  private:
    TVetor3D fatores;
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
    
    TNoCSG(
    const TEscala& escala,
    TArvoreCSG filho
  );
    const TEscala& Escala() const;
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
    TEscala escala;   
    TArvoreCSG esquerda;
    TArvoreCSG direita;
};

//----------------------------------------------------------------------------------------------
TArvoreCSG CriaEscala(
  const TEscala& escala,
  TArvoreCSG filho
);

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
