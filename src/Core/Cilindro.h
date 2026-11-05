#ifndef CILINDRO_H_
#define CILINDRO_H_

#include "Primitiva.h"

//----------------------------------------------------------------------------------------------

// Cilindro de eixo z, com base em z = 0 e topo em z = altura: C = H1 ∩ H2 ∩ H3, com
// H1: x² + y² - r² <= 0, H2: z >= 0 e H3: z - h <= 0.
class TCilindro : public TPrimitiva
{
  public:
    TCilindro(
      double raio,
      double altura
    );

    double Raio() const;
    double Altura() const;

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

    // A escala em x e y deve ser a mesma, para que a seção continue circular.
    std::shared_ptr<const TPrimitiva> Escalada(
      const TVetor3D& fatores
    ) const override;

    TCaixa CaixaCanonica() const override;

    std::vector<TArestaCandidata> ArestasDoContorno() const override;

  private:
    double raio;
    double altura;
};

//----------------------------------------------------------------------------------------------

#endif
