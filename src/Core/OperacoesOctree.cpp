#include "OperacoesOctree.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace
{
  struct TIntervalo
  {
    double minimo;
    double maximo;
  };

  TIntervalo CriaIntervalo(
    double centro,
    double lado
  )
  {
    const double meioLado = lado / 2.0;
    return { centro - meioLado, centro + meioLado };
  }

  bool CubosIguais(
    const TCubo& primeiro,
    const TCubo& segundo
  )
  {
    return
      primeiro.Centro().x == segundo.Centro().x &&
      primeiro.Centro().y == segundo.Centro().y &&
      primeiro.Centro().z == segundo.Centro().z &&
      primeiro.Lado() == segundo.Lado();
  }

  bool EstaDentro(
    const TIntervalo& interno,
    const TIntervalo& externo
  )
  {
    return interno.minimo >= externo.minimo && interno.maximo <= externo.maximo;
  }

  bool EstaFora(
    const TIntervalo& primeiro,
    const TIntervalo& segundo
  )
  {
    return
      primeiro.maximo <= segundo.minimo ||
      primeiro.minimo >= segundo.maximo;
  }

  bool CuboContido(
    const TCubo& cubo,
    const TCubo& dominio
  )
  {
    return
      EstaDentro(
        CriaIntervalo(cubo.Centro().x, cubo.Lado()),
        CriaIntervalo(dominio.Centro().x, dominio.Lado())
      ) &&
      EstaDentro(
        CriaIntervalo(cubo.Centro().y, cubo.Lado()),
        CriaIntervalo(dominio.Centro().y, dominio.Lado())
      ) &&
      EstaDentro(
        CriaIntervalo(cubo.Centro().z, cubo.Lado()),
        CriaIntervalo(dominio.Centro().z, dominio.Lado())
      );
  }

  EEstadoNoOctree ClassificaCubo(
    const TCubo& regiao,
    const TCubo& cubo
  )
  {
    const TIntervalo regiaoX = CriaIntervalo(regiao.Centro().x, regiao.Lado());
    const TIntervalo regiaoY = CriaIntervalo(regiao.Centro().y, regiao.Lado());
    const TIntervalo regiaoZ = CriaIntervalo(regiao.Centro().z, regiao.Lado());
    const TIntervalo cuboX = CriaIntervalo(cubo.Centro().x, cubo.Lado());
    const TIntervalo cuboY = CriaIntervalo(cubo.Centro().y, cubo.Lado());
    const TIntervalo cuboZ = CriaIntervalo(cubo.Centro().z, cubo.Lado());

    if (
      EstaFora(regiaoX, cuboX) ||
      EstaFora(regiaoY, cuboY) ||
      EstaFora(regiaoZ, cuboZ)
    ) {
      return EEstadoNoOctree::VAZIO;
    }

    if (
      EstaDentro(regiaoX, cuboX) &&
      EstaDentro(regiaoY, cuboY) &&
      EstaDentro(regiaoZ, cuboZ)
    ) {
      return EEstadoNoOctree::CHEIO;
    }

    return EEstadoNoOctree::PARCIAL;
  }

  void CompactaNo(
    TNoOctree& no
  );

  TNoOctree ClonaNo(
    const TNoOctree& origem
  )
  {
    TNoOctree copia(origem.Regiao(), origem.Estado());

    if (!origem.EhFolha()) {
      for (std::size_t indice = 0; indice < 8; ++indice) {
        copia.Filho(indice) = ClonaNo(origem.Filho(indice));
      }

      CompactaNo(copia);
    }

    return copia;
  }

  void CompactaNo(
    TNoOctree& no
  )
  {
    if (no.EhFolha()) {
      return;
    }

    const EEstadoNoOctree estado = no.Filho(0).Estado();
    if (estado == EEstadoNoOctree::PARCIAL) {
      return;
    }

    for (std::size_t indice = 1; indice < 8; ++indice) {
      if (
        no.Filho(indice).Estado() != estado ||
        !no.Filho(indice).EhFolha()
      ) {
        return;
      }
    }

    no.DefineEstado(estado);
  }

  TNoOctree UneNos(
    const TNoOctree& primeiro,
    const TNoOctree& segundo
  )
  {
    if (!CubosIguais(primeiro.Regiao(), segundo.Regiao())) {
      throw std::invalid_argument("Os nos devem representar a mesma regiao");
    }

    if (
      primeiro.Estado() == EEstadoNoOctree::CHEIO ||
      segundo.Estado() == EEstadoNoOctree::CHEIO
    ) {
      return TNoOctree(primeiro.Regiao(), EEstadoNoOctree::CHEIO);
    }

    if (primeiro.Estado() == EEstadoNoOctree::VAZIO) {
      return ClonaNo(segundo);
    }

    if (segundo.Estado() == EEstadoNoOctree::VAZIO) {
      return ClonaNo(primeiro);
    }

    TNoOctree resultado(primeiro.Regiao(), EEstadoNoOctree::PARCIAL);

    for (std::size_t indice = 0; indice < 8; ++indice) {
      resultado.Filho(indice) = UneNos(
        primeiro.Filho(indice),
        segundo.Filho(indice)
      );
    }

    CompactaNo(resultado);
    return resultado;
  }

  void PreparaSubdivisaoVazia(
    TNoOctree& no
  )
  {
    no.DefineEstado(EEstadoNoOctree::PARCIAL);

    for (std::size_t indice = 0; indice < 8; ++indice) {
      no.Filho(indice).DefineEstado(EEstadoNoOctree::VAZIO);
    }
  }

  void InsereCubo(
    TNoOctree& no,
    const TCubo& cubo,
    int profundidade
  )
  {
    if (no.Estado() == EEstadoNoOctree::CHEIO) {
      return;
    }

    const EEstadoNoOctree classificacao = ClassificaCubo(no.Regiao(), cubo);

    if (classificacao == EEstadoNoOctree::VAZIO) {
      return;
    }

    if (
      classificacao == EEstadoNoOctree::CHEIO ||
      profundidade == 1
    ) {
      no.DefineEstado(EEstadoNoOctree::CHEIO);
      return;
    }

    if (no.Estado() == EEstadoNoOctree::VAZIO) {
      PreparaSubdivisaoVazia(no);
    }

    for (std::size_t indice = 0; indice < 8; ++indice) {
      InsereCubo(no.Filho(indice), cubo, profundidade - 1);
    }

    CompactaNo(no);
  }

  TCubo EscalaCubo(
    const TCubo& cubo,
    double fator
  )
  {
    return TCubo(
      {
        cubo.Centro().x * fator,
        cubo.Centro().y * fator,
        cubo.Centro().z * fator
      },
      cubo.Lado() * fator
    );
  }

  void InsereFolhasEscaladas(
    const TNoOctree& origem,
    TNoOctree& destino,
    const TConfiguracaoOctree& configuracao,
    double fator
  )
  {
    if (origem.Estado() == EEstadoNoOctree::VAZIO) {
      return;
    }

    if (origem.Estado() == EEstadoNoOctree::CHEIO) {
      const TCubo cuboEscalado = EscalaCubo(origem.Regiao(), fator);

      if (!CuboContido(cuboEscalado, configuracao.Dominio())) {
        throw std::invalid_argument("A escala ultrapassa o dominio da octree");
      }

      InsereCubo(
        destino,
        cuboEscalado,
        configuracao.ProfundidadeMaxima()
      );
      return;
    }

    for (std::size_t indice = 0; indice < 8; ++indice) {
      InsereFolhasEscaladas(
        origem.Filho(indice),
        destino,
        configuracao,
        fator
      );
    }
  }
}

