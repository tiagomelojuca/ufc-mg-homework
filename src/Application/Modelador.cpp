#include "Modelador.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

#include "Core/AnaliseOctree.h"
#include "Core/OperacoesOctree.h"
#include "Persistence/PersistenciaDF.h"

namespace
{
  void ValidaCentro(
    const TCoordenada3D& centro
  )
  {
    if (!std::isfinite(centro.x) || !std::isfinite(centro.y) || !std::isfinite(centro.z)) {
      throw std::invalid_argument("As coordenadas devem ser finitas");
    }
  }
}

//----------------------------------------------------------------------------------------------

TModeloOctree::TModeloOctree(
  int id,
  const std::string& nome,
  TOctree&& octree
) :
  id(id),
  nome(nome),
  octree(std::make_shared<const TOctree>(std::move(octree))),
  arestas(TGeradorAramadoOctree().Gera(*this->octree)),
  faces(TGeradorSuperficieOctree().Gera(*this->octree)),
  volume(TAnaliseOctree().CalculaVolume(*this->octree)),
  areaSuperficial(TAnaliseOctree().CalculaAreaSuperficial(faces))
{
}

//----------------------------------------------------------------------------------------------

int TModeloOctree::Id() const
{
  return id;
}

const std::string& TModeloOctree::Nome() const
{
  return nome;
}

const TOctree& TModeloOctree::Octree() const
{
  return *octree;
}

const std::vector<TAresta3D>& TModeloOctree::Arestas() const
{
  return arestas;
}

const std::vector<TFace3D>& TModeloOctree::Faces() const
{
  return faces;
}

double TModeloOctree::Volume() const
{
  return volume;
}

double TModeloOctree::AreaSuperficial() const
{
  return areaSuperficial;
}

//----------------------------------------------------------------------------------------------

const std::vector<TModeloOctree>& TModelador::Modelos() const
{
  return modelos;
}

const TModeloOctree* TModelador::Selecionado() const
{
  return selecionado == 0 ? nullptr : &Modelo(selecionado);
}

const TModeloOctree& TModelador::Modelo(
  int id
) const
{
  const auto modelo = std::find_if(modelos.begin(), modelos.end(), [id](const TModeloOctree& item) {
    return item.Id() == id;
  });
  if (modelo == modelos.end()) {
    throw std::invalid_argument("Selecione um modelo existente");
  }
  return *modelo;
}

const TConfiguracaoOctree& TModelador::Configuracao() const
{
  return configuracao;
}

//----------------------------------------------------------------------------------------------

void TModelador::DefineProfundidade(
  int profundidade
)
{
  if (profundidade < 1 || profundidade > PROFUNDIDADE_MAXIMA_INTERATIVA) {
    throw std::invalid_argument("A profundidade interativa deve estar entre 1 e 8");
  }
  configuracao = TConfiguracaoOctree(configuracao.Dominio(), profundidade);
}

void TModelador::Seleciona(
  int id
)
{
  Modelo(id);
  selecionado = id;
}

void TModelador::Remove(
  int id
)
{
  Modelo(id);
  modelos.erase(std::remove_if(modelos.begin(), modelos.end(), [id](const TModeloOctree& item) {
    return item.Id() == id;
  }), modelos.end());

  if (selecionado == id) {
    selecionado = modelos.empty() ? 0 : modelos.back().Id();
  }
}

//----------------------------------------------------------------------------------------------

int TModelador::CriaBloco(
  const TBloco& bloco,
  const std::string& nome
)
{
  ValidaCentro(bloco.Centro());
  if (!std::isfinite(bloco.LadoX()) || !std::isfinite(bloco.LadoY()) || !std::isfinite(bloco.LadoZ())) {
    throw std::invalid_argument("Os lados do bloco devem ser finitos");
  }
  TOctree octree(configuracao);
  octree.Constroi(TClassificadorBlocoOctree(bloco));
  return Adiciona(std::move(octree), nome.empty() ? "Bloco" : nome);
}

int TModelador::CriaEsfera(
  const TEsfera& esfera,
  const std::string& nome
)
{
  ValidaCentro(esfera.Centro());
  if (!std::isfinite(esfera.Raio())) {
    throw std::invalid_argument("O raio da esfera deve ser finito");
  }
  TOctree octree(configuracao);
  octree.Constroi(TClassificadorEsferaOctree(esfera));
  return Adiciona(std::move(octree), nome.empty() ? "Esfera" : nome);
}

int TModelador::CriaCilindro(
  const TCilindro& cilindro,
  const std::string& nome
)
{
  ValidaCentro(cilindro.Centro());
  if (!std::isfinite(cilindro.Raio()) || !std::isfinite(cilindro.Altura())) {
    throw std::invalid_argument("O raio e a altura do cilindro devem ser finitos");
  }
  TOctree octree(configuracao);
  octree.Constroi(TClassificadorCilindroOctree(cilindro));
  return Adiciona(std::move(octree), nome.empty() ? "Cilindro" : nome);
}

int TModelador::Une(
  int primeira,
  int segunda
)
{
  TOctree octree = TOperacoesBooleanasOctree().Uniao(Modelo(primeira).Octree(), Modelo(segunda).Octree());
  return Adiciona(std::move(octree), "União");
}

int TModelador::Intersecta(
  int primeira,
  int segunda
)
{
  TOctree octree = TOperacoesBooleanasOctree().Intersecao(Modelo(primeira).Octree(), Modelo(segunda).Octree());
  return Adiciona(std::move(octree), "Interseção");
}

int TModelador::Escala(
  int id,
  double fator
)
{
  TOctree octree = TOperacoesGeometricasOctree().Escala(Modelo(id).Octree(), fator);
  return Adiciona(std::move(octree), "Escala");
}

int TModelador::Translada(
  int id,
  const TCoordenada3D& deslocamento
)
{
  TOctree octree = TOperacoesGeometricasOctree().Translada(Modelo(id).Octree(), deslocamento);
  return Adiciona(std::move(octree), "Translação");
}

//----------------------------------------------------------------------------------------------

int TModelador::Abre(
  const std::filesystem::path& arquivo
)
{
  if (arquivo.empty()) {
    throw std::invalid_argument("Informe o caminho do arquivo");
  }
  TOctree octree = TPersistenciaDF().Carrega(arquivo, configuracao);
  return Adiciona(std::move(octree), arquivo.filename().u8string());
}

void TModelador::Salva(
  int id,
  const std::filesystem::path& arquivo,
  bool substituir
) const
{
  const TModeloOctree& modelo = Modelo(id);
  if (arquivo.empty()) {
    throw std::invalid_argument("Informe o caminho do arquivo");
  }
  if (!substituir && std::filesystem::exists(arquivo)) {
    throw std::invalid_argument("O arquivo já existe. Confirme a substituição");
  }
  TPersistenciaDF().Salva(modelo.Octree(), arquivo);
}

int TModelador::Adiciona(
  TOctree&& octree,
  const std::string& nome
)
{
  modelos.emplace_back(proximoId, nome + " " + std::to_string(proximoId), std::move(octree));
  selecionado = proximoId++;
  return selecionado;
}

//----------------------------------------------------------------------------------------------
