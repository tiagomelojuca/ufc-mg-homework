#ifndef PERSISTENCIA_DF_H_
#define PERSISTENCIA_DF_H_

#include <cstddef>
#include <filesystem>
#include <string>

#include "Core/Octree.h"

//----------------------------------------------------------------------------------------------

class TPersistenciaDF
{
  public:
    std::string Codifica(
      const TOctree& octree
    ) const;

    TOctree Decodifica(
      const std::string& entrada,
      const TConfiguracaoOctree& configuracao = TConfiguracaoOctree()
    ) const;

    void Salva(
      const TOctree& octree,
      const std::filesystem::path& arquivo
    ) const;

    TOctree Carrega(
      const std::filesystem::path& arquivo,
      const TConfiguracaoOctree& configuracao = TConfiguracaoOctree()
    ) const;

  private:
    void CodificaNo(
      const TNoOctree& no,
      std::string& saida
    ) const;

    void DecodificaNo(
      TNoOctree& no,
      const std::string& entrada,
      std::size_t& posicao,
      int profundidade
    ) const;
};

//----------------------------------------------------------------------------------------------

#endif
