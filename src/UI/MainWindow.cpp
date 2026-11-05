#include "MainWindow.h"

#include <memory>

#include "Core/Bloco.h"
#include "Core/Esfera.h"

namespace
{
  // Cena exibida enquanto a interface ainda não cria modelos: um bloco girado unido a uma esfera.
  TArvoreCSG CriaCenaDeExemplo()
  {
    const TArvoreCSG bloco = CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, { 0.0, 30.0, 0.0 }),
      CriaPrimitiva(std::make_shared<TBloco>(3.0, 1.5, 2.0))
    );

    const TArvoreCSG esfera = CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, { 0.8, 0.9, 0.3 }),
      CriaPrimitiva(std::make_shared<TEsfera>(1.0))
    );

    return CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, bloco, esfera);
  }
}

//----------------------------------------------------------------------------------------------
// Class TGanchoImGuiMainWindow
//----------------------------------------------------------------------------------------------

TGanchoImGuiMainWindow::TGanchoImGuiMainWindow(
  TWindow& wnd
) :
  TWindow::TGancho(wnd)
{
}

//----------------------------------------------------------------------------------------------

std::unique_ptr<TWindow::TGancho> TGanchoImGuiMainWindow::Copia() const
{
  return std::make_unique<TGanchoImGuiMainWindow>(*this);
}

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
  TWindow::TEstrategiaProcessamento(wnd),
  painelRayCasting(CriaCenaDeExemplo())
{
}

//----------------------------------------------------------------------------------------------

std::unique_ptr<TWindow::TEstrategiaProcessamento> TEstrategiaProcessamentoMainWindow::Copia() const
{
  return std::make_unique<TEstrategiaProcessamentoMainWindow>(*this);
}

//----------------------------------------------------------------------------------------------

void TEstrategiaProcessamentoMainWindow::Executa()
{
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Modelador CSG");
  ImGui::End();

  painelRayCasting.Desenha();

  ImGui::Render();

  int display_w, display_h;
  glfwGetFramebufferSize(parent.InstanciaGLFW(), &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(0.055f, 0.075f, 0.11f, 1.0f);
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
  wnd.AdicionaHook(TGanchoImGuiMainWindow { wnd });
  wnd.EstrategiaMainLoop(TEstrategiaProcessamentoMainWindow { wnd });
}

//----------------------------------------------------------------------------------------------

TWindow::ECodigoRetorno TMainWindow::Executa()
{
  return wnd.Executa();
}

//----------------------------------------------------------------------------------------------
