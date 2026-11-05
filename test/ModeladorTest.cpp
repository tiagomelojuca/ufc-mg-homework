#include <gtest/gtest.h>

#include <cmath>
#include <filesystem>
#include <stdexcept>

#include "Application/Modelador.h"
#include "Core/Bloco.h"
#include "Core/ClassificadorCSG.h"
#include "Core/Esfera.h"

namespace
{
  constexpr double PI = 3.14159265358979323846;
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_criar_primitivas_com_identificadores_crescentes)
{
  TModelador modelador;

  const TIdModelo bloco = modelador.CriaBloco(2.0, 1.0, 1.0);
  const TIdModelo esfera = modelador.CriaEsfera(0.5);

  EXPECT_EQ(bloco, 1);
  EXPECT_EQ(esfera, 2);
  ASSERT_EQ(modelador.Modelos().size(), 2u);
  EXPECT_EQ(modelador.Modelo(bloco).nome, "#1 Bloco 2 x 1 x 1");
  EXPECT_EQ(modelador.Modelo(esfera).nome, "#2 Esfera de raio 0.5");
  EXPECT_EQ(modelador.Modelo(esfera).arvore->Primitiva().Tipo(), ETipoPrimitiva::ESFERA);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_unir_em_um_novo_modelo_sem_alterar_as_entradas)
{
  TModelador modelador;
  const TIdModelo bloco = modelador.CriaBloco(2.0, 2.0, 2.0);
  const TIdModelo esfera = modelador.Translada(modelador.CriaEsfera(1.0), { 2.0, 0.0, 0.0 });
  const TArvoreCSG blocoAntes = modelador.Modelo(bloco).arvore;

  const TIdModelo uniao = modelador.Une(bloco, esfera);

  EXPECT_EQ(modelador.Modelos().size(), 4u);
  EXPECT_EQ(modelador.Modelo(bloco).arvore, blocoAntes);
  EXPECT_EQ(modelador.Modelo(uniao).nome, "#4 União de #1 e #3");

  const TArvoreCSG& arvore = modelador.Modelo(uniao).arvore;
  ASSERT_EQ(arvore->Tipo(), ETipoNoCSG::OPERACAO_BOOLEANA);
  EXPECT_EQ(arvore->Esquerda(), blocoAntes);
  EXPECT_EQ(ClassificaPonto(arvore, { 2.5, 0.0, 0.0 }), EClassificacao::IN);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_transladar_e_rotacionar_com_nos_de_movimento)
{
  TModelador modelador;
  const TIdModelo bloco = modelador.CriaBloco(4.0, 2.0, 2.0);

  const TIdModelo girado = modelador.Rotaciona(bloco, { 0.0, 0.0, 90.0 });
  const TIdModelo movido = modelador.Translada(girado, { 0.0, 0.0, 5.0 });

  const TArvoreCSG& arvore = modelador.Modelo(movido).arvore;
  ASSERT_EQ(arvore->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
  EXPECT_EQ(arvore->Movimento().Tipo(), ETipoMovimentoRigido::TRANSLACAO);
  EXPECT_EQ(arvore->Filho(), modelador.Modelo(girado).arvore);

  // O lado de 4 passa a ficar ao longo de y, e o bloco sobe 5 em z.
  EXPECT_EQ(ClassificaPonto(arvore, { 0.0, 1.5, 5.0 }), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(arvore, { 1.5, 0.0, 5.0 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_escalar_pelos_parametros_da_primitiva)
{
  TModelador modelador;
  const TIdModelo bloco = modelador.CriaBloco(2.0, 2.0, 2.0);

  const TIdModelo escalado = modelador.Escala(bloco, { 2.0, 1.0, 3.0 });

  const TArvoreCSG& arvore = modelador.Modelo(escalado).arvore;
  ASSERT_EQ(arvore->Tipo(), ETipoNoCSG::PRIMITIVA);
  const TBloco& resultado = dynamic_cast<const TBloco&>(arvore->Primitiva());
  EXPECT_DOUBLE_EQ(resultado.LadoX(), 4.0);
  EXPECT_DOUBLE_EQ(resultado.LadoZ(), 6.0);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_manter_os_identificadores_apos_remover)
{
  TModelador modelador;
  const TIdModelo primeiro = modelador.CriaEsfera(1.0);
  const TIdModelo segundo = modelador.Translada(primeiro, { 1.0, 0.0, 0.0 });

  modelador.Remove(primeiro);
  const TIdModelo terceiro = modelador.CriaEsfera(2.0);

  EXPECT_FALSE(modelador.Existe(primeiro));
  EXPECT_TRUE(modelador.Existe(segundo));
  EXPECT_EQ(terceiro, 3);
  EXPECT_EQ(ClassificaPonto(modelador.Modelo(segundo).arvore, { 1.5, 0.0, 0.0 }), EClassificacao::IN);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_rejeitar_identificadores_inexistentes_sem_criar_modelo)
{
  TModelador modelador;
  const TIdModelo bloco = modelador.CriaBloco(1.0, 1.0, 1.0);

  EXPECT_THROW(modelador.Une(bloco, 42), std::invalid_argument);
  EXPECT_THROW(modelador.Translada(42, { 1.0, 0.0, 0.0 }), std::invalid_argument);
  EXPECT_THROW(modelador.Remove(42), std::invalid_argument);
  EXPECT_EQ(modelador.Modelos().size(), 1u);

  // Uma operação que falha também não consome identificador.
  EXPECT_THROW(modelador.CriaBloco(-1.0, 1.0, 1.0), std::invalid_argument);
  EXPECT_EQ(modelador.CriaEsfera(1.0), 2);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_calcular_o_volume_pela_octree_na_caixa_do_modelo)
{
  TModelador modelador;
  const TIdModelo bloco = modelador.Translada(modelador.CriaBloco(2.0, 3.0, 4.0), { 5.0, 5.0, 5.0 });
  const TIdModelo esfera = modelador.CriaEsfera(1.0);

  // A caixa do bloco coincide com ele: o volume é exato em qualquer profundidade.
  EXPECT_NEAR(modelador.Volume(bloco, 1), 24.0, 1e-9);
  EXPECT_NEAR(modelador.Volume(esfera, 6), 4.0 / 3.0 * PI, 0.05);
  EXPECT_THROW(modelador.Volume(esfera, 0), std::invalid_argument);
  EXPECT_THROW(modelador.Volume(esfera, 9), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_salvar_e_carregar_em_um_novo_modelo)
{
  const std::filesystem::path arquivo =
    std::filesystem::temp_directory_path() / "ufc_mg_csg_modelador_teste.txt";

  TModelador modelador;
  const TIdModelo uniao = modelador.Une(
    modelador.CriaBloco(2.0, 2.0, 2.0),
    modelador.Translada(modelador.CriaEsfera(1.0), { 2.0, 0.0, 0.0 })
  );

  modelador.Salva(uniao, arquivo);
  const TIdModelo lido = modelador.Carrega(arquivo);
  std::filesystem::remove(arquivo);

  EXPECT_EQ(modelador.Modelo(lido).nome, "#5 Lido de ufc_mg_csg_modelador_teste.txt");
  const TArvoreCSG& arvore = modelador.Modelo(lido).arvore;
  EXPECT_EQ(ClassificaPonto(arvore, { 2.5, 0.0, 0.0 }), EClassificacao::IN);
  EXPECT_EQ(ClassificaPonto(arvore, { 3.5, 0.0, 0.0 }), EClassificacao::OUT);
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_falhar_ao_carregar_arquivo_inexistente)
{
  TModelador modelador;

  EXPECT_THROW(modelador.Carrega("/arquivo/que/nao/existe.txt"), std::runtime_error);
  EXPECT_TRUE(modelador.Modelos().empty());
}

//----------------------------------------------------------------------------------------------

TEST(ModeladorTest, deve_criar_cilindro_e_intersectar_modelos)
{
  TModelador modelador;
  const TIdModelo cilindro = modelador.CriaCilindro(1.0, 2.0);
  const TIdModelo bloco = modelador.CriaBloco(4.0, 4.0, 1.0);

  const TIdModelo intersecao = modelador.Intersecta(cilindro, bloco);

  EXPECT_EQ(modelador.Modelo(cilindro).nome, "#1 Cilindro de raio 1 e altura 2");
  EXPECT_EQ(modelador.Modelo(intersecao).nome, "#3 Interseção de #1 e #2");

  // O bloco corta o cilindro em z de 0 a 0,5: sobra um disco de raio 1 e altura 0,5.
  EXPECT_NEAR(modelador.Volume(cilindro, 6), 2.0 * PI, 0.1);
  EXPECT_NEAR(modelador.Volume(intersecao, 6), 0.5 * PI, 0.05);
}

//----------------------------------------------------------------------------------------------
