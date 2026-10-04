#ifndef PAINEL_MODELADOR_H_
#define PAINEL_MODELADOR_H_

#include <functional>
#include <string>

#include "Application/Modelador.h"
#include "JanelaEstruturaOctree.h"
#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

class TPainelModelador
{
  public:
    static void ConfiguraEstilo();
    void Desenha();
    void DesenhaModelo(
      int larguraFramebuffer,
      int alturaFramebuffer
    ) const;

  private:
    void DesenhaCabecalho();
    void DesenhaControles();
    void DesenhaLista();
    void DesenhaCriacao();
    void DesenhaOperacoes();
    void DesenhaArquivos();
    void DesenhaVisualizacao();
    void DesenhaMensagem();
    void DesenhaConfirmacao();
    void SolicitaSalvar();
    void ExecutaAcao(
      const std::function<void()>& acao,
      const char* sucesso
    );
    bool EscolheModelo(
      const char* rotulo,
      int& id
    );

    TModelador modelador;
    TJanelaEstruturaOctree janelaEstrutura;
    int profundidade = 5;
    int primitiva = 0;
    std::string nome;
    float centro[3] = { 0.0f, 0.0f, 0.0f };
    float lados[3] = { 1.0f, 1.5f, 0.75f };
    float raio = 0.5f;
    float fatorEscala = 0.5f;
    bool solido = false;
    int primeiroOperando = 0;
    int segundoOperando = 0;
    int ultimaSelecao = 0;
    std::string caminho = "modelo.df";
    std::string caminhoPendente;
    int modeloPendente = 0;
    bool confirmarSubstituicao = false;
    std::string mensagem = "Crie uma primitiva ou abra um arquivo para começar.";
    bool erro = false;
    ImVec2 posicaoCena;
    ImVec2 tamanhoCena;
};

//----------------------------------------------------------------------------------------------

#endif
