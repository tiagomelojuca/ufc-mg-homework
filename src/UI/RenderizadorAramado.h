#ifndef RENDERIZADOR_ARAMADO_H_
#define RENDERIZADOR_ARAMADO_H_

#include <vector>

#include "Core/AramadoOctree.h"
#include "Core/SuperficieOctree.h"

//----------------------------------------------------------------------------------------------

struct TLoteAramado
{
  const std::vector<TAresta3D>* arestas;
  float cor[3];
};

//----------------------------------------------------------------------------------------------

struct TRetanguloTela
{
  int x;
  int y;
  int largura;
  int altura;
};

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

    // Desenha os lotes na ordem recebida; o recorte limita viewport e limpeza de profundidade.
    void Desenha(
      const std::vector<TLoteAramado>& lotes,
      const TCubo& enquadramento,
      const TRetanguloTela& area,
      const TRetanguloTela& recorte
    ) const;

    // Preenche as faces e desenha as arestas por cima. Sem iluminação, usa um tom fixo por orientação;
    // com iluminação, aplica o modelo de Phong em cada vértice.
    void DesenhaSolido(
      const std::vector<TFace3D>& faces,
      const std::vector<TAresta3D>& arestas,
      const TCubo& dominio,
      bool iluminado,
      int x,
      int y,
      int largura,
      int altura
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
