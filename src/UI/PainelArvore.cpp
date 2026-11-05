#include "PainelArvore.h"

#include <algorithm>

namespace
{
  constexpr float MARGEM = 12.0f;
  constexpr float FOLGA_TEXTO = 8.0f;
  constexpr float ESPACO_ENTRE_COLUNAS = 16.0f;
  constexpr float ESPACO_ENTRE_NIVEIS = 36.0f;

  //--------------------------------------------------------------------------------------------

  ImU32 CorDoTipo(
    ETipoNoCSG tipo
  )
  {
    switch (tipo) {
      case ETipoNoCSG::PRIMITIVA:
        return IM_COL32(52, 101, 164, 255);

      case ETipoNoCSG::OPERACAO_BOOLEANA:
        return IM_COL32(196, 110, 40, 255);

      case ETipoNoCSG::MOVIMENTO_RIGIDO:
        return IM_COL32(60, 140, 80, 255);
    }

    return IM_COL32(128, 128, 128, 255);
  }

  //--------------------------------------------------------------------------------------------

  void DesenhaItemDaLegenda(
    ETipoNoCSG tipo,
    const char* nome
  )
  {
    const ImVec2 posicao = ImGui::GetCursorScreenPos();
    const float lado = ImGui::GetTextLineHeight();

    ImGui::GetWindowDrawList()->AddRectFilled(
      posicao,
      ImVec2(posicao.x + lado, posicao.y + lado),
      CorDoTipo(tipo),
      3.0f
    );

    ImGui::Dummy(ImVec2(lado, lado));
    ImGui::SameLine();
    ImGui::TextUnformatted(nome);
  }
}

//----------------------------------------------------------------------------------------------

TPainelArvore::TPainelArvore() :
  centralizaRaiz(false)
{
}

//----------------------------------------------------------------------------------------------

void TPainelArvore::DefineArvore(
  const TArvoreCSG& arvore
)
{
  centralizaRaiz = true;
  diagrama.reset();
  if (arvore) {
    diagrama = MontaDiagrama(arvore);
  }
}

//----------------------------------------------------------------------------------------------

void TPainelArvore::Desenha()
{
  ImGui::SetNextWindowPos(ImVec2(980.0f, 10.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(290.0f, 700.0f), ImGuiCond_FirstUseEver);
  ImGui::Begin("Árvore CSG");

  if (!diagrama) {
    ImGui::TextDisabled("Selecione um modelo na lista para ver a sua árvore.");
    ImGui::End();
    return;
  }

  DesenhaItemDaLegenda(ETipoNoCSG::PRIMITIVA, "Primitiva");
  DesenhaItemDaLegenda(ETipoNoCSG::OPERACAO_BOOLEANA, "Operação booleana");
  DesenhaItemDaLegenda(ETipoNoCSG::MOVIMENTO_RIGIDO, "Movimento rígido");
  ImGui::Separator();

  ImGui::BeginChild(
    "##diagrama",
    ImVec2(0.0f, 0.0f),
    ImGuiChildFlags_None,
    ImGuiWindowFlags_HorizontalScrollbar
  );
  DesenhaDiagrama();
  ImGui::EndChild();

  ImGui::End();
}

//----------------------------------------------------------------------------------------------

void TPainelArvore::DesenhaDiagrama()
{
  // Todas as caixas têm a largura do maior rótulo, para que as colunas fiquem alinhadas; cada uma
  // tem duas linhas, o tipo do nó e os seus parâmetros.
  float larguraTexto = 0.0f;
  for (const TNoDiagrama& no : diagrama->nos) {
    larguraTexto = std::max({
      larguraTexto,
      ImGui::CalcTextSize(no.rotulo.titulo.c_str()).x,
      ImGui::CalcTextSize(no.rotulo.detalhe.c_str()).x
    });
  }

  const float alturaLinha = ImGui::GetTextLineHeight();
  const ImVec2 caixa(larguraTexto + 2.0f * FOLGA_TEXTO, 2.0f * alturaLinha + 2.0f * FOLGA_TEXTO);
  const float larguraColuna = caixa.x + ESPACO_ENTRE_COLUNAS;
  const float alturaNivel = caixa.y + ESPACO_ENTRE_NIVEIS;

  const ImVec2 origem = ImGui::GetCursorScreenPos();
  ImGui::Dummy(ImVec2(
    2.0f * MARGEM + diagrama->colunas * larguraColuna,
    2.0f * MARGEM + diagrama->niveis * alturaNivel
  ));

  if (centralizaRaiz) {
    const float colunaDaRaiz = static_cast<float>(diagrama->nos.front().coluna);
    ImGui::SetScrollX(MARGEM + (colunaDaRaiz + 0.5f) * larguraColuna - ImGui::GetWindowWidth() / 2.0f);
    centralizaRaiz = false;
  }

  // Centro da caixa de cada nó na tela.
  const auto centro = [&](const TNoDiagrama& no) {
    return ImVec2(
      origem.x + MARGEM + (static_cast<float>(no.coluna) + 0.5f) * larguraColuna,
      origem.y + MARGEM + no.nivel * alturaNivel + caixa.y / 2.0f
    );
  };

  ImDrawList* desenho = ImGui::GetWindowDrawList();

  // Primeiro as ligações, para que as caixas fiquem por cima delas.
  for (const TNoDiagrama& no : diagrama->nos) {
    if (no.pai) {
      const ImVec2 pai = centro(diagrama->nos[*no.pai]);
      const ImVec2 filho = centro(no);
      desenho->AddLine(
        ImVec2(pai.x, pai.y + caixa.y / 2.0f),
        ImVec2(filho.x, filho.y - caixa.y / 2.0f),
        IM_COL32(170, 170, 180, 255),
        1.5f
      );
    }
  }

  for (const TNoDiagrama& no : diagrama->nos) {
    const ImVec2 meio = centro(no);
    const ImVec2 inicio(meio.x - caixa.x / 2.0f, meio.y - caixa.y / 2.0f);
    const ImVec2 fim(meio.x + caixa.x / 2.0f, meio.y + caixa.y / 2.0f);

    desenho->AddRectFilled(inicio, fim, CorDoTipo(no.tipo), 4.0f);
    desenho->AddRect(inicio, fim, IM_COL32(230, 230, 235, 120), 4.0f);

    // Título em branco na linha de cima; parâmetros, mais apagados, na de baixo.
    const float larguraTitulo = ImGui::CalcTextSize(no.rotulo.titulo.c_str()).x;
    const float larguraDetalhe = ImGui::CalcTextSize(no.rotulo.detalhe.c_str()).x;
    desenho->AddText(
      ImVec2(meio.x - larguraTitulo / 2.0f, meio.y - alturaLinha),
      IM_COL32(255, 255, 255, 255),
      no.rotulo.titulo.c_str()
    );
    desenho->AddText(
      ImVec2(meio.x - larguraDetalhe / 2.0f, meio.y),
      IM_COL32(225, 225, 230, 210),
      no.rotulo.detalhe.c_str()
    );
  }
}

//----------------------------------------------------------------------------------------------
