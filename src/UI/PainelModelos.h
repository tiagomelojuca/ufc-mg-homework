#ifndef PAINELMODELOS_H_
#define PAINELMODELOS_H_

#include <array>
#include <optional>
#include <string>

#include "Application/Modelador.h"
#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

// Painel que dá acesso aos casos de uso: lista e seleção de modelos, criação de primitivas,
// operações sobre o modelo selecionado, cálculo de volume e arquivos.
class TPainelModelos
{
  public:
    TPainelModelos();

    // Deve ser chamado entre ImGui::NewFrame e ImGui::Render.
    void Desenha(
      TModelador& modelador
    );

    // Modelo selecionado, se ainda existir.
    std::optional<TIdModelo> Selecionado(
      const TModelador& modelador
    ) const;

  private:
    enum class ETipoCriacao
    {
      BLOCO,
      ESFERA
    };

    void DesenhaLista(
      TModelador& modelador
    );

    void DesenhaCriacao(
      TModelador& modelador
    );

    void DesenhaOperacoes(
      TModelador& modelador,
      TIdModelo selecionado
    );

    void DesenhaAnalise(
      const TModelador& modelador,
      TIdModelo selecionado
    );

    void DesenhaArquivos(
      TModelador& modelador
    );

    void DesenhaMensagem() const;

    // Executa uma ação do usuário, guardando o resultado ou o erro como mensagem.
    template <typename TAcao>
    void Executa(
      TAcao acao
    );

    std::optional<TIdModelo> selecionado;
    std::optional<TIdModelo> segundoOperando;

    ETipoCriacao tipoCriacao;
    std::array<double, 3> lados;
    double raio;

    std::array<double, 3> deslocamento;
    std::array<double, 3> angulos;
    std::array<double, 3> fatores;

    int profundidadeOctree;
    std::string resultadoVolume;

    std::string caminhoArquivo;

    std::string mensagem;
    bool mensagemDeErro;
};

//----------------------------------------------------------------------------------------------

#endif
