#include "Matriz4x4.h"

#include <cmath>

namespace
{
  constexpr double PI = 3.14159265358979323846;

  double EmRadianos(
    double anguloGraus
  )
  {
    return anguloGraus * PI / 180.0;
  }
}

//----------------------------------------------------------------------------------------------

TMatriz4x4::TMatriz4x4() :
  elementos {}
{
}

//----------------------------------------------------------------------------------------------

TMatriz4x4 TMatriz4x4::Identidade()
{
  TMatriz4x4 matriz;
  for (int i = 0; i < 4; ++i) {
    matriz.elementos[i][i] = 1.0;
  }

  return matriz;
}

//----------------------------------------------------------------------------------------------

TMatriz4x4 TMatriz4x4::Translacao(
  const TVetor3D& deslocamento
)
{
  TMatriz4x4 matriz = Identidade();
  matriz.elementos[0][3] = deslocamento.x;
  matriz.elementos[1][3] = deslocamento.y;
  matriz.elementos[2][3] = deslocamento.z;

  return matriz;
}

//----------------------------------------------------------------------------------------------

// As rotações seguem a regra da mão direita: ângulo positivo gira no sentido anti-horário
// quando visto do lado positivo do eixo.
TMatriz4x4 TMatriz4x4::RotacaoX(
  double anguloGraus
)
{
  const double c = std::cos(EmRadianos(anguloGraus));
  const double s = std::sin(EmRadianos(anguloGraus));

  TMatriz4x4 matriz = Identidade();
  matriz.elementos[1][1] = c;
  matriz.elementos[1][2] = -s;
  matriz.elementos[2][1] = s;
  matriz.elementos[2][2] = c;

  return matriz;
}

//----------------------------------------------------------------------------------------------

TMatriz4x4 TMatriz4x4::RotacaoY(
  double anguloGraus
)
{
  const double c = std::cos(EmRadianos(anguloGraus));
  const double s = std::sin(EmRadianos(anguloGraus));

  TMatriz4x4 matriz = Identidade();
  matriz.elementos[0][0] = c;
  matriz.elementos[0][2] = s;
  matriz.elementos[2][0] = -s;
  matriz.elementos[2][2] = c;

  return matriz;
}

//----------------------------------------------------------------------------------------------

TMatriz4x4 TMatriz4x4::RotacaoZ(
  double anguloGraus
)
{
  const double c = std::cos(EmRadianos(anguloGraus));
  const double s = std::sin(EmRadianos(anguloGraus));

  TMatriz4x4 matriz = Identidade();
  matriz.elementos[0][0] = c;
  matriz.elementos[0][1] = -s;
  matriz.elementos[1][0] = s;
  matriz.elementos[1][1] = c;

  return matriz;
}

//----------------------------------------------------------------------------------------------

double TMatriz4x4::Elemento(
  int linha,
  int coluna
) const
{
  return elementos.at(linha).at(coluna);
}

//----------------------------------------------------------------------------------------------

TMatriz4x4 TMatriz4x4::operator*(
  const TMatriz4x4& outra
) const
{
  TMatriz4x4 produto;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        produto.elementos[i][j] += elementos[i][k] * outra.elementos[k][j];
      }
    }
  }

  return produto;
}

//----------------------------------------------------------------------------------------------

TVetor3D TMatriz4x4::TransformaPonto(
  const TVetor3D& ponto
) const
{
  const double coordenadas[4] = { ponto.x, ponto.y, ponto.z, 1.0 };
  double resultado[4] = { 0.0, 0.0, 0.0, 0.0 };

  for (int i = 0; i < 4; ++i) {
    for (int k = 0; k < 4; ++k) {
      resultado[i] += elementos[i][k] * coordenadas[k];
    }
  }

  return { resultado[0], resultado[1], resultado[2] };
}
TMatriz4x4 TMatriz4x4::Escala(
  const TVetor3D& fatores
)
{
  TMatriz4x4 matriz = Identidade();
  matriz.elementos[0][0] = fatores.x;
  matriz.elementos[1][1] = fatores.y;
  matriz.elementos[2][2] = fatores.z;

  return matriz;
}
//----------------------------------------------------------------------------------------------
