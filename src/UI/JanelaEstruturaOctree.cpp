#include "JanelaEstruturaOctree.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <string>

#include "RenderizadorAramado.h"

namespace
{
  // Acima desse total o desenho imediato fica lento demais para uso interativo.
  constexpr std::size_t LIMITE_ARESTAS = 600000;
  constexpr float LARGURA_ARVORE = 330.0f;
  const ImVec4 COR_CHEIO = ImVec4(0.35f, 0.8f, 1.0f, 1.0f);
  const ImVec4 COR_PARCIAL = ImVec4(0.96f, 0.69f, 0.29f, 1.0f);
  const ImVec4 COR_VAZIO = ImVec4(0.36f, 0.44f, 0.54f, 1.0f);

  using TUseProgram = void (APIENTRY*)(GLuint);

  const TNoOctree& NoDoCaminho(
    const TOctree& octree,
    const std::vector<std::size_t>& caminho
  )
  {
    const TNoOctree* no = &octree.Raiz();
    for (std::size_t octante : caminho) {
      no = &no->Filho(octante);
    }
    return *no;
  }

  ImVec4 CorEstado(
    EEstadoNoOctree estado
  )
  {
    switch (estado) {
      case EEstadoNoOctree::CHEIO: return COR_CHEIO;
      case EEstadoNoOctree::VAZIO: return COR_VAZIO;
      default: return COR_PARCIAL;
    }
  }

  const char* DescricaoEstado(
    EEstadoNoOctree estado
  )
  {
    switch (estado) {
      case EEstadoNoOctree::CHEIO: return "B · cheio";
      case EEstadoNoOctree::VAZIO: return "W · vazio";
      default: return "( · parcial";
    }
  }

  TLoteAramado Lote(
    const std::vector<TAresta3D>& arestas,
    const ImVec4& cor
  )
  {
    return { &arestas, { cor.x, cor.y, cor.z } };
  }

