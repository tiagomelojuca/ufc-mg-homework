#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Window.h"

//----------------------------------------------------------------------------------------------

class TGanchoImGuiMainWindow : public TWindow::TGancho
{
  public:
    explicit TGanchoImGuiMainWindow(
      TWindow& wnd
    );

    std::unique_ptr<TGancho> Copia() const override;

    bool Inicializa() override;
    void Limpa() override;
};

//----------------------------------------------------------------------------------------------

class TEstrategiaProcessamentoMainWindow : public TWindow::TEstrategiaProcessamento
{
  public:
    explicit TEstrategiaProcessamentoMainWindow(
      TWindow& wnd
    );

    std::unique_ptr<TEstrategiaProcessamento> Copia() const override;

    void Executa() override;
};

//----------------------------------------------------------------------------------------------

class TMainWindow
{
  public:
    TMainWindow() = delete;

    explicit TMainWindow(
      const char* titulo,
      int largura,
      int altura
    );

    TMainWindow(
      const TMainWindow&
    ) = delete;

    TMainWindow(
      TMainWindow&&
    ) = delete;

    TMainWindow& operator=(
      const TMainWindow&
    ) = delete;

    TWindow::ECodigoRetorno Executa();

  private:
    TWindow wnd;
};

//----------------------------------------------------------------------------------------------

#endif
