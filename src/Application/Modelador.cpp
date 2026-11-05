#include "Modelador.h"

#include <algorithm>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "Core/Bloco.h"
#include "Core/CaixaEnvolvente.h"
#include "Core/Esfera.h"
#include "Core/Octree.h"

namespace
{
  constexpr int PROFUNDIDADE_MINIMA = 1;
  constexpr int PROFUNDIDADE_MAXIMA = 8;

  //--------------------------------------------------------------------------------------------

  // Número curto para o nome do modelo, com ponto decimal independente da configuração regional.
  std::string FormataNumero(
    double valor
  )
  {
    std::ostringstream fluxo;
    fluxo.imbue(std::locale::classic());
    fluxo << valor;
    return fluxo.str();
  }

  //--------------------------------------------------------------------------------------------

  std::string FormataVetor(
    const TVetor3D& valores
  )
  {
    return
      "(" + FormataNumero(valores.x) + ", " + FormataNumero(valores.y) + ", " +
      FormataNumero(valores.z) + ")";
  }

  //--------------------------------------------------------------------------------------------

  std::string Referencia(
    TIdModelo id
  )
  {
    return "#" + std::to_string(id);
  }
}

//----------------------------------------------------------------------------------------------

TModelador::TModelador() :
  proximoId(1)
{
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::CriaBloco(
  double ladoX,
  double ladoY,
  double ladoZ
)
{
  return Adiciona(
    "Bloco " + FormataNumero(ladoX) + " x " + FormataNumero(ladoY) + " x " + FormataNumero(ladoZ),
    CriaPrimitiva(std::make_shared<TBloco>(ladoX, ladoY, ladoZ))
  );
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::CriaEsfera(
  double raio
)
{
  return Adiciona(
    "Esfera de raio " + FormataNumero(raio),
    CriaPrimitiva(std::make_shared<TEsfera>(raio))
  );
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::Une(
  TIdModelo esquerda,
  TIdModelo direita
)
{
  return Adiciona(
    "União de " + Referencia(esquerda) + " e " + Referencia(direita),
    CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, Modelo(esquerda).arvore, Modelo(direita).arvore)
  );
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::Translada(
  TIdModelo id,
  const TVetor3D& deslocamento
)
{
  return Adiciona(
    Referencia(id) + " transladado de " + FormataVetor(deslocamento),
    CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, deslocamento),
      Modelo(id).arvore
    )
  );
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::Rotaciona(
  TIdModelo id,
  const TVetor3D& angulosGraus
)
{
  return Adiciona(
    Referencia(id) + " rotacionado de " + FormataVetor(angulosGraus),
    CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, angulosGraus),
      Modelo(id).arvore
    )
  );
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::Escala(
  TIdModelo id,
  const TVetor3D& fatores
)
{
  return Adiciona(
    Referencia(id) + " escalado por " + FormataVetor(fatores),
    ::Escala(Modelo(id).arvore, fatores)
  );
}

//----------------------------------------------------------------------------------------------

void TModelador::Remove(
  TIdModelo id
)
{
  // As árvores são compartilhadas: os modelos feitos a partir deste continuam válidos.
  const auto modelo = std::find_if(modelos.begin(), modelos.end(), [id](const TModelo& candidato) {
    return candidato.id == id;
  });

  if (modelo == modelos.end()) {
    throw std::invalid_argument("Modelo inexistente: " + Referencia(id));
  }

  modelos.erase(modelo);
}

//----------------------------------------------------------------------------------------------

const std::vector<TModelo>& TModelador::Modelos() const
{
  return modelos;
}

//----------------------------------------------------------------------------------------------

bool TModelador::Existe(
  TIdModelo id
) const
{
  return std::any_of(modelos.begin(), modelos.end(), [id](const TModelo& modelo) {
    return modelo.id == id;
  });
}

//----------------------------------------------------------------------------------------------

const TModelo& TModelador::Modelo(
  TIdModelo id
) const
{
  for (const TModelo& modelo : modelos) {
    if (modelo.id == id) {
      return modelo;
    }
  }

  throw std::invalid_argument("Modelo inexistente: " + Referencia(id));
}

//----------------------------------------------------------------------------------------------

double TModelador::Volume(
  TIdModelo id,
  int profundidade
) const
{
  if (profundidade < PROFUNDIDADE_MINIMA || profundidade > PROFUNDIDADE_MAXIMA) {
    throw std::invalid_argument("A profundidade da octree deve estar entre 1 e 8");
  }

  const TArvoreCSG& arvore = Modelo(id).arvore;
  const TCaixa caixa = CaixaEnvolvente(arvore);

  TOctree octree(profundidade);
  octree.ConstruirDeCSG(arvore, caixa.minimo, caixa.maximo);

  return octree.CalcularVolume();
}

//----------------------------------------------------------------------------------------------

void TModelador::Salva(
  TIdModelo id,
  const std::filesystem::path& arquivo
) const
{
  persistencia.Salva(Modelo(id).arvore, arquivo);
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::Carrega(
  const std::filesystem::path& arquivo
)
{
  return Adiciona("Lido de " + arquivo.filename().string(), persistencia.Carrega(arquivo));
}

//----------------------------------------------------------------------------------------------

TIdModelo TModelador::Adiciona(
  const std::string& nome,
  TArvoreCSG arvore
)
{
  const TIdModelo id = proximoId++;
  modelos.push_back({ id, Referencia(id) + " " + nome, std::move(arvore) });
  return id;
}

//----------------------------------------------------------------------------------------------
