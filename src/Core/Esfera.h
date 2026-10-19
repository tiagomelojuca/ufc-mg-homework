#ifndef ESFERA_H_
#define ESFERA_H_

#include "Primitiva.h"

//----------------------------------------------------------------------------------------------

class TEsfera : public TPrimitiva
{
  public:
    explicit TEsfera(
      double raio
    );

    double Raio() const;

    ETipoPrimitiva Tipo() const override;

    EClassificacao ClassificaPonto(
      const TVetor3D& ponto
    ) const override;

    TClassificacaoAresta ClassificaAresta(
      const TAresta& aresta
    ) const override;

  private:
    double raio;
};

//----------------------------------------------------------------------------------------------

#endif
