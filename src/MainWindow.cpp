#include "MainWindow.h"

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

void TEstrategiaProcessamentoMainWindow::Executa()
{
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::ShowDemoWindow();
  ImGui::Render();

  int display_w, display_h;
  glfwGetFramebufferSize(parent.InstanciaGLFW(), &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

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
