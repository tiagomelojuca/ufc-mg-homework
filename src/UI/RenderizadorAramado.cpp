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
  if (largura <= 0 || altura <= 0) {
    return;
  }

  GLint modoMatriz;
  glGetIntegerv(GL_MATRIX_MODE, &modoMatriz);
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glViewport(x, y, largura, altura);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glLineWidth(1.0f);

  const double aspecto = static_cast<double>(largura) / altura;
  const double alcance = dominio.Lado();
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
  glTranslated(-dominio.Centro().x, -dominio.Centro().y, -dominio.Centro().z);

  glColor3f(0.35f, 0.8f, 1.0f);
  glBegin(GL_LINES);
  for (const TAresta3D& aresta : arestas) {
    glVertex3d(aresta.inicio.x, aresta.inicio.y, aresta.inicio.z);
    glVertex3d(aresta.fim.x, aresta.fim.y, aresta.fim.z);
  }
  glEnd();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(modoMatriz);
  glPopAttrib();
}

//----------------------------------------------------------------------------------------------
