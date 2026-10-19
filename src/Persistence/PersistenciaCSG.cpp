#include "PersistenciaCSG.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

#include "Core/Bloco.h"
#include "Core/Esfera.h"

namespace
{
  const char* const SEPARADOR_CAMPOS = "; ";

  std::string Apara(
    const std::string& texto
  )
  {
    const auto naoEspaco = [](unsigned char c) { return !std::isspace(c); };
    const auto inicio = std::find_if(texto.begin(), texto.end(), naoEspaco);
    const auto fim = std::find_if(texto.rbegin(), texto.rend(), naoEspaco).base();

    return inicio < fim ? std::string(inicio, fim) : std::string();
  }

  std::vector<std::string> Divide(
    const std::string& texto,
    char separador
  )
  {
    std::vector<std::string> partes;
    std::istringstream fluxo(texto);
    std::string parte;
    while (std::getline(fluxo, parte, separador)) {
      partes.push_back(Apara(parte));
    }

    return partes;
  }

  std::vector<std::string> DividePorEspacos(
    const std::string& texto
  )
  {
    std::vector<std::string> partes;
    std::istringstream fluxo(texto);
    std::string parte;
    while (fluxo >> parte) {
      partes.push_back(parte);
    }

    return partes;
  }

  // O ponto decimal independe da configuração regional da máquina.
  std::string FormataNumero(
    double valor
  )
  {
    std::ostringstream fluxo;
    fluxo.imbue(std::locale::classic());
    fluxo << std::setprecision(15) << valor;

    return fluxo.str();
  }

  double LeNumero(
    const std::string& texto
  )
  {
    std::istringstream fluxo(texto);
    fluxo.imbue(std::locale::classic());

    double valor = 0.0;
    fluxo >> valor;
    if (texto.empty() || fluxo.fail() || !fluxo.eof() || !std::isfinite(valor)) {
      throw std::invalid_argument("Valor numerico invalido: '" + texto + "'");
    }

    return valor;
  }

  std::string FormataVetor(
    const TVetor3D& vetor
  )
  {
    return FormataNumero(vetor.x) + "," + FormataNumero(vetor.y) + "," + FormataNumero(vetor.z);
  }

  // Lê um campo no formato "<código> v1,v2,...", exigindo a quantidade de valores indicada.
  std::vector<double> LeValoresDoCampo(
    const std::string& campo,
    std::size_t quantidade
  )
  {
    const std::vector<std::string> textos = Divide(Apara(campo.substr(1)), ',');
    if (textos.size() != quantidade) {
      throw std::invalid_argument(
        "O campo '" + campo + "' deve ter " + std::to_string(quantidade) + " valor(es)"
      );
    }

    std::vector<double> valores;
    for (const std::string& texto : textos) {
      valores.push_back(LeNumero(texto));
    }

    return valores;
  }

  std::string CodificaParametros(
    const TPrimitiva& primitiva
  )
  {
    if (const auto* bloco = dynamic_cast<const TBloco*>(&primitiva)) {
      return FormataVetor({ bloco->LadoX(), bloco->LadoY(), bloco->LadoZ() });
    }

    if (const auto* esfera = dynamic_cast<const TEsfera*>(&primitiva)) {
      return FormataNumero(esfera->Raio());
    }

    throw std::invalid_argument("Primitiva sem suporte no arquivo");
  }

  TArvoreCSG DecodificaPrimitiva(
    char codigo,
    const std::string& campoParametros
  )
  {
    if (campoParametros.empty() || campoParametros[0] != 'S') {
      throw std::invalid_argument("A primitiva deve ser seguida do campo S com seus parametros");
    }

    if (codigo == static_cast<char>(ETipoPrimitiva::BLOCO)) {
      const std::vector<double> lados = LeValoresDoCampo(campoParametros, 3);
      return CriaPrimitiva(std::make_shared<TBloco>(lados[0], lados[1], lados[2]));
    }

    if (codigo == static_cast<char>(ETipoPrimitiva::ESFERA)) {
      const std::vector<double> raio = LeValoresDoCampo(campoParametros, 1);
      return CriaPrimitiva(std::make_shared<TEsfera>(raio[0]));
    }

    throw std::invalid_argument(std::string("Primitiva sem suporte: ") + codigo);
  }

