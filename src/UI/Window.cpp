#include "Window.h"

#include <iostream>

namespace
{
  void RegistraErroGlfw(int codigo, const char* descricao)
  {
    std::cerr << "GLFW (" << codigo << "): " << descricao << '\n';
  }
}

//----------------------------------------------------------------------------------------------

TWindow::TWindow(
  const char* titulo,
  int largura,
  int altura
) :
  titulo(titulo),
  largura(largura),
  altura(altura),
  instanciaGlfw(nullptr),
  estrategiaMainLoop(nullptr)
{
}

//----------------------------------------------------------------------------------------------

TWindow::~TWindow()
{
  for (TGancho* hook : hooks) {
    delete hook;
  }
  delete estrategiaMainLoop;
}

//----------------------------------------------------------------------------------------------

void TWindow::AdicionaHook(
  const TGancho& hook
)
{
  hooks.push_back(hook.Copia());
}

//----------------------------------------------------------------------------------------------

void TWindow::EstrategiaMainLoop(
  const TEstrategiaProcessamento& estrategia
)
{
  delete estrategiaMainLoop;
  estrategiaMainLoop = estrategia.Copia();
}

//----------------------------------------------------------------------------------------------

TWindow::ECodigoRetorno TWindow::Executa()
{
  const bool inicializou = Inicializa();
  if (!inicializou) {
    return ECodigoRetorno::FALHA_INICIALIZACAO;
  }

  Processa();
  Limpa();

  return ECodigoRetorno::OK;
}

//----------------------------------------------------------------------------------------------

GLFWwindow* TWindow::InstanciaGLFW()
{
  return instanciaGlfw;
}

//----------------------------------------------------------------------------------------------

bool TWindow::Inicializa()
{
  glfwSetErrorCallback(RegistraErroGlfw);

  if (!glfwInit()) {
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  instanciaGlfw = glfwCreateWindow(largura, altura, titulo.c_str(), nullptr, nullptr);
  if (instanciaGlfw == nullptr) {
    glfwTerminate();

    return false;
  }

  glfwMakeContextCurrent(instanciaGlfw);
  glfwSetWindowSizeLimits(instanciaGlfw, 960, 640, GLFW_DONT_CARE, GLFW_DONT_CARE);
  glfwSwapInterval(1);

  for (TGancho* hook : hooks) {
    if (!hook->Inicializa()) {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------

void TWindow::Processa()
{
  if (estrategiaMainLoop != nullptr) {
    while (!glfwWindowShouldClose(instanciaGlfw)) {
      estrategiaMainLoop->Executa();
    }
  }
}

//----------------------------------------------------------------------------------------------

void TWindow::Limpa()
{
  for (TGancho* hook : hooks) {
    hook->Limpa();
  }

  glfwDestroyWindow(instanciaGlfw);
  glfwTerminate();
}

//----------------------------------------------------------------------------------------------
