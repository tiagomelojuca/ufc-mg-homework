#ifndef PAINELRAYCASTING_H_
#define PAINELRAYCASTING_H_

#include "Core/ArvoreCSG.h"
#include "Core/RayCasting.h"
#include "ExibicaoSMC.h"
#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

// Painel que exibe a imagem da árvore gerada por ray-casting, com a câmera em órbita em torno do
// centro do modelo, e as arestas classificadas pela SMC sobre ela. A imagem só é recalculada quando
// a árvore ou a câmera mudam.
class TPainelRayCasting
{
  public:
    TPainelRayCasting();

    // Uma árvore vazia deixa o painel sem modelo. A câmera é reenquadrada no novo modelo.
    void DefineArvore(
      TArvoreCSG novaArvore
    );

    // Deve ser chamado entre ImGui::NewFrame e ImGui::Render, com o contexto OpenGL ativo.
    void Desenha();

  private:
    TCena MontaCena() const;
    void AtualizaTextura();

    TArvoreCSG arvore;
    TExibicaoSMC exibicaoSMC;

    // Centro da caixa envolvente do modelo e metade da sua diagonal.
    TVetor3D alvo;
    double raioDaCena;

    float azimuteGraus;
    float elevacaoGraus;
    float distancia;
    int largura;
    int altura;

    bool precisaRenderizar;

    // Criada no primeiro desenho; liberada com o contexto OpenGL ao fechar a janela.
    GLuint textura;
};

//----------------------------------------------------------------------------------------------

#endif