  bool EhCodigoPrimitiva(
    const std::string& texto
  )
  {
    return
      texto == std::string(1, static_cast<char>(ETipoPrimitiva::BLOCO)) ||
      texto == std::string(1, static_cast<char>(ETipoPrimitiva::ESFERA)) ||
      texto == std::string(1, static_cast<char>(ETipoPrimitiva::CILINDRO));
  }

  bool EhCodigoOperacao(
    const std::string& texto
  )
  {
    return
      texto == std::string(1, static_cast<char>(EOperacaoBooleana::UNIAO)) ||
      texto == std::string(1, static_cast<char>(EOperacaoBooleana::INTERSECAO)) ||
      texto == std::string(1, static_cast<char>(EOperacaoBooleana::DIFERENCA));
  }

  bool EhCodigoReservado(
    const std::string& texto
  )
  {
    return EhCodigoPrimitiva(texto) || EhCodigoOperacao(texto) ||
      texto == "S" || texto == "T" || texto == "R";
  }
}

//----------------------------------------------------------------------------------------------

std::string TPersistenciaCSG::Codifica(
  const TArvoreCSG& arvore
) const
{
  if (arvore == nullptr) {
    throw std::invalid_argument("A arvore nao pode ser vazia");
  }

  TNomesDosNos nomes;
  std::vector<std::string> linhas;
  CodificaNo(*arvore, nomes, linhas);

  std::string saida;
  for (const std::string& linha : linhas) {
    saida += linha + "\n";
  }

  return saida;
}

//----------------------------------------------------------------------------------------------

TArvoreCSG TPersistenciaCSG::Decodifica(
  const std::string& entrada
) const
{
  TNosPorNome nos;
  TArvoreCSG ultimo;

  std::istringstream fluxo(entrada);
  std::string linha;
  int numeroLinha = 0;
  while (std::getline(fluxo, linha)) {
    ++numeroLinha;
    if (Apara(linha).empty()) {
      continue;
    }

    try {
      DecodificaLinha(linha, nos, ultimo);
    } catch (const std::invalid_argument& erro) {
      throw std::invalid_argument("Linha " + std::to_string(numeroLinha) + ": " + erro.what());
    }
  }

  if (ultimo == nullptr) {
    throw std::invalid_argument("O arquivo nao contem nenhum no");
  }

  return ultimo;
}

//----------------------------------------------------------------------------------------------

void TPersistenciaCSG::Salva(
  const TArvoreCSG& arvore,
  const std::filesystem::path& arquivo
) const
{
  const std::string conteudo = Codifica(arvore);

  std::ofstream fluxo(arquivo, std::ios::binary | std::ios::trunc);
  if (!fluxo) {
    throw std::runtime_error("Nao foi possivel abrir o arquivo para escrita");
  }

  fluxo << conteudo;
  if (!fluxo) {
    throw std::runtime_error("Nao foi possivel escrever a arvore");
  }
}

//----------------------------------------------------------------------------------------------

TArvoreCSG TPersistenciaCSG::Carrega(
  const std::filesystem::path& arquivo
) const
{
  std::ifstream fluxo(arquivo, std::ios::binary);
  if (!fluxo) {
    throw std::runtime_error("Nao foi possivel abrir o arquivo para leitura");
  }

  std::ostringstream conteudo;
  conteudo << fluxo.rdbuf();
  if (fluxo.bad()) {
    throw std::runtime_error("Nao foi possivel ler a arvore");
  }

  return Decodifica(conteudo.str());
}

//----------------------------------------------------------------------------------------------

