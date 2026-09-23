#include "PersistenciaDF.h"

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <utility>

//----------------------------------------------------------------------------------------------

std::string TPersistenciaDF::Codifica(
  const TOctree& octree
) const
{
  std::string saida;
  CodificaNo(octree.Raiz(), saida);
  return saida;
}

//----------------------------------------------------------------------------------------------

TOctree TPersistenciaDF::Decodifica(
  const std::string& entrada,
  const TConfiguracaoOctree& configuracao
) const
{
  if (entrada.empty()) {
    throw std::invalid_argument("A representacao DF nao pode ser vazia");
  }

  std::size_t posicao = 0;
  TNoOctree raiz(configuracao.Dominio());
  DecodificaNo(raiz, entrada, posicao, configuracao.ProfundidadeMaxima());

  if (posicao != entrada.size()) {
    throw std::invalid_argument("A representacao DF possui dados depois da raiz");
  }

  return TOctree(configuracao, std::move(raiz));
}

//----------------------------------------------------------------------------------------------

void TPersistenciaDF::Salva(
  const TOctree& octree,
  const std::filesystem::path& arquivo
) const
{
  std::ofstream fluxo(arquivo, std::ios::binary | std::ios::trunc);

  if (!fluxo.is_open()) {
    throw std::runtime_error("Nao foi possivel abrir o arquivo para escrita");
  }

  const std::string representacao = Codifica(octree);
  fluxo.write(representacao.data(), static_cast<std::streamsize>(representacao.size()));

  if (!fluxo) {
    throw std::runtime_error("Nao foi possivel escrever a representacao DF");
  }
}

//----------------------------------------------------------------------------------------------

TOctree TPersistenciaDF::Carrega(
  const std::filesystem::path& arquivo,
  const TConfiguracaoOctree& configuracao
) const
{
  std::ifstream fluxo(arquivo, std::ios::binary);

  if (!fluxo.is_open()) {
    throw std::runtime_error("Nao foi possivel abrir o arquivo para leitura");
  }

  const std::string entrada {
    std::istreambuf_iterator<char>(fluxo),
    std::istreambuf_iterator<char>()
  };

  if (fluxo.bad()) {
    throw std::runtime_error("Nao foi possivel ler a representacao DF");
  }

  return Decodifica(entrada, configuracao);
}

//----------------------------------------------------------------------------------------------

void TPersistenciaDF::CodificaNo(
  const TNoOctree& no,
  std::string& saida
) const
{
  saida.push_back(static_cast<char>(no.Estado()));

  if (no.EhFolha()) {
    return;
  }

  for (std::size_t indice = 0; indice < 8; ++indice) {
    CodificaNo(no.Filho(indice), saida);
  }
}

//----------------------------------------------------------------------------------------------

void TPersistenciaDF::DecodificaNo(
  TNoOctree& no,
  const std::string& entrada,
  std::size_t& posicao,
  int profundidade
) const
{
  if (posicao >= entrada.size()) {
    throw std::invalid_argument("A representacao DF esta incompleta");
  }

  const char estado = entrada[posicao++];

  if (estado == static_cast<char>(EEstadoNoOctree::CHEIO)) {
    no.DefineEstado(EEstadoNoOctree::CHEIO);
    return;
  }

  if (estado == static_cast<char>(EEstadoNoOctree::VAZIO)) {
    no.DefineEstado(EEstadoNoOctree::VAZIO);
    return;
  }

  if (estado != static_cast<char>(EEstadoNoOctree::PARCIAL)) {
    throw std::invalid_argument("A representacao DF possui um caractere desconhecido");
  }

  if (profundidade <= 1) {
    throw std::invalid_argument("A representacao DF excede a profundidade maxima");
  }

  no.DefineEstado(EEstadoNoOctree::PARCIAL);

  for (std::size_t indice = 0; indice < 8; ++indice) {
    DecodificaNo(no.Filho(indice), entrada, posicao, profundidade - 1);
  }
}

//----------------------------------------------------------------------------------------------
