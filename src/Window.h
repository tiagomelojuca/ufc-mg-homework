#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include <vector>

#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

class TWindow
{
  public:
    enum ECodigoRetorno { OK, FALHA_INICIALIZACAO };

    class TGancho
    {
      public:
        TGancho(
          TWindow& parent
        ) :
          parent(parent)
        {
        }

        virtual ~TGancho() = default;

        virtual TGancho* Copia() const = 0;

        virtual bool Inicializa() = 0;
        virtual void Limpa() = 0;

      protected:
        TWindow& parent;
    };

    class TEstrategiaProcessamento
    {
      public:
        TEstrategiaProcessamento(
          TWindow& parent
        ) :
          parent(parent)
        {
        }
        virtual ~TEstrategiaProcessamento() = default;

        virtual TEstrategiaProcessamento* Copia() const = 0;

        virtual void Executa() = 0;

      protected:
        TWindow& parent;
    };

    TWindow() = delete;

    explicit TWindow(
      const char* titulo,
      int largura,
      int altura
    );

    TWindow(
      const TWindow&
    ) = delete;

    TWindow(
      TWindow&&
    ) = delete;

    ~TWindow();

    TWindow& operator=(
      const TWindow&
    ) = delete;

    void AdicionaHook(
      const TGancho& hook
    );

    void EstrategiaMainLoop(
      const TEstrategiaProcessamento& estrategia
    );

    ECodigoRetorno Executa();

    GLFWwindow* InstanciaGLFW();

  private:
    bool Inicializa();
    void Processa();
    void Limpa();

    std::string titulo;
    int largura;
    int altura;

    GLFWwindow* instanciaGlfw;
    std::vector<TGancho*> hooks;
    TEstrategiaProcessamento* estrategiaMainLoop;
};

//----------------------------------------------------------------------------------------------

#endif
