#include "PainelModelador.h"

#include <algorithm>
#include <exception>

#include "misc/cpp/imgui_stdlib.h"
#include "RenderizadorAramado.h"

namespace
{
  constexpr float MARGEM = 16.0f;
  constexpr float LARGURA_CONTROLES = 344.0f;
  constexpr ImGuiWindowFlags FLAGS_PAINEL =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

  void Titulo(
    const char* texto
  )
  {
    ImGui::PushFont(nullptr, 21.0f);
    ImGui::TextUnformatted(texto);
    ImGui::PopFont();
  }

  void Rotulo(
    const char* texto
  )
  {
    ImGui::TextDisabled("%s", texto);
  }

  bool BotaoPrincipal(
    const char* texto
  )
  {
    return ImGui::Button(texto, ImVec2(-1.0f, 36.0f));
  }

  void Campo(
    const char* rotulo
  )
  {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(rotulo);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1.0f);
  }

  void Cartao(
    const char* rotulo,
    const char* valor,
    float largura
  )
  {
    ImGui::PushID(rotulo);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 10.0f));
    ImGui::BeginChild("cartao", ImVec2(largura, 66.0f), ImGuiChildFlags_Borders);
    Rotulo(rotulo);
    ImGui::TextUnformatted(valor);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopID();
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelador::ConfiguraEstilo()
{
  ImGui::StyleColorsDark();
  ImGuiStyle& estilo = ImGui::GetStyle();
  estilo.FontSizeBase = 16.0f;
  estilo.WindowPadding = ImVec2(18.0f, 16.0f);
  estilo.FramePadding = ImVec2(10.0f, 7.0f);
  estilo.ItemSpacing = ImVec2(10.0f, 8.0f);
  estilo.WindowRounding = 12.0f;
  estilo.ChildRounding = 8.0f;
  estilo.FrameRounding = 6.0f;
  estilo.PopupRounding = 10.0f;
  estilo.TabRounding = 6.0f;
  estilo.WindowBorderSize = 1.0f;
  estilo.Colors[ImGuiCol_Text] = ImVec4(0.91f, 0.94f, 0.97f, 1.0f);
  estilo.Colors[ImGuiCol_TextDisabled] = ImVec4(0.53f, 0.61f, 0.70f, 1.0f);
  estilo.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.11f, 0.16f, 1.0f);
  estilo.Colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.09f, 0.13f, 1.0f);
  estilo.Colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.13f, 0.19f, 1.0f);
  estilo.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.13f, 0.18f, 1.0f);
  estilo.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.22f, 0.28f, 1.0f);
  estilo.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.01f, 0.02f, 0.03f, 0.70f);
  estilo.Colors[ImGuiCol_Border] = ImVec4(0.16f, 0.22f, 0.29f, 1.0f);
  estilo.Colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.17f, 0.23f, 1.0f);
  estilo.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.24f, 0.32f, 1.0f);
  estilo.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.29f, 0.36f, 1.0f);
  estilo.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.39f, 0.44f, 1.0f);
  estilo.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.49f, 0.54f, 1.0f);
  estilo.Colors[ImGuiCol_ButtonActive] = ImVec4(0.09f, 0.33f, 0.38f, 1.0f);
  estilo.Colors[ImGuiCol_Header] = ImVec4(0.11f, 0.27f, 0.33f, 1.0f);
  estilo.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.14f, 0.35f, 0.42f, 1.0f);
  estilo.Colors[ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.40f, 0.46f, 1.0f);
  estilo.Colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.15f, 0.21f, 1.0f);
  estilo.Colors[ImGuiCol_TabSelected] = ImVec4(0.13f, 0.29f, 0.35f, 1.0f);
  estilo.Colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.35f, 0.42f, 1.0f);
  estilo.Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.72f, 0.77f, 1.0f);
  estilo.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.86f, 0.88f, 1.0f);
  estilo.Colors[ImGuiCol_CheckMark] = ImVec4(0.37f, 0.86f, 0.88f, 1.0f);
  estilo.Colors[ImGuiCol_Separator] = estilo.Colors[ImGuiCol_Border];
}

//----------------------------------------------------------------------------------------------

