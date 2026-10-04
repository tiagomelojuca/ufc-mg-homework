#include "RenderizadorAramado.h"

#include <GLFW/glfw3.h>

namespace
{
  // Prepara viewport, recorte, profundidade e a vista ortográfica fixa; restaura tudo ao sair.
  class TCenaOpenGL
  {
    public:
      TCenaOpenGL(
        const TCubo& enquadramento,
        const TRetanguloTela& area,
        const TRetanguloTela& recorte
      )
      {
        glGetIntegerv(GL_MATRIX_MODE, &modoMatriz);
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glViewport(area.x, area.y, area.largura, area.altura);
        glEnable(GL_SCISSOR_TEST);
        glScissor(recorte.x, recorte.y, recorte.largura, recorte.altura);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        glLineWidth(1.0f);

        const double aspecto = static_cast<double>(area.largura) / area.altura;
        const double alcance = enquadramento.Lado();
        const double alcanceX = aspecto >= 1.0 ? alcance * aspecto : alcance;
        const double alcanceY = aspecto >= 1.0 ? alcance : alcance / aspecto;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-alcanceX, alcanceX, -alcanceY, alcanceY, -4.0 * alcance, 4.0 * alcance);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glRotated(25.0, 1.0, 0.0, 0.0);
        glRotated(-35.0, 0.0, 1.0, 0.0);
        glTranslated(-enquadramento.Centro().x, -enquadramento.Centro().y, -enquadramento.Centro().z);
      }

      ~TCenaOpenGL()
      {
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(modoMatriz);
        glPopAttrib();
      }

      TCenaOpenGL(
        const TCenaOpenGL&
      ) = delete;

      TCenaOpenGL& operator=(
        const TCenaOpenGL&
      ) = delete;

    private:
      GLint modoMatriz;
  };

  bool AreaValida(
    const TRetanguloTela& area
  )
  {
    return area.largura > 0 && area.altura > 0;
  }

  void DesenhaArestas(
    const std::vector<TAresta3D>& arestas
  )
  {
    for (const TAresta3D& aresta : arestas) {
      glVertex3d(aresta.inicio.x, aresta.inicio.y, aresta.inicio.z);
      glVertex3d(aresta.fim.x, aresta.fim.y, aresta.fim.z);
    }
  }

  // Tom constante por orientação da face, sem modelo de iluminação, para distinguir os lados.
  float TomFace(
    const TCoordenada3D& normal
  )
  {
    if (normal.y > 0.0) return 1.0f;
    if (normal.y < 0.0) return 0.45f;
    if (normal.x > 0.0) return 0.62f;
    if (normal.x < 0.0) return 0.55f;
    if (normal.z > 0.0) return 0.82f;
    return 0.5f;
  }
}

//----------------------------------------------------------------------------------------------

void TRenderizadorAramado::Desenha(
  const std::vector<TAresta3D>& arestas,
  const TCubo& dominio,
  int x,
  int y,
  int largura,
  int altura
) const
{
  const TRetanguloTela area = { x, y, largura, altura };
  Desenha({ { &arestas, { 0.35f, 0.8f, 1.0f } } }, dominio, area, area);
}

//----------------------------------------------------------------------------------------------

void TRenderizadorAramado::Desenha(
  const std::vector<TLoteAramado>& lotes,
  const TCubo& enquadramento,
  const TRetanguloTela& area,
  const TRetanguloTela& recorte
) const
{
  if (!AreaValida(area) || !AreaValida(recorte)) {
    return;
  }

  const TCenaOpenGL cena(enquadramento, area, recorte);
  glBegin(GL_LINES);
  for (const TLoteAramado& lote : lotes) {
    glColor3fv(lote.cor);
    DesenhaArestas(*lote.arestas);
  }
  glEnd();
}

//----------------------------------------------------------------------------------------------

void TRenderizadorAramado::DesenhaSolido(
  const std::vector<TFace3D>& faces,
  const std::vector<TAresta3D>& arestas,
  const TCubo& dominio,
  int x,
  int y,
  int largura,
  int altura
) const
{
  const TRetanguloTela area = { x, y, largura, altura };
  if (!AreaValida(area)) {
    return;
  }

  const TCenaOpenGL cena(dominio, area, area);
  // Afasta as faces para que as arestas visíveis vençam o teste de profundidade.
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glBegin(GL_QUADS);
  for (const TFace3D& face : faces) {
    const float tom = TomFace(face.normal);
    glColor3f(0.20f * tom, 0.62f * tom, 0.78f * tom);
    for (const TCoordenada3D& vertice : face.vertices) {
      glVertex3d(vertice.x, vertice.y, vertice.z);
    }
  }
  glEnd();
  glDisable(GL_POLYGON_OFFSET_FILL);

  glColor3f(0.04f, 0.13f, 0.18f);
  glBegin(GL_LINES);
  DesenhaArestas(arestas);
  glEnd();
}

//----------------------------------------------------------------------------------------------
