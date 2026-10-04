#ifndef JANELA_ESTRUTURA_OCTREE_H_
#define JANELA_ESTRUTURA_OCTREE_H_

#include <cstddef>
#include <vector>

#include "Application/Modelador.h"
#include "Core/AnaliseOctree.h"
#include "Core/AramadoOctree.h"
#include "UiFramework.h"

//----------------------------------------------------------------------------------------------

class TJanelaEstruturaOctree
{
  public:
    void Abre();
    void Desenha(
      const TModeloOctree* modelo
    );

  private:
    void Atualiza(
      const TModeloOctree& modelo
    );
    void DesenhaCaminho();
    void DesenhaArvore(
      const TNoOctree& no,
      const char* rotulo,
      std::vector<std::size_t>& caminhoAtual
    );
    void DesenhaContagens() const;
    void DesenhaCena();
    static void DesenhaCenaOpenGL(
      const ImDrawList* lista,
      const ImDrawCmd* comando
    );

    bool aberta = false;
    bool focar = false;
    int idModelo = 0;
    std::vector<std::size_t> caminho;
    int niveis = 3;
    bool mostrarVazias = true;

    std::vector<std::size_t> caminhoCalculado;
    int niveisCalculados = -1;
    TEstruturaAramadaOctree estrutura;
    std::vector<TContagemNivelOctree> contagens;
    TCubo enquadramento = TCubo({ 0.0, 0.0, 0.0 }, 2.0);
    ImVec2 posicaoCena;
    ImVec2 tamanhoCena;
};

//----------------------------------------------------------------------------------------------

#endif
