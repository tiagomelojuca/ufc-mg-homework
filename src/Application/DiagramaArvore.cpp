#include "DiagramaArvore.h"

#include <algorithm>
#include <stdexcept>

#include "Core/Bloco.h"
#include "Core/Cilindro.h"
#include "Core/Esfera.h"
#include "Formatacao.h"

namespace
{
  TRotuloNo RotuloDaPrimitiva(
    const TPrimitiva& primitiva
  )
  {
    if (const auto* bloco = dynamic_cast<const TBloco*>(&primitiva)) {
      return {
        "Bloco",
        FormataNumero(bloco->LadoX()) + " x " + FormataNumero(bloco->LadoY()) + " x " +
          FormataNumero(bloco->LadoZ())
      };
    }

    if (const auto* esfera = dynamic_cast<const TEsfera*>(&primitiva)) {
      return { "Esfera", "r = " + FormataNumero(esfera->Raio()) };
    }

    if (const auto* cilindro = dynamic_cast<const TCilindro*>(&primitiva)) {
      return {
        "Cilindro",
        "r = " + FormataNumero(cilindro->Raio()) + ", h = " + FormataNumero(cilindro->Altura())
      };
    }

    return { "Primitiva " + std::string(1, static_cast<char>(primitiva.Tipo())), "" };
  }

  //--------------------------------------------------------------------------------------------

  // Acrescenta o nó e os seus descendentes em pré-ordem e devolve a coluna do nó. As folhas
  // recebem colunas consecutivas na ordem em que são visitadas.
  double AcrescentaNo(
    const TArvoreCSG& arvore,
    int nivel,
    std::optional<std::size_t> pai,
    int& proximaFolha,
    TDiagramaArvore& diagrama
  )
  {
    const std::size_t indice = diagrama.nos.size();
    diagrama.nos.push_back({ arvore->Tipo(), RotuloDoNo(*arvore), nivel, 0.0, pai });
    diagrama.niveis = std::max(diagrama.niveis, nivel + 1);

    double coluna = 0.0;
    switch (arvore->Tipo()) {
      case ETipoNoCSG::PRIMITIVA:
        coluna = proximaFolha++;
        break;

      case ETipoNoCSG::MOVIMENTO_RIGIDO:
        coluna = AcrescentaNo(arvore->Filho(), nivel + 1, indice, proximaFolha, diagrama);
        break;

      case ETipoNoCSG::OPERACAO_BOOLEANA: {
        const double esquerda =
          AcrescentaNo(arvore->Esquerda(), nivel + 1, indice, proximaFolha, diagrama);
        const double direita =
          AcrescentaNo(arvore->Direita(), nivel + 1, indice, proximaFolha, diagrama);
        coluna = (esquerda + direita) / 2.0;
        break;
      }
    }

    diagrama.nos[indice].coluna = coluna;
    return coluna;
  }
}

//----------------------------------------------------------------------------------------------

TDiagramaArvore MontaDiagrama(
  const TArvoreCSG& arvore
)
{
  if (arvore == nullptr) {
    throw std::invalid_argument("A arvore nao pode ser vazia");
  }

  TDiagramaArvore diagrama = { {}, 0, 0 };
  int proximaFolha = 0;
  AcrescentaNo(arvore, 0, std::nullopt, proximaFolha, diagrama);
  diagrama.colunas = proximaFolha;

  return diagrama;
}

//----------------------------------------------------------------------------------------------

TRotuloNo RotuloDoNo(
  const TNoCSG& no
)
{
  switch (no.Tipo()) {
    case ETipoNoCSG::PRIMITIVA:
      return RotuloDaPrimitiva(no.Primitiva());

    case ETipoNoCSG::OPERACAO_BOOLEANA:
      switch (no.Operacao()) {
        case EOperacaoBooleana::UNIAO:
          return { "União", "U" };

        case EOperacaoBooleana::INTERSECAO:
          return { "Interseção", "I" };

        case EOperacaoBooleana::DIFERENCA:
          return { "Diferença", "D" };
      }

      break;

    case ETipoNoCSG::MOVIMENTO_RIGIDO:
      if (no.Movimento().Tipo() == ETipoMovimentoRigido::TRANSLACAO) {
        return { "Translação", FormataVetor(no.Movimento().Valores()) };
      }

      return { "Rotação", FormataVetor(no.Movimento().Valores()) + " graus" };
  }

  throw std::logic_error("Tipo de no desconhecido");
}

//----------------------------------------------------------------------------------------------
