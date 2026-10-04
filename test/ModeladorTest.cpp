#include <gtest/gtest.h>

#include <chrono>
#include <fstream>
#include <limits>
#include <stdexcept>

#include "Application/Modelador.h"
#include "Persistence/PersistenciaDF.h"

namespace
{
  class TModeladorTest : public testing::Test
  {
    protected:
      void SetUp() override
      {
        const auto instante = std::chrono::steady_clock::now().time_since_epoch().count();
        diretorio = std::filesystem::temp_directory_path() / ("ufc-modelador-" + std::to_string(instante));
        std::filesystem::create_directory(diretorio);
        arquivo = diretorio / "modelo.df";
      }

      void TearDown() override
      {
        std::filesystem::remove_all(diretorio);
      }

      std::filesystem::path diretorio;
      std::filesystem::path arquivo;
      TModelador modelador;
  };
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_iniciar_sem_modelos_com_profundidade_cinco)
{
  EXPECT_TRUE(modelador.Modelos().empty());
  EXPECT_EQ(modelador.Selecionado(), nullptr);
  EXPECT_EQ(modelador.Configuracao().ProfundidadeMaxima(), 5);
  EXPECT_DOUBLE_EQ(modelador.Configuracao().Dominio().Lado(), 2.0);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_criar_selecionar_e_preparar_o_aramado_do_bloco)
{
  const int id = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0), "Peça");

  ASSERT_EQ(modelador.Modelos().size(), 1u);
  ASSERT_NE(modelador.Selecionado(), nullptr);
  EXPECT_EQ(modelador.Selecionado()->Id(), id);
  EXPECT_EQ(modelador.Selecionado()->Nome(), "Peça 1");
  EXPECT_DOUBLE_EQ(modelador.Selecionado()->Volume(), 1.0);
  EXPECT_EQ(modelador.Selecionado()->Arestas().size(), 12u);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_criar_e_selecionar_uma_esfera)
{
  const int id = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));

  ASSERT_NE(modelador.Selecionado(), nullptr);
  EXPECT_EQ(modelador.Selecionado()->Id(), id);
  EXPECT_GT(modelador.Selecionado()->Volume(), 0.0);
  EXPECT_LT(modelador.Selecionado()->Volume(), 8.0);
  EXPECT_FALSE(modelador.Selecionado()->Arestas().empty());
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_aplicar_a_profundidade_somente_a_novas_criacoes)
{
  const int primeiro = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));
  modelador.DefineProfundidade(3);
  const int segundo = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));

  EXPECT_EQ(modelador.Modelo(primeiro).Octree().Configuracao().ProfundidadeMaxima(), 5);
  EXPECT_EQ(modelador.Modelo(segundo).Octree().Configuracao().ProfundidadeMaxima(), 3);
  EXPECT_NE(modelador.Modelo(primeiro).Arestas().size(), modelador.Modelo(segundo).Arestas().size());
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_rejeitar_profundidade_fora_da_faixa_interativa)
{
  EXPECT_THROW(modelador.DefineProfundidade(0), std::invalid_argument);
  EXPECT_THROW(modelador.DefineProfundidade(9), std::invalid_argument);
  EXPECT_EQ(modelador.Configuracao().ProfundidadeMaxima(), 5);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_acrescentar_a_uniao_preservando_as_entradas)
{
  const int primeira = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  const int segunda = modelador.CriaBloco(TBloco({ 0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  const std::string entrada = TPersistenciaDF().Codifica(modelador.Modelo(primeira).Octree());
  const int resultado = modelador.Une(primeira, segunda);

  ASSERT_EQ(modelador.Modelos().size(), 3u);
  EXPECT_EQ(modelador.Selecionado()->Id(), resultado);
  EXPECT_DOUBLE_EQ(modelador.Modelo(resultado).Volume(), 2.0);
  EXPECT_DOUBLE_EQ(modelador.Modelo(segunda).Volume(), 1.0);
  EXPECT_EQ(TPersistenciaDF().Codifica(modelador.Modelo(primeira).Octree()), entrada);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_unir_modelos_de_profundidades_diferentes)
{
  modelador.DefineProfundidade(3);
  const int primeira = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  modelador.DefineProfundidade(5);
  const int segunda = modelador.CriaBloco(TBloco({ 0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  const int resultado = modelador.Une(primeira, segunda);

  EXPECT_EQ(modelador.Modelo(resultado).Octree().Configuracao().ProfundidadeMaxima(), 5);
  EXPECT_DOUBLE_EQ(modelador.Modelo(resultado).Volume(), 2.0);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_acrescentar_a_escala_preservando_o_original)
{
  modelador.DefineProfundidade(3);
  const int original = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  modelador.DefineProfundidade(1);
  const int resultado = modelador.Escala(original, 0.5);

  ASSERT_EQ(modelador.Modelos().size(), 2u);
  EXPECT_EQ(modelador.Selecionado()->Id(), resultado);
  EXPECT_DOUBLE_EQ(modelador.Modelo(original).Volume(), 1.0);
  EXPECT_DOUBLE_EQ(modelador.Modelo(resultado).Volume(), 0.125);
  EXPECT_EQ(modelador.Modelo(resultado).Arestas().size(), 12u);
  EXPECT_EQ(modelador.Modelo(resultado).Octree().Configuracao().ProfundidadeMaxima(), 3);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_preservar_estado_quando_uma_operacao_falha)
{
  const int id = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));

  EXPECT_THROW(modelador.CriaEsfera(TEsfera({ 1.0, 0.0, 0.0 }, 0.5)), std::invalid_argument);
  EXPECT_THROW(modelador.Escala(id, 3.0), std::invalid_argument);
  EXPECT_THROW(modelador.Une(id, 999), std::invalid_argument);
  EXPECT_THROW(modelador.Seleciona(999), std::invalid_argument);
  EXPECT_THROW(modelador.Remove(999), std::invalid_argument);
  EXPECT_EQ(modelador.Modelos().size(), 1u);
  EXPECT_EQ(modelador.Selecionado()->Id(), id);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_rejeitar_parametros_nao_finitos)
{
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const double infinito = std::numeric_limits<double>::infinity();

  EXPECT_THROW(modelador.CriaBloco(TBloco({ nan, 0.0, 0.0 }, 1.0, 1.0, 1.0)), std::invalid_argument);
  EXPECT_THROW(modelador.CriaBloco(TBloco({ 0.0, 0.0, 0.0 }, nan, 1.0, 1.0)), std::invalid_argument);
  EXPECT_THROW(modelador.CriaEsfera(TEsfera({ 0.0, infinito, 0.0 }, 0.5)), std::invalid_argument);
  EXPECT_THROW(modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, nan)), std::invalid_argument);
  EXPECT_TRUE(modelador.Modelos().empty());
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_manter_identificadores_estaveis_apos_remocao)
{
  const int primeiro = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));
  const int segundo = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.25));
  modelador.Seleciona(primeiro);
  modelador.Remove(segundo);
  EXPECT_EQ(modelador.Selecionado()->Id(), primeiro);
  const int terceiro = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.25));
  EXPECT_GT(terceiro, segundo);
  modelador.Remove(terceiro);
  EXPECT_EQ(modelador.Selecionado()->Id(), primeiro);
  modelador.Remove(primeiro);
  EXPECT_EQ(modelador.Selecionado(), nullptr);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_salvar_e_reabrir_somente_a_string_df)
{
  const int original = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));
  const std::string df = TPersistenciaDF().Codifica(modelador.Modelo(original).Octree());
  modelador.Salva(original, arquivo);
  std::ifstream fluxo(arquivo, std::ios::binary);
  const std::string conteudo { std::istreambuf_iterator<char>(fluxo), std::istreambuf_iterator<char>() };
  EXPECT_EQ(conteudo, df);
  const int reaberto = modelador.Abre(arquivo);

  ASSERT_EQ(modelador.Modelos().size(), 2u);
  EXPECT_EQ(modelador.Selecionado()->Id(), reaberto);
  EXPECT_EQ(TPersistenciaDF().Codifica(modelador.Modelo(reaberto).Octree()), df);
  EXPECT_DOUBLE_EQ(modelador.Modelo(reaberto).Volume(), modelador.Modelo(original).Volume());
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_preservar_estado_ao_abrir_arquivo_invalido_ou_profundo_demais)
{
  const int original = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));
  modelador.Salva(original, arquivo);
  modelador.DefineProfundidade(1);
  EXPECT_THROW(modelador.Abre(arquivo), std::invalid_argument);
  EXPECT_THROW(modelador.Abre(diretorio / "ausente.df"), std::runtime_error);
  EXPECT_THROW(modelador.Abre({}), std::invalid_argument);
  { std::ofstream fluxo(arquivo); fluxo << "ABC"; }
  EXPECT_THROW(modelador.Abre(arquivo), std::invalid_argument);
  EXPECT_EQ(modelador.Modelos().size(), 1u);
  EXPECT_EQ(modelador.Selecionado()->Id(), original);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_exigir_confirmacao_para_substituir_arquivo)
{
  const int primeiro = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  const int segundo = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));
  modelador.Salva(primeiro, arquivo);
  EXPECT_THROW(modelador.Salva(segundo, arquivo), std::invalid_argument);
  const TOctree preservada = TPersistenciaDF().Carrega(arquivo);
  EXPECT_EQ(TPersistenciaDF().Codifica(preservada), TPersistenciaDF().Codifica(modelador.Modelo(primeiro).Octree()));
  modelador.Salva(segundo, arquivo, true);
  const TOctree substituida = TPersistenciaDF().Carrega(arquivo);
  EXPECT_EQ(TPersistenciaDF().Codifica(substituida), TPersistenciaDF().Codifica(modelador.Modelo(segundo).Octree()));
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_preservar_estado_quando_salvar_falha)
{
  const int id = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));
  EXPECT_THROW(modelador.Salva(id, {}), std::invalid_argument);
  EXPECT_THROW(modelador.Salva(id, diretorio / "ausente" / "modelo.df"), std::runtime_error);
  EXPECT_EQ(modelador.Selecionado()->Id(), id);
  EXPECT_EQ(modelador.Modelos().size(), 1u);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_copiar_o_estado_da_estrategia_sem_mutar_modelos_compartilhados)
{
  const int id = modelador.CriaEsfera(TEsfera({ 0.0, 0.0, 0.0 }, 0.5));
  TModelador copia = modelador;
  copia.Escala(id, 0.5);
  copia.Remove(id);

  EXPECT_EQ(modelador.Modelos().size(), 1u);
  EXPECT_EQ(modelador.Selecionado()->Id(), id);
  EXPECT_EQ(copia.Modelos().size(), 1u);
  EXPECT_NE(copia.Selecionado()->Id(), id);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_criar_um_cilindro_e_calcular_sua_area)
{
  const int id = modelador.CriaCilindro(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, 1.0, EEixo::Z));

  ASSERT_NE(modelador.Selecionado(), nullptr);
  EXPECT_EQ(modelador.Selecionado()->Id(), id);
  EXPECT_EQ(modelador.Selecionado()->Nome(), "Cilindro 1");
  EXPECT_GT(modelador.Selecionado()->Volume(), 0.0);
  EXPECT_GT(modelador.Selecionado()->AreaSuperficial(), 0.0);
  EXPECT_THROW(modelador.CriaCilindro(TCilindro({ 0.0, 0.0, 0.0 }, 0.5, 3.0)), std::invalid_argument);
  EXPECT_EQ(modelador.Modelos().size(), 1u);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_calcular_a_area_superficial_ao_acrescentar_o_modelo)
{
  modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));

  EXPECT_DOUBLE_EQ(modelador.Selecionado()->AreaSuperficial(), 6.0);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_acrescentar_a_intersecao_preservando_as_entradas)
{
  const int primeiro = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  const int segundo = modelador.CriaBloco(TBloco({ 0.0, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  const int intersecao = modelador.Intersecta(primeiro, segundo);

  ASSERT_EQ(modelador.Modelos().size(), 3u);
  EXPECT_EQ(modelador.Selecionado()->Id(), intersecao);
  EXPECT_EQ(modelador.Selecionado()->Nome(), "Interseção 3");
  EXPECT_DOUBLE_EQ(modelador.Selecionado()->Volume(), 0.5);
  EXPECT_DOUBLE_EQ(modelador.Modelo(primeiro).Volume(), 1.0);
  EXPECT_DOUBLE_EQ(modelador.Modelo(segundo).Volume(), 1.0);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_acrescentar_a_translacao_preservando_o_original)
{
  const int original = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));
  const int transladado = modelador.Translada(original, { 1.0, 0.0, 0.0 });

  ASSERT_EQ(modelador.Modelos().size(), 2u);
  EXPECT_EQ(modelador.Selecionado()->Id(), transladado);
  EXPECT_EQ(modelador.Selecionado()->Nome(), "Translação 2");
  EXPECT_DOUBLE_EQ(modelador.Selecionado()->Volume(), 1.0);
  EXPECT_DOUBLE_EQ(modelador.Modelo(original).Octree().Raiz().Filho(0).Regiao().Centro().x, -0.5);
  EXPECT_EQ(modelador.Selecionado()->Octree().Raiz().Filho(1).Estado(), EEstadoNoOctree::CHEIO);
}

//----------------------------------------------------------------------------------------------

TEST_F(TModeladorTest, deve_preservar_estado_quando_a_translacao_falha)
{
  const int original = modelador.CriaBloco(TBloco({ -0.5, -0.5, -0.5 }, 1.0, 1.0, 1.0));

  EXPECT_THROW(modelador.Translada(original, { -1.0, 0.0, 0.0 }), std::invalid_argument);
  EXPECT_EQ(modelador.Modelos().size(), 1u);
  EXPECT_EQ(modelador.Selecionado()->Id(), original);
}

//----------------------------------------------------------------------------------------------
