#include "CaixaEnvolvente.h"

#include <stdexcept>

namespace
{
  TCaixa TransformaCaixa(
    const TCaixa& caixa,
    const TMatriz4x4& matriz
  )
  {
    TCaixa resultado = { matriz.TransformaPonto(caixa.minimo), matriz.TransformaPonto(caixa.minimo) };

    for (int canto = 0; canto < 8; ++canto) {
      const TVetor3D ponto = {
        (canto & 1) ? caixa.maximo.x : caixa.minimo.x,
        (canto & 2) ? caixa.maximo.y : caixa.minimo.y,
        (canto & 4) ? caixa.maximo.z : caixa.minimo.z
      };
      const TVetor3D transformado = matriz.TransformaPonto(ponto);
      resultado = UneCaixas(resultado, { transformado, transformado });
    }

    return resultado;
  }
}

//----------------------------------------------------------------------------------------------

TCaixa CaixaEnvolvente(
  const TArvoreCSG& arvore
)
{
  switch (arvore->Tipo()) {
    case ETipoNoCSG::PRIMITIVA:
      return arvore->Primitiva().CaixaCanonica();

    case ETipoNoCSG::MOVIMENTO_RIGIDO:
      return TransformaCaixa(CaixaEnvolvente(arvore->Filho()), arvore->Movimento().Matriz());

    case ETipoNoCSG::OPERACAO_BOOLEANA: {
      const TCaixa esquerda = CaixaEnvolvente(arvore->Esquerda());
      const TCaixa direita = CaixaEnvolvente(arvore->Direita());

      switch (arvore->Operacao()) {
        case EOperacaoBooleana::UNIAO:
          return UneCaixas(esquerda, direita);

        // O resultado está contido no operando da esquerda.
        case EOperacaoBooleana::INTERSECAO:
        case EOperacaoBooleana::DIFERENCA:
          return esquerda;
      }

      break;
    }
  }

  throw std::logic_error("Tipo de no desconhecido");
}

//----------------------------------------------------------------------------------------------
