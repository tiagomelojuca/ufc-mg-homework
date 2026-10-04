#ifndef MODELADOR_H_
#define MODELADOR_H_

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "Core/AramadoOctree.h"
#include "Core/Bloco.h"
#include "Core/Esfera.h"
#include "Core/SuperficieOctree.h"

//----------------------------------------------------------------------------------------------

class TModeloOctree
{
  public:
    TModeloOctree(
      int id,
      const std::string& nome,
      TOctree&& octree
    );

    int Id() const;
    const std::string& Nome() const;
    const TOctree& Octree() const;
    const std::vector<TAresta3D>& Arestas() const;
    const std::vector<TFace3D>& Faces() const;
    double Volume() const;

  private:
    int id;
    std::string nome;
    std::shared_ptr<const TOctree> octree;
    std::vector<TAresta3D> arestas;
    std::vector<TFace3D> faces;
    double volume;
};

//----------------------------------------------------------------------------------------------

class TModelador
{
  public:
    static constexpr int PROFUNDIDADE_MAXIMA_INTERATIVA = 8;

    const std::vector<TModeloOctree>& Modelos() const;
    const TModeloOctree* Selecionado() const;
    const TModeloOctree& Modelo(
      int id
    ) const;
    const TConfiguracaoOctree& Configuracao() const;

    void DefineProfundidade(
      int profundidade
    );
    void Seleciona(
      int id
    );
    void Remove(
      int id
    );
    int CriaBloco(
      const TBloco& bloco,
      const std::string& nome = ""
    );
    int CriaEsfera(
      const TEsfera& esfera,
      const std::string& nome = ""
    );
    int Une(
      int primeira,
      int segunda
    );
    int Escala(
      int id,
      double fator
    );
    int Abre(
      const std::filesystem::path& arquivo
    );
    void Salva(
      int id,
      const std::filesystem::path& arquivo,
      bool substituir = false
    ) const;

  private:
    int Adiciona(
      TOctree&& octree,
      const std::string& nome
    );

    TConfiguracaoOctree configuracao;
    std::vector<TModeloOctree> modelos;
    int proximoId = 1;
    int selecionado = 0;
};

//----------------------------------------------------------------------------------------------

#endif
