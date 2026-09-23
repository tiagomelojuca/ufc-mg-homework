#include <gtest/gtest.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include "Persistence/PersistenciaDF.h"

namespace
{
  constexpr const char* REPRESENTACAO_PROFESSOR = "(BWWBBW(BWWBBBWWB";

  class TArquivoTemporario
  {
    public:
      explicit TArquivoTemporario(
        const std::string& nome
      ) :
        caminho(std::filesystem::temp_directory_path() / nome)
      {
        std::error_code erro;
        std::filesystem::remove(caminho, erro);
      }

      ~TArquivoTemporario()
      {
        std::error_code erro;
        std::filesystem::remove(caminho, erro);
      }

      const std::filesystem::path& Caminho() const
      {
        return caminho;
      }

    private:
      std::filesystem::path caminho;
  };

  TOctree CriaOctreeDoExemploDoProfessor()
  {
    const TConfiguracaoOctree configuracao(
      TCubo({ 0.0, 0.0, 0.0 }, 2.0),
      3
    );
    TNoOctree raiz(configuracao.Dominio(), EEstadoNoOctree::PARCIAL);

    const std::array<EEstadoNoOctree, 8> estadosRaiz = {
      EEstadoNoOctree::CHEIO,
      EEstadoNoOctree::VAZIO,
      EEstadoNoOctree::VAZIO,
      EEstadoNoOctree::CHEIO,
      EEstadoNoOctree::CHEIO,
      EEstadoNoOctree::VAZIO,
      EEstadoNoOctree::PARCIAL,
      EEstadoNoOctree::CHEIO
    };

    for (std::size_t indice = 0; indice < estadosRaiz.size(); ++indice) {
      raiz.Filho(indice).DefineEstado(estadosRaiz[indice]);
    }

    const std::array<EEstadoNoOctree, 8> estadosFilhoParcial = {
      EEstadoNoOctree::CHEIO,
      EEstadoNoOctree::VAZIO,
      EEstadoNoOctree::VAZIO,
      EEstadoNoOctree::CHEIO,
      EEstadoNoOctree::CHEIO,
      EEstadoNoOctree::CHEIO,
      EEstadoNoOctree::VAZIO,
      EEstadoNoOctree::VAZIO
    };

    for (std::size_t indice = 0; indice < estadosFilhoParcial.size(); ++indice) {
      raiz.Filho(6).Filho(indice).DefineEstado(estadosFilhoParcial[indice]);
    }

    return TOctree(configuracao, std::move(raiz));
  }
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_codificar_folhas_cheias_e_vazias)
{
  const TPersistenciaDF persistencia;
  const TConfiguracaoOctree configuracao;
  TOctree cheia(configuracao, TNoOctree(configuracao.Dominio(), EEstadoNoOctree::CHEIO));
  TOctree vazia(configuracao, TNoOctree(configuracao.Dominio(), EEstadoNoOctree::VAZIO));

  EXPECT_EQ(persistencia.Codifica(cheia), "B");
  EXPECT_EQ(persistencia.Codifica(vazia), "W");
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_codificar_o_exemplo_apresentado_pelo_professor)
{
  const TPersistenciaDF persistencia;
  const TOctree octree = CriaOctreeDoExemploDoProfessor();

  EXPECT_EQ(persistencia.Codifica(octree), REPRESENTACAO_PROFESSOR);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_decodificar_o_exemplo_apresentado_pelo_professor)
{
  const TPersistenciaDF persistencia;
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 3);

  const TOctree octree = persistencia.Decodifica(REPRESENTACAO_PROFESSOR, configuracao);

  ASSERT_EQ(octree.Raiz().Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(octree.Raiz().Filho(0).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(octree.Raiz().Filho(1).Estado(), EEstadoNoOctree::VAZIO);
  ASSERT_EQ(octree.Raiz().Filho(6).Estado(), EEstadoNoOctree::PARCIAL);
  EXPECT_EQ(octree.Raiz().Filho(6).Filho(5).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(octree.Raiz().Filho(7).Estado(), EEstadoNoOctree::CHEIO);
  EXPECT_EQ(persistencia.Codifica(octree), REPRESENTACAO_PROFESSOR);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_rejeitar_entrada_vazia_ou_caractere_desconhecido)
{
  const TPersistenciaDF persistencia;

  EXPECT_THROW(persistencia.Decodifica(""), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("X"), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_rejeitar_uma_representacao_incompleta)
{
  const TPersistenciaDF persistencia;

  EXPECT_THROW(persistencia.Decodifica("(BWW"), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_rejeitar_dados_depois_da_raiz)
{
  const TPersistenciaDF persistencia;

  EXPECT_THROW(persistencia.Decodifica("BW"), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_rejeitar_arvore_acima_da_profundidade_configurada)
{
  const TPersistenciaDF persistencia;
  const TConfiguracaoOctree configuracao(TCubo({ 0.0, 0.0, 0.0 }, 2.0), 1);

  EXPECT_THROW(
    persistencia.Decodifica("(BBBBBBBB", configuracao),
    std::invalid_argument
  );
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_rejeitar_espacos_e_quebras_de_linha)
{
  const TPersistenciaDF persistencia;

  EXPECT_THROW(persistencia.Decodifica(" B"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("B\n"), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_salvar_e_carregar_sem_alterar_a_representacao)
{
  const TPersistenciaDF persistencia;
  const TOctree original = CriaOctreeDoExemploDoProfessor();
  const TArquivoTemporario arquivo("ufc-mg-homework-persistencia-df.txt");

  persistencia.Salva(original, arquivo.Caminho());

  std::ifstream fluxo(arquivo.Caminho(), std::ios::binary);
  const std::string conteudo {
    std::istreambuf_iterator<char>(fluxo),
    std::istreambuf_iterator<char>()
  };
  const TOctree carregada = persistencia.Carrega(
    arquivo.Caminho(),
    original.Configuracao()
  );

  EXPECT_EQ(conteudo, REPRESENTACAO_PROFESSOR);
  EXPECT_EQ(persistencia.Codifica(carregada), REPRESENTACAO_PROFESSOR);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaDFTest, deve_informar_erro_ao_carregar_arquivo_inexistente)
{
  const TPersistenciaDF persistencia;
  const TArquivoTemporario arquivo("ufc-mg-homework-inexistente.df");

  EXPECT_THROW(persistencia.Carrega(arquivo.Caminho()), std::runtime_error);
}

//----------------------------------------------------------------------------------------------