  void Legenda(
    const ImVec4& cor,
    const char* texto
  )
  {
    const float lado = ImGui::GetTextLineHeight() * 0.6f;
    const ImVec2 posicao = ImGui::GetCursorScreenPos();
    const float deslocamento = (ImGui::GetTextLineHeight() - lado) / 2.0f;
    ImGui::GetWindowDrawList()->AddRectFilled(
      ImVec2(posicao.x, posicao.y + deslocamento),
      ImVec2(posicao.x + lado, posicao.y + deslocamento + lado),
      ImGui::ColorConvertFloat4ToU32(cor), 2.0f
    );
    ImGui::Dummy(ImVec2(lado, ImGui::GetTextLineHeight()));
    ImGui::SameLine(0.0f, 6.0f);
    ImGui::TextUnformatted(texto);
  }
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::Abre()
{
  aberta = true;
  focar = true;
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::Desenha(
  const TModeloOctree* modelo
)
{
  if (!aberta) {
    return;
  }

  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(940.0f, 620.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSizeConstraints(ImVec2(700.0f, 440.0f), ImVec2(FLT_MAX, FLT_MAX));
  if (focar) {
    ImGui::SetNextWindowFocus();
    focar = false;
  }

  char titulo[256];
  std::snprintf(titulo, sizeof(titulo), "Estrutura da octree · %s###EstruturaOctree", modelo == nullptr ? "sem modelo" : modelo->Nome().c_str());
  if (!ImGui::Begin(titulo, &aberta, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
    ImGui::End();
    return;
  }

  if (modelo == nullptr) {
    ImGui::TextDisabled("Selecione um modelo para inspecionar sua octree.");
    ImGui::End();
    return;
  }

  Atualiza(*modelo);
  DesenhaCaminho();

  const float alturaUtil = ImGui::GetContentRegionAvail().y;
  ImGui::BeginGroup();
  ImGui::BeginChild("Arvore", ImVec2(LARGURA_ARVORE, alturaUtil * 0.58f), ImGuiChildFlags_Borders);
  std::vector<std::size_t> caminhoAtual;
  DesenhaArvore(modelo->Octree().Raiz(), "Raiz", caminhoAtual);
  ImGui::EndChild();
  DesenhaContagens();
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::BeginGroup();
  DesenhaCena();
  ImGui::EndGroup();

  ImGui::End();
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::Atualiza(
  const TModeloOctree& modelo
)
{
  if (modelo.Id() != idModelo) {
    idModelo = modelo.Id();
    caminho.clear();
    niveis = 3;
    niveisCalculados = -1;
  }

  const TNoOctree& no = NoDoCaminho(modelo.Octree(), caminho);
  const bool mudouNo = niveisCalculados < 0 || caminho != caminhoCalculado;
  if (mudouNo) {
    contagens = TAnaliseOctree().ContaNosPorNivel(no);
    enquadramento = no.Regiao();
  }

  niveis = std::clamp(niveis, 0, static_cast<int>(contagens.size()) - 1);
  if (mudouNo || niveis != niveisCalculados) {
    estrutura = TGeradorAramadoOctree().GeraEstrutura(no, niveis);
  }
  caminhoCalculado = caminho;
  niveisCalculados = niveis;
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::DesenhaCaminho()
{
  ImGui::TextDisabled("Nó em foco");
  ImGui::SameLine();
  std::string texto = "Raiz";
  for (std::size_t octante : caminho) {
    texto += " / " + std::to_string(octante);
  }
  ImGui::TextUnformatted(texto.c_str());
  ImGui::SameLine();
  ImGui::TextDisabled("(nível %zu)", caminho.size());

  const float larguraBotoes = ImGui::CalcTextSize("Subir um nível").x + ImGui::CalcTextSize("Raiz").x +
    4.0f * ImGui::GetStyle().FramePadding.x + ImGui::GetStyle().ItemSpacing.x;
  ImGui::SameLine();
  ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - larguraBotoes));
  ImGui::BeginDisabled(caminho.empty());
  if (ImGui::SmallButton("Subir um nível")) {
    caminho.pop_back();
  }
  ImGui::SameLine();
  if (ImGui::SmallButton("Raiz")) {
    caminho.clear();
  }
  ImGui::EndDisabled();
  ImGui::Separator();
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::DesenhaArvore(
  const TNoOctree& no,
  const char* rotulo,
  std::vector<std::size_t>& caminhoAtual
)
{
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
  if (no.EhFolha()) {
    flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }
  if (caminhoAtual == caminho) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  const bool aberto = ImGui::TreeNodeEx(rotulo, flags);
  if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    caminho = caminhoAtual;
  }
  if (ImGui::IsItemHovered()) {
    const TCubo& regiao = no.Regiao();
    ImGui::SetTooltip(
      "Nível %zu\nCentro (%.4f, %.4f, %.4f)\nLado %.4f",
      caminhoAtual.size(), regiao.Centro().x, regiao.Centro().y, regiao.Centro().z, regiao.Lado()
    );
  }
  ImGui::SameLine();
  ImGui::TextColored(CorEstado(no.Estado()), "%s", DescricaoEstado(no.Estado()));

  if (!aberto || no.EhFolha()) {
    return;
  }

  for (std::size_t indice = 0; indice < 8; ++indice) {
    char rotuloFilho[16];
    std::snprintf(rotuloFilho, sizeof(rotuloFilho), "Octante %zu", indice);
    caminhoAtual.push_back(indice);
    DesenhaArvore(no.Filho(indice), rotuloFilho, caminhoAtual);
    caminhoAtual.pop_back();
  }
  ImGui::TreePop();
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::DesenhaContagens() const
{
  ImGui::TextDisabled("Nós por nível da subárvore em foco");
  constexpr ImGuiTableFlags flags =
    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame;
  if (!ImGui::BeginTable("Contagens", 5, flags, ImVec2(LARGURA_ARVORE, ImGui::GetContentRegionAvail().y))) {
    return;
  }
  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableSetupColumn("Nível");
  ImGui::TableSetupColumn("B");
  ImGui::TableSetupColumn("W");
  ImGui::TableSetupColumn("(");
  ImGui::TableSetupColumn("Total");
  ImGui::TableHeadersRow();

  TContagemNivelOctree soma;
  for (std::size_t nivel = 0; nivel < contagens.size(); ++nivel) {
    const TContagemNivelOctree& contagem = contagens[nivel];
    soma.cheios += contagem.cheios;
    soma.vazios += contagem.vazios;
    soma.parciais += contagem.parciais;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%zu", caminho.size() + nivel);
    ImGui::TableNextColumn();
    ImGui::Text("%zu", contagem.cheios);
    ImGui::TableNextColumn();
    ImGui::Text("%zu", contagem.vazios);
    ImGui::TableNextColumn();
    ImGui::Text("%zu", contagem.parciais);
    ImGui::TableNextColumn();
    ImGui::Text("%zu", contagem.cheios + contagem.vazios + contagem.parciais);
  }

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::TextDisabled("Soma");
  ImGui::TableNextColumn();
  ImGui::Text("%zu", soma.cheios);
  ImGui::TableNextColumn();
  ImGui::Text("%zu", soma.vazios);
  ImGui::TableNextColumn();
  ImGui::Text("%zu", soma.parciais);
  ImGui::TableNextColumn();
  ImGui::Text("%zu", soma.cheios + soma.vazios + soma.parciais);
  ImGui::EndTable();
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::DesenhaCena()
{
  const int altura = static_cast<int>(contagens.size()) - 1;
  ImGui::TextDisabled("Níveis abaixo do nó");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(180.0f);
  ImGui::BeginDisabled(altura == 0);
  ImGui::SliderInt("##niveis", &niveis, 0, std::max(altura, 1), "%d", ImGuiSliderFlags_AlwaysClamp);
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::Checkbox("Mostrar vazios", &mostrarVazias);

  const std::size_t totalArestas =
    estrutura.parciais.size() + estrutura.cheias.size() + (mostrarVazias ? estrutura.vazias.size() : 0);
  const ImVec2 disponivel = ImGui::GetContentRegionAvail();
  posicaoCena = ImGui::GetCursorScreenPos();
  tamanhoCena = ImVec2(disponivel.x, std::max(1.0f, disponivel.y - ImGui::GetFrameHeightWithSpacing()));
  ImGui::InvisibleButton("CenaEstrutura", tamanhoCena);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Vista ortográfica fixa enquadrada no nó em foco.\nEscolha outro nó na árvore para aproximar.");
  }

  ImDrawList* desenho = ImGui::GetWindowDrawList();
  const ImVec2 fimCena(posicaoCena.x + tamanhoCena.x, posicaoCena.y + tamanhoCena.y);
  desenho->AddRectFilled(posicaoCena, fimCena, ImGui::GetColorU32(ImGuiCol_ChildBg), 8.0f);
  if (totalArestas > LIMITE_ARESTAS) {
    const char* aviso = "Células demais para desenhar. Reduza os níveis ou escolha um nó mais profundo.";
    const ImVec2 texto = ImGui::CalcTextSize(aviso);
    desenho->AddText(
      ImVec2(posicaoCena.x + (tamanhoCena.x - texto.x) / 2.0f, posicaoCena.y + tamanhoCena.y / 2.0f),
      ImGui::GetColorU32(ImGuiCol_TextDisabled), aviso
    );
  } else {
    // Os segmentos são desenhados pelo OpenGL na ordem de composição das janelas da ImGui.
    desenho->AddCallback(DesenhaCenaOpenGL, this);
    desenho->AddCallback(ImGui::GetPlatformIO().DrawCallback_ResetRenderState, nullptr);
  }
  desenho->AddRect(posicaoCena, fimCena, IM_COL32(40, 55, 73, 255), 8.0f);

  char texto[64];
  std::snprintf(texto, sizeof(texto), "Parcial (%zu)", estrutura.parciais.size() / 12);
  Legenda(COR_PARCIAL, texto);
  ImGui::SameLine(0.0f, 18.0f);
  std::snprintf(texto, sizeof(texto), "Cheio (%zu)", estrutura.cheias.size() / 12);
  Legenda(COR_CHEIO, texto);
  ImGui::SameLine(0.0f, 18.0f);
  std::snprintf(texto, sizeof(texto), "Vazio (%zu)", estrutura.vazias.size() / 12);
  Legenda(COR_VAZIO, texto);
}

//----------------------------------------------------------------------------------------------

void TJanelaEstruturaOctree::DesenhaCenaOpenGL(
  const ImDrawList*,
  const ImDrawCmd* comando
)
{
  const auto* janela = static_cast<const TJanelaEstruturaOctree*>(comando->UserCallbackData);
  const ImGuiIO& io = ImGui::GetIO();
  const float escalaX = io.DisplayFramebufferScale.x;
  const float escalaY = io.DisplayFramebufferScale.y;
  // ImGui mede a partir do topo; OpenGL mede viewport e recorte a partir da base.
  const auto converte = [&](const ImVec2& minimo, const ImVec2& maximo) {
    return TRetanguloTela {
      static_cast<int>(minimo.x * escalaX),
      static_cast<int>((io.DisplaySize.y - maximo.y) * escalaY),
      static_cast<int>((maximo.x - minimo.x) * escalaX),
      static_cast<int>((maximo.y - minimo.y) * escalaY)
    };
  };

  const ImVec2 minimo = janela->posicaoCena;
  const ImVec2 maximo(minimo.x + janela->tamanhoCena.x, minimo.y + janela->tamanhoCena.y);
  const ImVec2 minimoRecorte(std::max(minimo.x, comando->ClipRect.x), std::max(minimo.y, comando->ClipRect.y));
  const ImVec2 maximoRecorte(std::min(maximo.x, comando->ClipRect.z), std::min(maximo.y, comando->ClipRect.w));
  if (maximoRecorte.x <= minimoRecorte.x || maximoRecorte.y <= minimoRecorte.y) {
    return;
  }

  // O backend deixa seu shader ativo; o renderizador usa o pipeline fixo do OpenGL.
  static const auto useProgram = reinterpret_cast<TUseProgram>(glfwGetProcAddress("glUseProgram"));
  if (useProgram != nullptr) {
    useProgram(0);
  }

  std::vector<TLoteAramado> lotes;
  if (janela->mostrarVazias) {
    lotes.push_back(Lote(janela->estrutura.vazias, COR_VAZIO));
  }
  lotes.push_back(Lote(janela->estrutura.parciais, COR_PARCIAL));
  lotes.push_back(Lote(janela->estrutura.cheias, COR_CHEIO));
  TRenderizadorAramado().Desenha(lotes, janela->enquadramento, converte(minimo, maximo), converte(minimoRecorte, maximoRecorte));
}

//----------------------------------------------------------------------------------------------
