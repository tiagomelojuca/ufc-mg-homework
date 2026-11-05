#ifndef MODELADOR_H_
#define MODELADOR_H_

#include <filesystem>
#include <string>
#include <vector>

#include "Core/ArvoreCSG.h"
#include "Core/Vetor3D.h"
#include "Persistence/PersistenciaCSG.h"

//----------------------------------------------------------------------------------------------

// Identificador estável: não muda nem é reaproveitado quando outros modelos são removidos.
using TIdModelo = int;

//----------------------------------------------------------------------------------------------

struct TModelo
{
  TIdModelo id;
  std::string nome;
  TArvoreCSG arvore;
};

//----------------------------------------------------------------------------------------------

// Casos de uso do modelador sobre a lista de modelos. Cada operação acrescenta um novo modelo com
// o resultado e devolve o seu identificador; os modelos de entrada não mudam.
class TModelador
{
  public:
    TModelador();

    TIdModelo CriaBloco(
      double ladoX,
      double ladoY,
      double ladoZ
    );

    TIdModelo CriaEsfera(
      double raio
    );

    TIdModelo Une(
      TIdModelo esquerda,
      TIdModelo direita
    );

    TIdModelo Translada(
      TIdModelo id,
      const TVetor3D& deslocamento
    );

    // Ângulos em graus, com R = Rz · Ry · Rx.
    TIdModelo Rotaciona(
      TIdModelo id,
      const TVetor3D& angulosGraus
    );

    // Altera os parâmetros da primitiva do modelo; não se aplica a operações booleanas.
    TIdModelo Escala(
      TIdModelo id,
      const TVetor3D& fatores
    );

    void Remove(
      TIdModelo id
    );

    const std::vector<TModelo>& Modelos() const;

    bool Existe(
      TIdModelo id
    ) const;

    const TModelo& Modelo(
      TIdModelo id
    ) const;

    // Volume pela octree gerada da árvore, dentro da caixa envolvente do modelo.
    double Volume(
      TIdModelo id,
      int profundidade
    ) const;

    void Salva(
      TIdModelo id,
      const std::filesystem::path& arquivo
    ) const;

    TIdModelo Carrega(
      const std::filesystem::path& arquivo
    );

  private:
    TIdModelo Adiciona(
      const std::string& nome,
      TArvoreCSG arvore
    );

    std::vector<TModelo> modelos;
    TIdModelo proximoId;
    TPersistenciaCSG persistencia;
};

//----------------------------------------------------------------------------------------------

#endif
