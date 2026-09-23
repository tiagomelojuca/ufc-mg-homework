#ifndef OCTREE_H_
#define OCTREE_H_

#include <array>
#include <cstddef>
#include <memory>

//----------------------------------------------------------------------------------------------

struct TCoordenada3D
{
  double x;
  double y;
  double z;
};

//----------------------------------------------------------------------------------------------

class TCubo
{
  public:
    TCubo(
      const TCoordenada3D& centro,
      double lado
    );

    const TCoordenada3D& Centro() const;
    double Lado() const;

    TCubo Octante(
      std::size_t indice
    ) const;

  private:
    TCoordenada3D centro;
    double lado;
};

//----------------------------------------------------------------------------------------------

enum class EEstadoNoOctree : char
{
  CHEIO = 'B',
  VAZIO = 'W',
  PARCIAL = '('
};

//----------------------------------------------------------------------------------------------

class TNoOctree
{
  public:
    explicit TNoOctree(
      const TCubo& regiao,
      EEstadoNoOctree estado = EEstadoNoOctree::CHEIO
    );

    TNoOctree(
      const TNoOctree&
    ) = delete;

    TNoOctree& operator=(
      const TNoOctree&
    ) = delete;

    TNoOctree(
      TNoOctree&&
    ) noexcept = default;

    TNoOctree& operator=(
      TNoOctree&&
    ) noexcept = default;

    EEstadoNoOctree Estado() const;
    const TCubo& Regiao() const;
    bool EhFolha() const;

    const TNoOctree& Filho(
      std::size_t indice
    ) const;

    TNoOctree& Filho(
      std::size_t indice
    );

    void DefineEstado(
      EEstadoNoOctree novoEstado
    );

  private:
    void Subdivide();

    EEstadoNoOctree estado;
    TCubo regiao;
    std::array<std::unique_ptr<TNoOctree>, 8> filhos;
};

//----------------------------------------------------------------------------------------------

class TClassificadorOctree
{
  public:
    virtual ~TClassificadorOctree() = default;

    virtual EEstadoNoOctree Classifica(
      const TCubo& regiao
    ) const = 0;

    virtual bool EstaContido(
      const TCubo& dominio
    ) const = 0;
};

//----------------------------------------------------------------------------------------------

class TConfiguracaoOctree
{
  public:
    TConfiguracaoOctree();

    TConfiguracaoOctree(
      const TCubo& dominio,
      int profundidadeMaxima
    );

    const TCubo& Dominio() const;
    int ProfundidadeMaxima() const;

  private:
    TCubo dominio;
    int profundidadeMaxima;
};

//----------------------------------------------------------------------------------------------

class TOctree
{
  public:
    explicit TOctree(
      const TConfiguracaoOctree& configuracao = TConfiguracaoOctree()
    );

    TOctree(
      const TConfiguracaoOctree& configuracao,
      TNoOctree&& raiz
    );

    TOctree(
      const TOctree&
    ) = delete;

    TOctree& operator=(
      const TOctree&
    ) = delete;

    TOctree(
      TOctree&&
    ) noexcept = default;

    TOctree& operator=(
      TOctree&&
    ) noexcept = default;

    const TConfiguracaoOctree& Configuracao() const;
    const TNoOctree& Raiz() const;

    void Constroi(
      const TClassificadorOctree& classificador
    );

  private:
    void ConstroiNo(
      TNoOctree& no,
      const TClassificadorOctree& classificador,
      int profundidade
    );

    TConfiguracaoOctree configuracao;
    TNoOctree raiz;
};

//----------------------------------------------------------------------------------------------

#endif
