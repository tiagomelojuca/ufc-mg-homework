#ifndef EXIBICAOSMC_H_
#define EXIBICAOSMC_H_

#include <array>
#include <vector>

#include "Core/ArvoreCSG.h"
#include "Core/Classificacao.h"
#include "Core/RayCasting.h"
#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

// Arestas classificadas contra a árvore e desenhadas sobre a imagem, cada parte com a cor de IN,
// ON ou OUT. As arestas são desenhadas por cima do sólido, para que as partes IN fiquem visíveis.
class TExibicaoSMC
{
  public:
    TExibicaoSMC();

    // As arestas voltam às três paralelas aos eixos pelo centro do novo modelo.
    void DefineArvore(
      TArvoreCSG novaArvore
    );

    // Controles das arestas, legenda e lista dos trechos classificados.
    void DesenhaControles();

    // Deve ser chamado logo após desenhar a imagem com a mesma câmera, na posição origem da tela.
    void DesenhaSobreImagem(
      const TCamera& camera,
      const ImVec2& origem,
      int largura,
      int altura
    );

  private:
    struct TArestaEditavel
    {
      std::array<float, 3> inicio;
      std::array<float, 3> fim;
    };

    // Classifica as arestas novamente apenas quando a árvore ou alguma aresta muda.
    const std::vector<TClassificacaoAresta>& Classificacoes();

    TArvoreCSG arvore;
    std::vector<TArestaEditavel> arestas;
    std::vector<TClassificacaoAresta> classificacoes;
    bool precisaClassificar;
    bool visivel;
};

//----------------------------------------------------------------------------------------------

#endif
