#ifndef BLOCO_H_
#define BLOCO_H_

#include "Primitiva.h"

//----------------------------------------------------------------------------------------------

class TBloco : public TPrimitiva
{
  public:
    TBloco(
      double ladoX,
      double ladoY,
      double ladoZ
    );

    double LadoX() const;
    double LadoY() const;
    double LadoZ() const;

    ETipoPrimitiva Tipo() const override;

    EClassificacao ClassificaPonto(
      const TVetor3D& ponto
    ) const override;

    TClassificacaoAresta ClassificaAresta(
      const TAresta& aresta
    ) const override;

    TVetor3D Normal(
      const TVetor3D& ponto
    ) const override;

    std::shared_ptr<const TPrimitiva> Escalada(
      const TVetor3D& fatores
    ) const override;

    TCaixa CaixaCanonica() const override;

    std::vector<TArestaCandidata> ArestasDoContorno() const override;

  private:
    double ladoX;
    double ladoY;
    double ladoZ;
};

//----------------------------------------------------------------------------------------------

#endif
