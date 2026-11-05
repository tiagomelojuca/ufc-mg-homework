#ifndef PAINELRAYCASTING_H_
#define PAINELRAYCASTING_H_

#include <vector>

#include "Core/ArvoreCSG.h"
#include "Core/RayCasting.h"
#include "ExibicaoSMC.h"
#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

// Painel que exibe o modelo por ray-casting ou em aramado, com a câmera em órbita em torno do
// centro do modelo, e as arestas classificadas pela SMC por cima. A imagem do ray-casting só é
// recalculada quando a árvore ou a câmera mudam; os segmentos do aramado, quando a árvore muda.
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
    enum class EModoExibicao
    {
      RAY_CASTING,
      ARAMADO
    };

    TCena MontaCena() const;
    void AtualizaTextura();

    void DesenhaAramado(
      const TCamera& camera,
      const ImVec2& origem
    ) const;

    TArvoreCSG arvore;
    TExibicaoSMC exibicaoSMC;

    EModoExibicao modo;

    // Segmentos EonS do modelo, no sistema da raiz.
    std::vector<TAresta> aramado;

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
