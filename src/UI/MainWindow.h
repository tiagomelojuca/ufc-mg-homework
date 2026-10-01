#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Window.h"
#include "Core/AramadoOctree.h"

//----------------------------------------------------------------------------------------------

class TGanchoImGuiMainWindow : public TWindow::TGancho
{
  public:
    explicit TGanchoImGuiMainWindow(
      TWindow& wnd
    ) :
      TWindow::TGancho(wnd)
    {
    }

    TGancho* Copia() const override
    {
      return new TGanchoImGuiMainWindow(*this);
    }

    bool Inicializa() override;
    void Limpa() override;
};

//----------------------------------------------------------------------------------------------

class TEstrategiaProcessamentoMainWindow : public TWindow::TEstrategiaProcessamento
{
  public:
    TEstrategiaProcessamentoMainWindow(
      TWindow& wnd
    );

    TEstrategiaProcessamento* Copia() const override
    {
      return new TEstrategiaProcessamentoMainWindow(*this);
    }

    void Executa() override;

  private:
    TConfiguracaoOctree configuracao;
    std::vector<TAresta3D> arestasBloco;
    std::vector<TAresta3D> arestasEsfera;
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
