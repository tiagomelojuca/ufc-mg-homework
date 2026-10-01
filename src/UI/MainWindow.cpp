#include "MainWindow.h"

#include "Core/Bloco.h"
#include "Core/Esfera.h"
#include "RenderizadorAramado.h"

//----------------------------------------------------------------------------------------------
// Class TGanchoImGuiMainWindow
//----------------------------------------------------------------------------------------------

bool TGanchoImGuiMainWindow::Inicializa()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  constexpr const char* glsl_version = "#version 130";
  ImGui_ImplGlfw_InitForOpenGL(parent.InstanciaGLFW(), true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return true;
}

//----------------------------------------------------------------------------------------------

void TGanchoImGuiMainWindow::Limpa()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

//----------------------------------------------------------------------------------------------
// Class TEstrategiaProcessamentoMainWindow
//----------------------------------------------------------------------------------------------

TEstrategiaProcessamentoMainWindow::TEstrategiaProcessamentoMainWindow(
  TWindow& wnd
) :
  TWindow::TEstrategiaProcessamento(wnd)
{
  TOctree bloco(configuracao);
  bloco.Constroi(TClassificadorBlocoOctree(TBloco({ 0.0, 0.0, 0.0 }, 1.0, 1.5, 0.75)));

  TOctree esfera(configuracao);
  esfera.Constroi(TClassificadorEsferaOctree(TEsfera({ 0.0, 0.0, 0.0 }, 0.75)));

  const TGeradorAramadoOctree gerador;
  arestasBloco = gerador.Gera(bloco);
  arestasEsfera = gerador.Gera(esfera);
}

//----------------------------------------------------------------------------------------------

void TEstrategiaProcessamentoMainWindow::Executa()
{
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  const ImVec2 tamanho = ImGui::GetIO().DisplaySize;
  ImDrawList* texto = ImGui::GetForegroundDrawList();
  texto->AddText(ImVec2(16.0f, 16.0f), IM_COL32_WHITE, "Bloco");
  texto->AddText(ImVec2(tamanho.x / 2.0f + 16.0f, 16.0f), IM_COL32_WHITE, "Esfera");
  ImGui::Render();

  int display_w, display_h;
  glfwGetFramebufferSize(parent.InstanciaGLFW(), &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const TRenderizadorAramado renderizador;
  const int larguraBloco = display_w / 2;
  renderizador.Desenha(
    arestasBloco, configuracao.Dominio(), 0, 0, larguraBloco, display_h
  );
  renderizador.Desenha(
    arestasEsfera, configuracao.Dominio(), larguraBloco, 0,
    display_w - larguraBloco, display_h
  );

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(parent.InstanciaGLFW());
}

//----------------------------------------------------------------------------------------------
// Class TMainWindow
//----------------------------------------------------------------------------------------------

TMainWindow::TMainWindow(
  const char* titulo,
  int largura,
  int altura
) :
  wnd(titulo, largura, altura)
{
  TGanchoImGuiMainWindow hookImGui { wnd };
  TEstrategiaProcessamentoMainWindow estrategiaProcessamento { wnd };

  wnd.AdicionaHook(hookImGui);
  wnd.EstrategiaMainLoop(estrategiaProcessamento);
}

//----------------------------------------------------------------------------------------------

TWindow::ECodigoRetorno TMainWindow::Executa()
{
  return wnd.Executa();
}

//----------------------------------------------------------------------------------------------