void TPainelModelador::Desenha()
{
  DesenhaCabecalho();
  DesenhaControles();
  DesenhaVisualizacao();
  DesenhaMensagem();
  janelaEstrutura.Desenha(modelador.Selecionado());
  DesenhaConfirmacao();
}

void TPainelModelador::DesenhaCabecalho()
{
  const ImVec2 tela = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(MARGEM, MARGEM));
  ImGui::SetNextWindowSize(ImVec2(tela.x - 2.0f * MARGEM, 64.0f));
  ImGui::Begin("Cabecalho", nullptr, FLAGS_PAINEL);
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.37f, 0.86f, 0.88f, 1.0f));
  Titulo("OCTREE");
  ImGui::PopStyleColor();
  ImGui::SameLine(150.0f);
  ImGui::TextUnformatted("Modelador geométrico");
  ImGui::SameLine();
  Rotulo(" /  Modelagem em Computação Gráfica · UFC");
  ImGui::End();
}

void TPainelModelador::DesenhaControles()
{
  const ImVec2 tela = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(MARGEM, 92.0f));
  ImGui::SetNextWindowSize(ImVec2(LARGURA_CONTROLES, tela.y - 154.0f));
  ImGui::Begin("Controles", nullptr, FLAGS_PAINEL);
  Titulo("Modelos");
  DesenhaLista();
  ImGui::Separator();

  Rotulo("PROFUNDIDADE · NOVOS MODELOS");
  ImGui::SetNextItemWidth(-1.0f);
  if (ImGui::SliderInt("##profundidade", &profundidade, 1, TModelador::PROFUNDIDADE_MAXIMA_INTERATIVA, "%d", ImGuiSliderFlags_AlwaysClamp)) {
    ExecutaAcao([this] { modelador.DefineProfundidade(profundidade); }, "Profundidade atualizada para os próximos modelos.");
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Não altera as árvores existentes. Valores maiores geram mais detalhes.");
  }

  if (ImGui::BeginTabBar("Operacoes")) {
    if (ImGui::BeginTabItem("Criar")) {
      DesenhaCriacao();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Operar")) {
      DesenhaOperacoes();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Arquivo")) {
      DesenhaArquivos();
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::End();
}

void TPainelModelador::DesenhaLista()
{
  int remover = 0;
  const int selecaoInicial = modelador.Selecionado() == nullptr ? 0 : modelador.Selecionado()->Id();
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 8.0f));
  ImGui::BeginChild("Lista", ImVec2(0.0f, 112.0f), ImGuiChildFlags_Borders);
  if (modelador.Modelos().empty()) {
    Rotulo("Nenhum modelo ainda.");
    ImGui::TextWrapped("Use Criar ou Arquivo para começar.");
  }
  for (const TModeloOctree& modelo : modelador.Modelos()) {
    ImGui::PushID(modelo.Id());
    const bool selecionado = modelador.Selecionado() != nullptr && modelador.Selecionado()->Id() == modelo.Id();
    if (ImGui::Selectable(modelo.Nome().c_str(), selecionado)) {
      modelador.Seleciona(modelo.Id());
    }
    if (selecaoInicial != ultimaSelecao && modelo.Id() == selecaoInicial) {
      ImGui::SetScrollHereY(0.5f);
    }
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::MenuItem("Remover")) { remover = modelo.Id(); }
      ImGui::EndPopup();
    }
    ImGui::PopID();
  }
  ImGui::EndChild();
  ImGui::PopStyleVar();
  ultimaSelecao = selecaoInicial;
  ImGui::BeginDisabled(modelador.Selecionado() == nullptr);
  if (ImGui::SmallButton("Remover selecionado")) {
    remover = modelador.Selecionado()->Id();
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::TextDisabled("%zu modelo(s)", modelador.Modelos().size());
  if (remover != 0) {
    ExecutaAcao([this, remover] { modelador.Remove(remover); }, "Modelo removido.");
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelador::DesenhaCriacao()
{
  if (ImGui::BeginTable("Parametros", 2)) {
    ImGui::TableSetupColumn("Rotulo", ImGuiTableColumnFlags_WidthFixed, 66.0f);
    ImGui::TableSetupColumn("Valor", ImGuiTableColumnFlags_WidthStretch);
    Campo("Tipo");
    ImGui::Combo("##primitiva", &primitiva, "Bloco\0Esfera\0");
    Campo("Nome");
    ImGui::InputTextWithHint("##nome", "Opcional", &nome);
    Campo("Centro");
    ImGui::InputFloat3("##centro", centro, "%.3f");
    if (primitiva == 0) {
      Campo("Lados");
      ImGui::InputFloat3("##lados", lados, "%.3f");
    } else {
      Campo("Raio");
      ImGui::InputFloat("##raio", &raio, 0.0f, 0.0f, "%.3f");
    }
    ImGui::EndTable();
  }
  if (BotaoPrincipal(primitiva == 0 ? "Criar bloco" : "Criar esfera")) {
    ExecutaAcao([this] {
      const TCoordenada3D ponto = { centro[0], centro[1], centro[2] };
      if (primitiva == 0) {
        modelador.CriaBloco(TBloco(ponto, lados[0], lados[1], lados[2]), nome);
      } else {
        modelador.CriaEsfera(TEsfera(ponto, raio), nome);
      }
    }, "Modelo criado e selecionado.");
  }
  ImGui::TextDisabled("Domínio: [-1, 1] em X, Y e Z.");
  ImGui::TextDisabled("Centro e lados na ordem X / Y / Z.");
}

bool TPainelModelador::EscolheModelo(
  const char* rotulo,
  int& id
)
{
  const auto& modelos = modelador.Modelos();
  auto escolhido = std::find_if(modelos.begin(), modelos.end(), [id](const TModeloOctree& modelo) {
    return modelo.Id() == id;
  });
  if (escolhido == modelos.end()) {
    id = modelos.empty() ? 0 : modelos.front().Id();
  }
  const char* nomeModelo = id == 0 ? "Nenhum modelo" : modelador.Modelo(id).Nome().c_str();
  ImGui::SetNextItemWidth(-1.0f);
  if (ImGui::BeginCombo(rotulo, nomeModelo)) {
    for (const TModeloOctree& modelo : modelos) {
      ImGui::PushID(modelo.Id());
      if (ImGui::Selectable(modelo.Nome().c_str(), modelo.Id() == id)) {
        id = modelo.Id();
      }
      ImGui::PopID();
    }
    ImGui::EndCombo();
  }
  return id != 0;
}

void TPainelModelador::DesenhaOperacoes()
{
  Rotulo("UNIÃO · DOIS MODELOS");
  if (segundoOperando == 0 && modelador.Modelos().size() > 1) {
    segundoOperando = modelador.Modelos()[1].Id();
  }
  EscolheModelo("##primeiro", primeiroOperando);
  EscolheModelo("##segundo", segundoOperando);
  ImGui::BeginDisabled(primeiroOperando == 0 || segundoOperando == 0);
  if (BotaoPrincipal("Unir modelos")) {
    ExecutaAcao([this] { modelador.Une(primeiroOperando, segundoOperando); }, "União criada. Os modelos de entrada foram preservados.");
  }
  ImGui::EndDisabled();
  ImGui::Separator();
  Rotulo("ESCALA · MODELO SELECIONADO");
  ImGui::SetNextItemWidth(-1.0f);
  ImGui::InputFloat("##fator", &fatorEscala, 0.0f, 0.0f, "%.3f");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Fator positivo. A escala usa a origem como ponto fixo e gera um novo modelo.");
  }
  ImGui::BeginDisabled(modelador.Selecionado() == nullptr);
  if (BotaoPrincipal("Aplicar escala")) {
    ExecutaAcao([this] { modelador.Escala(modelador.Selecionado()->Id(), fatorEscala); }, "Modelo escalado criado. O modelo original foi preservado.");
  }
  ImGui::EndDisabled();
}

//----------------------------------------------------------------------------------------------

void TPainelModelador::DesenhaArquivos()
{
  Rotulo("CAMINHO DO ARQUIVO");
  ImGui::SetNextItemWidth(-1.0f);
  ImGui::InputTextWithHint("##caminho", "Ex.: modelos/carro.df", &caminho);
  if (BotaoPrincipal("Abrir como novo modelo")) {
    ExecutaAcao([this] { modelador.Abre(std::filesystem::u8path(caminho)); }, "Arquivo aberto e selecionado.");
  }
  ImGui::BeginDisabled(modelador.Selecionado() == nullptr);
  if (BotaoPrincipal("Salvar selecionado")) {
    SolicitaSalvar();
  }
  ImGui::EndDisabled();
  ImGui::TextWrapped("Formato DF do professor: somente B, W e (. A leitura usa a profundidade escolhida acima.");
  ImGui::PushTextWrapPos(0.0f);
  ImGui::TextDisabled("Caminhos relativos usam a pasta de execução.");
  ImGui::PopTextWrapPos();
}

void TPainelModelador::SolicitaSalvar()
{
  try {
    if (std::filesystem::exists(std::filesystem::u8path(caminho))) {
      caminhoPendente = caminho;
      modeloPendente = modelador.Selecionado()->Id();
      confirmarSubstituicao = true;
    } else {
      ExecutaAcao([this] { modelador.Salva(modelador.Selecionado()->Id(), std::filesystem::u8path(caminho)); }, "Modelo salvo no formato DF.");
    }
  } catch (const std::exception& excecao) {
    mensagem = excecao.what();
    erro = true;
  }
}

void TPainelModelador::DesenhaConfirmacao()
{
  if (confirmarSubstituicao) {
    ImGui::OpenPopup("Substituir arquivo?");
    confirmarSubstituicao = false;
  }
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(460.0f, 0.0f), ImGuiCond_Appearing);
  if (ImGui::BeginPopupModal("Substituir arquivo?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextWrapped("O arquivo já existe. Deseja substituir seu conteúdo?");
    ImGui::TextWrapped("%s", caminhoPendente.c_str());
    if (ImGui::Button("Substituir", ImVec2(180.0f, 36.0f))) {
      ExecutaAcao([this] { modelador.Salva(modeloPendente, std::filesystem::u8path(caminhoPendente), true); }, "Arquivo substituído e salvo no formato DF.");
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancelar", ImVec2(180.0f, 36.0f))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelador::DesenhaVisualizacao()
{
  const ImVec2 tela = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(LARGURA_CONTROLES + 2.0f * MARGEM, 92.0f));
  ImGui::SetNextWindowSize(ImVec2(tela.x - LARGURA_CONTROLES - 3.0f * MARGEM, tela.y - 154.0f));
  ImGui::Begin("Visualizacao", nullptr, FLAGS_PAINEL | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
  const TModeloOctree* modelo = modelador.Selecionado();
  Titulo(modelo == nullptr ? "Visualização" : modelo->Nome().c_str());
  const char* rotuloSolido = "Sólido";
  const char* rotuloEstrutura = "Estrutura da octree";
  const ImGuiStyle& estilo = ImGui::GetStyle();
  const float larguraSolido = ImGui::GetFrameHeight() + estilo.ItemInnerSpacing.x + ImGui::CalcTextSize(rotuloSolido).x;
  const float larguraEstrutura = ImGui::CalcTextSize(rotuloEstrutura).x + 2.0f * estilo.FramePadding.x;
  ImGui::SameLine();
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, ImGui::GetContentRegionAvail().x - larguraSolido - estilo.ItemSpacing.x * 2.0f - larguraEstrutura));
  ImGui::Checkbox(rotuloSolido, &solido);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Preenche as faces externas das folhas cheias. Desmarcado, mostra o aramado.");
  }
  ImGui::SameLine(0.0f, estilo.ItemSpacing.x * 2.0f);
  ImGui::BeginDisabled(modelo == nullptr);
  if (ImGui::Button(rotuloEstrutura)) {
    janelaEstrutura.Abre();
  }
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
    ImGui::SetTooltip("Inspecionar todos os nós da octree: cheios, vazios e parciais.");
  }
  ImGui::EndDisabled();
  Rotulo(solido ? "SUPERFÍCIE · FOLHAS CHEIAS DA OCTREE" : "ARAMADO · FOLHAS CHEIAS DA OCTREE");
  const float larguraCartao = (ImGui::GetContentRegionAvail().x - 20.0f) / 3.0f;
  const std::string volume = modelo == nullptr ? "--" : std::to_string(modelo->Volume()) + " u³";
  const std::string profundidadeModelo = modelo == nullptr ? "--" : std::to_string(modelo->Octree().Configuracao().ProfundidadeMaxima());
  const std::string celulas = modelo == nullptr ? "--" : std::to_string(modelo->Arestas().size() / 12);
  Cartao("Volume", volume.c_str(), larguraCartao);
  ImGui::SameLine();
  Cartao("Profundidade", profundidadeModelo.c_str(), larguraCartao);
  ImGui::SameLine();
  Cartao("Células cheias", celulas.c_str(), larguraCartao);

  posicaoCena = ImGui::GetCursorScreenPos();
  tamanhoCena = ImVec2(ImGui::GetContentRegionAvail().x, std::max(1.0f, ImGui::GetContentRegionAvail().y - 34.0f));
  ImGui::InvisibleButton("Cena", tamanhoCena);
  ImDrawList* desenho = ImGui::GetWindowDrawList();
  desenho->AddRect(posicaoCena, ImVec2(posicaoCena.x + tamanhoCena.x, posicaoCena.y + tamanhoCena.y), IM_COL32(40, 55, 73, 255), 8.0f);
  if (modelo == nullptr) {
    const char* orientacao = "Seu modelo aparecerá aqui";
    const ImVec2 texto = ImGui::CalcTextSize(orientacao);
    desenho->AddText(ImVec2(posicaoCena.x + (tamanhoCena.x - texto.x) / 2.0f, posicaoCena.y + tamanhoCena.y / 2.0f), IM_COL32(135, 156, 179, 255), orientacao);
  }
  Rotulo("Vista ortográfica fixa · X / Y / Z · domínio [-1, 1]³");
  ImGui::End();
}