//----------------------------------------------------------------------------------------------

TOctree TOperacoesBooleanasOctree::Uniao(
  const TOctree& primeira,
  const TOctree& segunda
) const
{
  const TCubo& primeiroDominio = primeira.Configuracao().Dominio();
  const TCubo& segundoDominio = segunda.Configuracao().Dominio();

  if (!CubosIguais(primeiroDominio, segundoDominio)) {
    throw std::invalid_argument("As octrees devem possuir o mesmo dominio");
  }

  const TConfiguracaoOctree configuracao(
    primeiroDominio,
    std::max(
      primeira.Configuracao().ProfundidadeMaxima(),
      segunda.Configuracao().ProfundidadeMaxima()
    )
  );

  return TOctree(
    configuracao,
    UneNos(primeira.Raiz(), segunda.Raiz())
  );
}

//----------------------------------------------------------------------------------------------

TOctree TOperacoesGeometricasOctree::Escala(
  const TOctree& octree,
  double fator
) const
{
  if (!std::isfinite(fator) || fator <= 0.0) {
    throw std::invalid_argument("O fator de escala deve ser positivo");
  }

  const TConfiguracaoOctree& configuracao = octree.Configuracao();
  TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::VAZIO);

  InsereFolhasEscaladas(
    octree.Raiz(),
    raiz,
    configuracao,
    fator
  );

  return TOctree(configuracao, std::move(raiz));
}

//----------------------------------------------------------------------------------------------
