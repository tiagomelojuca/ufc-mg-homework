#include "PainelModelos.h"

#include <cstdio>
#include <exception>

#include "misc/cpp/imgui_stdlib.h"

namespace
{
  TVetor3D ParaVetor(
    const std::array<double, 3>& valores
  )
  {
    return { valores[0], valores[1], valores[2] };
  }

  //--------------------------------------------------------------------------------------------

  bool CampoTriplo(
    const char* rotulo,
    std::array<double, 3>& valores
  )
  {
    return ImGui::InputScalarN(rotulo, ImGuiDataType_Double, valores.data(), 3, nullptr, nullptr, "%.3f");
  }
}

//----------------------------------------------------------------------------------------------

TPainelModelos::TPainelModelos() :
  tipoCriacao(ETipoCriacao::BLOCO),
  lados({ 2.0, 2.0, 2.0 }),
  raio(1.0),
  deslocamento({ 0.0, 0.0, 0.0 }),
  angulos({ 0.0, 0.0, 0.0 }),
  fatores({ 1.0, 1.0, 1.0 }),
  profundidadeOctree(5),
  caminhoArquivo("modelo.txt"),
  mensagemDeErro(false)
{
}

//----------------------------------------------------------------------------------------------

template <typename TAcao>
void TPainelModelos::Executa(
  TAcao acao
)
{
  try {
    mensagem = acao();
    mensagemDeErro = false;
  } catch (const std::exception& erro) {
    mensagem = erro.what();
    mensagemDeErro = true;
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelos::Desenha(
  TModelador& modelador
)
{
  ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(440.0f, 700.0f), ImGuiCond_FirstUseEver);
  ImGui::Begin("Modelos");

  DesenhaLista(modelador);
  DesenhaCriacao(modelador);

  const std::optional<TIdModelo> atual = Selecionado(modelador);
  if (atual) {
    DesenhaOperacoes(modelador, *atual);
    DesenhaAnalise(modelador, *atual);
  }

  DesenhaArquivos(modelador);
  DesenhaMensagem();

  ImGui::End();
}

//----------------------------------------------------------------------------------------------

std::optional<TIdModelo> TPainelModelos::Selecionado(
  const TModelador& modelador
) const
{
  if (selecionado && modelador.Existe(*selecionado)) {
    return selecionado;
  }

  return std::nullopt;
}

//----------------------------------------------------------------------------------------------

void TPainelModelos::DesenhaLista(
  TModelador& modelador
)
{
  ImGui::SeparatorText("Lista");

  if (modelador.Modelos().empty()) {
    ImGui::TextDisabled("Nenhum modelo. Crie uma primitiva ou carregue um arquivo.");
    return;
  }

  if (ImGui::BeginListBox("##modelos", ImVec2(-1.0f, 8.0f * ImGui::GetTextLineHeightWithSpacing()))) {
    for (const TModelo& modelo : modelador.Modelos()) {
      if (ImGui::Selectable(modelo.nome.c_str(), selecionado == modelo.id)) {
        selecionado = modelo.id;
      }
    }

    ImGui::EndListBox();
  }

  const std::optional<TIdModelo> atual = Selecionado(modelador);
  ImGui::BeginDisabled(!atual);
  if (ImGui::Button("Remover selecionado")) {
    Executa([&]() {
      const std::string nome = modelador.Modelo(*atual).nome;
      modelador.Remove(*atual);
      selecionado.reset();
      return "Removido " + nome;
    });
  }
  ImGui::EndDisabled();
}

//----------------------------------------------------------------------------------------------

void TPainelModelos::DesenhaCriacao(
  TModelador& modelador
)
{
  ImGui::SeparatorText("Criar primitiva");

  int tipo = static_cast<int>(tipoCriacao);
  ImGui::RadioButton("Bloco", &tipo, static_cast<int>(ETipoCriacao::BLOCO));
  ImGui::SameLine();
  ImGui::RadioButton("Esfera", &tipo, static_cast<int>(ETipoCriacao::ESFERA));
  tipoCriacao = static_cast<ETipoCriacao>(tipo);

  if (tipoCriacao == ETipoCriacao::BLOCO) {
    CampoTriplo("Lados x, y, z", lados);
  } else {
    ImGui::InputDouble("Raio", &raio, 0.0, 0.0, "%.3f");
  }

  if (ImGui::Button("Criar")) {
    Executa([&]() {
      const TIdModelo id = tipoCriacao == ETipoCriacao::BLOCO
        ? modelador.CriaBloco(lados[0], lados[1], lados[2])
        : modelador.CriaEsfera(raio);
      selecionado = id;
      return "Criado " + modelador.Modelo(id).nome;
    });
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelos::DesenhaOperacoes(
  TModelador& modelador,
  TIdModelo atual
)
{
  ImGui::SeparatorText("Operações sobre o selecionado");
  ImGui::TextWrapped("%s", modelador.Modelo(atual).nome.c_str());

  // Cada operação cria um novo modelo, que passa a ser o selecionado.
  const auto opera = [&](auto operacao) {
    Executa([&]() {
      const TIdModelo id = operacao();
      selecionado = id;
      return "Criado " + modelador.Modelo(id).nome;
    });
  };

  // União com outro modelo da lista.
  if (segundoOperando && !modelador.Existe(*segundoOperando)) {
    segundoOperando.reset();
  }

  const char* previa = segundoOperando ? modelador.Modelo(*segundoOperando).nome.c_str() : "Escolha o segundo operando";
  if (ImGui::BeginCombo("##segundo", previa)) {
    for (const TModelo& modelo : modelador.Modelos()) {
      if (ImGui::Selectable(modelo.nome.c_str(), segundoOperando == modelo.id)) {
        segundoOperando = modelo.id;
      }
    }

    ImGui::EndCombo();
  }

  ImGui::SameLine();
  ImGui::BeginDisabled(!segundoOperando);
  if (ImGui::Button("Unir")) {
    opera([&]() { return modelador.Une(atual, *segundoOperando); });
  }
  ImGui::EndDisabled();

  CampoTriplo("##deslocamento", deslocamento);
  ImGui::SameLine();
  if (ImGui::Button("Transladar")) {
    opera([&]() { return modelador.Translada(atual, ParaVetor(deslocamento)); });
  }

  CampoTriplo("##angulos", angulos);
  ImGui::SameLine();
  if (ImGui::Button("Rotacionar (graus)")) {
    opera([&]() { return modelador.Rotaciona(atual, ParaVetor(angulos)); });
  }

  CampoTriplo("##fatores", fatores);
  ImGui::SameLine();
  if (ImGui::Button("Escalar")) {
    opera([&]() { return modelador.Escala(atual, ParaVetor(fatores)); });
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelos::DesenhaAnalise(
  const TModelador& modelador,
  TIdModelo atual
)
{
  ImGui::SeparatorText("Análise");

  ImGui::SliderInt("Profundidade da octree", &profundidadeOctree, 1, 8);
  if (ImGui::Button("Calcular volume")) {
    Executa([&]() {
      const double volume = modelador.Volume(atual, profundidadeOctree);

      char texto[128];
      std::snprintf(texto, sizeof(texto), "Volume: %.6f (profundidade %d)", volume, profundidadeOctree);
      resultadoVolume = "#" + std::to_string(atual) + " " + texto;
      return std::string("Volume calculado");
    });
  }

  if (!resultadoVolume.empty()) {
    ImGui::TextUnformatted(resultadoVolume.c_str());
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelos::DesenhaArquivos(
  TModelador& modelador
)
{
  ImGui::SeparatorText("Arquivos");

  ImGui::InputText("Caminho", &caminhoArquivo);

  const std::optional<TIdModelo> atual = Selecionado(modelador);
  ImGui::BeginDisabled(!atual);
  if (ImGui::Button("Salvar selecionado")) {
    Executa([&]() {
      modelador.Salva(*atual, caminhoArquivo);
      return "Salvo em " + caminhoArquivo;
    });
  }
  ImGui::EndDisabled();

  ImGui::SameLine();
  if (ImGui::Button("Carregar")) {
    Executa([&]() {
      const TIdModelo id = modelador.Carrega(caminhoArquivo);
      selecionado = id;
      return "Criado " + modelador.Modelo(id).nome;
    });
  }
}

//----------------------------------------------------------------------------------------------

void TPainelModelos::DesenhaMensagem() const
{
  if (mensagem.empty()) {
    return;
  }

  ImGui::Separator();

  const ImVec4 cor = mensagemDeErro ? ImVec4(1.0f, 0.4f, 0.4f, 1.0f) : ImVec4(0.6f, 0.9f, 0.6f, 1.0f);
  ImGui::PushStyleColor(ImGuiCol_Text, cor);
  ImGui::TextWrapped("%s", mensagem.c_str());
  ImGui::PopStyleColor();
}

//----------------------------------------------------------------------------------------------
