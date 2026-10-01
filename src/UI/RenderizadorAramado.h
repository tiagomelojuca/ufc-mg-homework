#ifndef RENDERIZADOR_ARAMADO_H_
#define RENDERIZADOR_ARAMADO_H_

#include "Core/AramadoOctree.h"

//----------------------------------------------------------------------------------------------

class TRenderizadorAramado
{
  public:
    void Desenha(
      const std::vector<TAresta3D>& arestas,
      const TCubo& dominio,
      int x,
      int y,
      int largura,
      int altura
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
