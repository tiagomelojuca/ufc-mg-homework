#include "MainWindow.h"

//----------------------------------------------------------------------------------------------
// Class TGanchoImGuiMainWindow
//----------------------------------------------------------------------------------------------

bool TGanchoImGuiMainWindow::Inicializa()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  TPainelModelador::ConfiguraEstilo();

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
}

//----------------------------------------------------------------------------------------------

void TEstrategiaProcessamentoMainWindow::Executa()
{
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  painel.Desenha();
  ImGui::Render();

  int display_w, display_h;
  glfwGetFramebufferSize(parent.InstanciaGLFW(), &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(0.055f, 0.075f, 0.11f, 1.0f);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  painel.DesenhaModelo(display_w, display_h);

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