// Escreve primeiro os nós referenciados e devolve o nome atribuído ao nó. Uma sequência de
// movimentos rígidos fica na mesma linha da árvore que ela move, na ordem em que é aplicada.
std::string TPersistenciaCSG::CodificaNo(
  const TNoCSG& no,
  TNomesDosNos& nomes,
  std::vector<std::string>& linhas
) const
{
  const auto nomeExistente = nomes.find(&no);
  if (nomeExistente != nomes.end()) {
    return nomeExistente->second;
  }

  std::vector<const TMovimentoRigido*> movimentos;
  const TNoCSG* base = &no;
  while (base->Tipo() == ETipoNoCSG::MOVIMENTO_RIGIDO && (base == &no || nomes.count(base) == 0)) {
    movimentos.push_back(&base->Movimento());
    base = base->Filho().get();
  }
  std::reverse(movimentos.begin(), movimentos.end());

  std::string descricao;
  if (nomes.count(base) != 0) {
    descricao = nomes.at(base);
  } else if (base->Tipo() == ETipoNoCSG::PRIMITIVA) {
    const TPrimitiva& primitiva = base->Primitiva();
    descricao =
      std::string(1, static_cast<char>(primitiva.Tipo())) +
      SEPARADOR_CAMPOS + "S " + CodificaParametros(primitiva);
  } else {
    const std::string esquerda = CodificaNo(*base->Esquerda(), nomes, linhas);
    const std::string direita = CodificaNo(*base->Direita(), nomes, linhas);
    descricao = std::string(1, static_cast<char>(base->Operacao())) + " " + esquerda + " " + direita;
  }

  const std::string nome = "No" + std::to_string(linhas.size() + 1);
  std::string linha = nome + SEPARADOR_CAMPOS + descricao;
  for (const TMovimentoRigido* movimento : movimentos) {
    linha +=
      SEPARADOR_CAMPOS + std::string(1, static_cast<char>(movimento->Tipo())) +
      " " + FormataVetor(movimento->Valores());
  }

  linhas.push_back(linha);
  nomes[&no] = nome;

  return nome;
}

//----------------------------------------------------------------------------------------------

void TPersistenciaCSG::DecodificaLinha(
  const std::string& linha,
  TNosPorNome& nos,
  TArvoreCSG& ultimo
) const
{
  const std::vector<std::string> campos = Divide(linha, ';');
  if (campos.size() < 2) {
    throw std::invalid_argument("A linha deve ter ao menos o nome do no e sua descricao");
  }

  const std::string& nome = campos[0];
  if (nome.empty() || DividePorEspacos(nome).size() != 1 || EhCodigoReservado(nome)) {
    throw std::invalid_argument("Nome de no invalido: '" + nome + "'");
  }

  if (nos.count(nome) != 0) {
    throw std::invalid_argument("O no '" + nome + "' ja foi definido");
  }

  // O segundo campo descreve a árvore: uma primitiva, uma operação booleana ou um nó já definido.
  const std::vector<std::string> descricao = DividePorEspacos(campos[1]);
  TArvoreCSG arvore;
  std::size_t proximoCampo = 2;

  if (descricao.size() == 1 && EhCodigoPrimitiva(descricao[0])) {
    if (campos.size() < 3) {
      throw std::invalid_argument("A primitiva deve ser seguida do campo S com seus parametros");
    }

    arvore = DecodificaPrimitiva(descricao[0][0], campos[2]);
    proximoCampo = 3;
  } else if (descricao.size() == 3 && EhCodigoOperacao(descricao[0])) {
    for (std::size_t i = 1; i <= 2; ++i) {
      if (nos.count(descricao[i]) == 0) {
        throw std::invalid_argument("O no '" + descricao[i] + "' nao foi definido antes");
      }
    }

    arvore = CriaOperacaoBooleana(
      static_cast<EOperacaoBooleana>(descricao[0][0]),
      nos.at(descricao[1]),
      nos.at(descricao[2])
    );
  } else if (descricao.size() == 1 && nos.count(descricao[0]) != 0) {
    arvore = nos.at(descricao[0]);
  } else {
    throw std::invalid_argument("Descricao de no invalida: '" + campos[1] + "'");
  }

  for (std::size_t i = proximoCampo; i < campos.size(); ++i) {
    const std::string& campo = campos[i];
    const char codigo = campo.empty() ? '\0' : campo[0];

    if (codigo != static_cast<char>(ETipoMovimentoRigido::TRANSLACAO) &&
        codigo != static_cast<char>(ETipoMovimentoRigido::ROTACAO)) {
      throw std::invalid_argument("Campo invalido: '" + campo + "'");
    }

    const std::vector<double> valores = LeValoresDoCampo(campo, 3);
    arvore = CriaMovimentoRigido(
      TMovimentoRigido(static_cast<ETipoMovimentoRigido>(codigo), { valores[0], valores[1], valores[2] }),
      arvore
    );
  }

  nos[nome] = arvore;
  ultimo = arvore;
}

//----------------------------------------------------------------------------------------------