void TPainelModelador::DesenhaModelo(
  int larguraFramebuffer,
  int alturaFramebuffer
) const
{
  const TModeloOctree* modelo = modelador.Selecionado();
  const ImVec2 tela = ImGui::GetIO().DisplaySize;
  if (modelo == nullptr || tela.x <= 0.0f || tela.y <= 0.0f) {
    return;
  }
  const float escalaX = larguraFramebuffer / tela.x;
  const float escalaY = alturaFramebuffer / tela.y;
  // ImGui mede a cena a partir do topo; OpenGL mede o viewport a partir da base.
  const int x = static_cast<int>(posicaoCena.x * escalaX);
  const int y = static_cast<int>((tela.y - posicaoCena.y - tamanhoCena.y) * escalaY);
  const int largura = static_cast<int>(tamanhoCena.x * escalaX);
  const int altura = static_cast<int>(tamanhoCena.y * escalaY);
  const TCubo& dominio = modelo->Octree().Configuracao().Dominio();
  if (solido) {
    TRenderizadorAramado().DesenhaSolido(modelo->Faces(), modelo->Arestas(), dominio, x, y, largura, altura);
  } else {
    TRenderizadorAramado().Desenha(modelo->Arestas(), dominio, x, y, largura, altura);
  }
}

void TPainelModelador::DesenhaMensagem()
{
  const ImVec2 tela = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(MARGEM, tela.y - 50.0f));
  ImGui::SetNextWindowSize(ImVec2(tela.x - 2.0f * MARGEM, 38.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 8.0f));
  ImGui::Begin("Mensagem", nullptr, FLAGS_PAINEL);
  ImGui::TextColored(erro ? ImVec4(1.0f, 0.53f, 0.47f, 1.0f) : ImVec4(0.48f, 0.81f, 0.72f, 1.0f), "%s", mensagem.c_str());
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", mensagem.c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar();
}

void TPainelModelador::ExecutaAcao(
  const std::function<void()>& acao,
  const char* sucesso
)
{
  try {
    acao();
    mensagem = sucesso;
    erro = false;
  } catch (const std::exception& excecao) {
    mensagem = excecao.what();
    erro = true;
  }
}

//----------------------------------------------------------------------------------------------
