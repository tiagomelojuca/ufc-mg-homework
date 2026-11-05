#ifndef PAINELARVORE_H_
#define PAINELARVORE_H_

#include <optional>

#include "Application/DiagramaArvore.h"
#include "Core/ArvoreCSG.h"
#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

// Painel que desenha a árvore CSG do modelo selecionado: primitivas nas folhas, operações
// booleanas e movimentos rígidos nos nós internos, cada tipo com uma cor.
class TPainelArvore
{
  public:
    TPainelArvore();

    // Uma árvore vazia deixa o painel sem diagrama.
    void DefineArvore(
      const TArvoreCSG& arvore
    );

    // Deve ser chamado entre ImGui::NewFrame e ImGui::Render.
    void Desenha();

  private:
    void DesenhaDiagrama();

    std::optional<TDiagramaArvore> diagrama;

    // Pede que a rolagem horizontal leve a raiz ao meio da área visível.
    bool centralizaRaiz;
};

//----------------------------------------------------------------------------------------------

#endif
