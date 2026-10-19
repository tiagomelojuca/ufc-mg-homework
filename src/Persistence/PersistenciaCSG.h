#ifndef PERSISTENCIA_CSG_H_
#define PERSISTENCIA_CSG_H_

#include <filesystem>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "Core/ArvoreCSG.h"

//----------------------------------------------------------------------------------------------

// Formato textual da árvore, uma linha por nó:
//   No1; B; S 2,1,1; T 0,0,1
//   No2; E; S 0.5; T 1,0,0; R 0,0,90
//   No3; U No1 No2
// A raiz é o nó da última linha.
class TPersistenciaCSG
{
  public:
    std::string Codifica(
      const TArvoreCSG& arvore
    ) const;

    TArvoreCSG Decodifica(
      const std::string& entrada
    ) const;

    void Salva(
      const TArvoreCSG& arvore,
      const std::filesystem::path& arquivo
    ) const;

    TArvoreCSG Carrega(
      const std::filesystem::path& arquivo
    ) const;

  private:
    using TNomesDosNos = std::unordered_map<const TNoCSG*, std::string>;
    using TNosPorNome = std::map<std::string, TArvoreCSG>;

    std::string CodificaNo(
      const TNoCSG& no,
      TNomesDosNos& nomes,
      std::vector<std::string>& linhas
    ) const;

    void DecodificaLinha(
      const std::string& linha,
      TNosPorNome& nos,
      TArvoreCSG& ultimo
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
