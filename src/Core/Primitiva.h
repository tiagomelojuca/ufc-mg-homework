#ifndef PRIMITIVA_H_
#define PRIMITIVA_H_

#include <vector>

#include "Classificacao.h"
#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

// Os valores são os códigos usados no arquivo.
enum class ETipoPrimitiva : char
{
  BLOCO = 'B',
  ESFERA = 'E',
  CILINDRO = 'C'
};

//----------------------------------------------------------------------------------------------

class TPrimitiva
{
  public:
    virtual ~TPrimitiva() = default;

    virtual ETipoPrimitiva Tipo() const = 0;

    virtual EClassificacao ClassificaPonto(
      const TVetor3D& ponto
    ) const = 0;

    // Valores de t em que a reta da aresta cruza a fronteira, obtidos pelas equações da primitiva.
    virtual std::vector<double> IntersecoesComFronteira(
      const TAresta& aresta
    ) const = 0;

    TClassificacaoAresta ClassificaAresta(
      const TAresta& aresta
    ) const;

  protected:
    static void ExigeDimensaoPositiva(
      double valor,
      const char* mensagem
    );

    // Converte a distância com sinal à fronteira (negativa dentro) em IN, ON ou OUT.
    static EClassificacao ClassificaPorDistancia(
      double distanciaComSinal
    );
};

//----------------------------------------------------------------------------------------------

#endif
