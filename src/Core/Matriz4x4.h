#ifndef MATRIZ4X4_H_
#define MATRIZ4X4_H_

#include <array>

#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

// Matriz de transformação em coordenadas homogêneas, aplicada a pontos como colunas: p' = M p.
class TMatriz4x4
{
  public:
    TMatriz4x4();

    static TMatriz4x4 Identidade();

    static TMatriz4x4 Translacao(
      const TVetor3D& deslocamento
    );

    static TMatriz4x4 RotacaoX(
      double anguloGraus
    );

    static TMatriz4x4 RotacaoY(
      double anguloGraus
    );

    static TMatriz4x4 RotacaoZ(
      double anguloGraus
    );
    static TMatriz4x4 Escala(
        const TVetor3D& fatores
    );
    double Elemento(
      int linha,
      int coluna
    ) const;

    TMatriz4x4 operator*(
      const TMatriz4x4& outra
    ) const;

    TVetor3D TransformaPonto(
      const TVetor3D& ponto
    ) const;

  private:
    std::array<std::array<double, 4>, 4> elementos;
};

//----------------------------------------------------------------------------------------------

#endif
