#ifndef PRIMITIVA_H_
#define PRIMITIVA_H_

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

    // Prim_M: classificação da aresta pelas equações da primitiva.
    virtual TClassificacaoAresta ClassificaAresta(
      const TAresta& aresta
    ) const = 0;

  protected:
    static void ExigeDimensaoPositiva(
      double valor,
      const char* mensagem
    );

    // Converte a distância com sinal à fronteira (negativa dentro) em IN, ON ou OUT.
    static EClassificacao ClassificaPorDistancia(
      double distanciaComSinal
    );

    // Monta a classificação de uma aresta que está na primitiva apenas entre tEntrada e tSaida:
    // OUT antes, classificacaoInterna no intervalo e OUT depois, recortando em [0, 1].
    static TClassificacaoAresta ClassificaPorIntervalo(
      double tEntrada,
      double tSaida,
      EClassificacao classificacaoInterna
    );
};

//----------------------------------------------------------------------------------------------

#endif
