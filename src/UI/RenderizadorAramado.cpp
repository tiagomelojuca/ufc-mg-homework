#include "RenderizadorAramado.h"

#include <GLFW/glfw3.h>

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
  if (area.largura <= 0 || area.altura <= 0 || recorte.largura <= 0 || recorte.altura <= 0) {
    return;
  }

  GLint modoMatriz;
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

  glBegin(GL_LINES);
  for (const TLoteAramado& lote : lotes) {
    glColor3fv(lote.cor);
    for (const TAresta3D& aresta : *lote.arestas) {
      glVertex3d(aresta.inicio.x, aresta.inicio.y, aresta.inicio.z);
      glVertex3d(aresta.fim.x, aresta.fim.y, aresta.fim.z);
    }
  }
  glEnd();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(modoMatriz);
  glPopAttrib();
}

//----------------------------------------------------------------------------------------------
