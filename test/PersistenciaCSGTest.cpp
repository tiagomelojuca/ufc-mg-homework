#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

#include "Core/Bloco.h"
#include "Core/Esfera.h"
#include "Persistence/PersistenciaCSG.h"

namespace
{
  TArvoreCSG CriaBloco(
    double ladoX,
    double ladoY,
    double ladoZ
  )
  {
    return CriaPrimitiva(std::make_shared<TBloco>(ladoX, ladoY, ladoZ));
  }

  TArvoreCSG CriaEsfera(
    double raio
  )
  {
    return CriaPrimitiva(std::make_shared<TEsfera>(raio));
  }

  TArvoreCSG Translada(
    const TArvoreCSG& arvore,
    const TVetor3D& deslocamento
  )
  {
    return CriaMovimentoRigido(TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, deslocamento), arvore);
  }

  TArvoreCSG Rotaciona(
    const TArvoreCSG& arvore,
    const TVetor3D& angulos
  )
  {
    return CriaMovimentoRigido(TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, angulos), arvore);
  }
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_codificar_no_formato_do_quadro)
{
  const TArvoreCSG esfera = Translada(CriaEsfera(0.5), { 1.0, 0.0, 0.0 });
  const TArvoreCSG bloco = Rotaciona(Translada(CriaBloco(2.0, 1.0, 1.0), { 0.0, 0.0, 1.0 }), { 0.0, 0.0, 90.0 });

  const std::string texto = TPersistenciaCSG().Codifica(
    CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, esfera, bloco)
  );

  EXPECT_EQ(
    texto,
    "No1; E; S 0.5; T 1,0,0\n"
    "No2; B; S 2,1,1; T 0,0,1; R 0,0,90\n"
    "No3; U No1 No2\n"
  );
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_ler_o_exemplo_do_quadro)
{
  const TArvoreCSG raiz = TPersistenciaCSG().Decodifica(
    "No1; E; S 0.5; T 1,0,0\n"
    "No2; B; S 2,1,1; T 0,0,1; R 0,0,90\n"
    "No3; U No1 No2\n"
  );

  ASSERT_EQ(raiz->Tipo(), ETipoNoCSG::OPERACAO_BOOLEANA);
  EXPECT_EQ(raiz->Operacao(), EOperacaoBooleana::UNIAO);

  const TArvoreCSG& esfera = raiz->Esquerda();
  ASSERT_EQ(esfera->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
  EXPECT_EQ(esfera->Movimento().Tipo(), ETipoMovimentoRigido::TRANSLACAO);
  EXPECT_EQ(esfera->Filho()->Primitiva().Tipo(), ETipoPrimitiva::ESFERA);

  // A rotação é lida por último e, portanto, fica acima da translação.
  const TArvoreCSG& bloco = raiz->Direita();
  ASSERT_EQ(bloco->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
  EXPECT_EQ(bloco->Movimento().Tipo(), ETipoMovimentoRigido::ROTACAO);
  EXPECT_DOUBLE_EQ(bloco->Movimento().Valores().z, 90.0);
  ASSERT_EQ(bloco->Filho()->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
  EXPECT_EQ(bloco->Filho()->Movimento().Tipo(), ETipoMovimentoRigido::TRANSLACAO);

  const auto& primitiva = dynamic_cast<const TBloco&>(bloco->Filho()->Filho()->Primitiva());
  EXPECT_DOUBLE_EQ(primitiva.LadoX(), 2.0);
  EXPECT_DOUBLE_EQ(primitiva.LadoY(), 1.0);
  EXPECT_DOUBLE_EQ(primitiva.LadoZ(), 1.0);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_preservar_a_arvore_na_ida_e_volta)
{
  const TArvoreCSG original = Translada(
    CriaOperacaoBooleana(
      EOperacaoBooleana::DIFERENCA,
      CriaBloco(3.0, 0.25, 1.5),
      Rotaciona(CriaEsfera(0.125), { 10.0, 20.0, 30.0 })
    ),
    { -1.5, 2.0, 0.75 }
  );

  const TPersistenciaCSG persistencia;
  const std::string texto = persistencia.Codifica(original);

  EXPECT_EQ(persistencia.Codifica(persistencia.Decodifica(texto)), texto);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_escrever_movimento_sobre_operacao_booleana_na_mesma_linha)
{
  const TArvoreCSG arvore = Translada(
    CriaOperacaoBooleana(EOperacaoBooleana::INTERSECAO, CriaBloco(1.0, 1.0, 1.0), CriaEsfera(0.75)),
    { 0.0, 2.0, 0.0 }
  );

  EXPECT_EQ(
    TPersistenciaCSG().Codifica(arvore),
    "No1; B; S 1,1,1\n"
    "No2; E; S 0.75\n"
    "No3; I No1 No2; T 0,2,0\n"
  );
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_escrever_uma_vez_a_subarvore_compartilhada)
{
  const TArvoreCSG esfera = CriaEsfera(1.0);
  const TArvoreCSG movida = Translada(esfera, { 3.0, 0.0, 0.0 });

  EXPECT_EQ(
    TPersistenciaCSG().Codifica(CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, esfera, movida)),
    "No1; E; S 1\n"
    "No2; No1; T 3,0,0\n"
    "No3; U No1 No2\n"
  );
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_ler_movimento_sobre_no_ja_definido)
{
  const TArvoreCSG raiz = TPersistenciaCSG().Decodifica(
    "No1; E; S 1\n"
    "No2; No1; T 3,0,0\n"
  );

  ASSERT_EQ(raiz->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
  EXPECT_EQ(raiz->Filho()->Primitiva().Tipo(), ETipoPrimitiva::ESFERA);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_tolerar_espacos_e_linhas_vazias)
{
  const TArvoreCSG raiz = TPersistenciaCSG().Decodifica(
    "\n"
    "  No1 ;B;  S 1 , 2 , 3 ;T 0,0,0  \n"
    "\n"
  );

  EXPECT_EQ(raiz->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_rejeitar_arquivos_invalidos)
{
  const TPersistenciaCSG persistencia;

  EXPECT_THROW(persistencia.Decodifica(""), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; X; S 1"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; E"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; E; S 1,2"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; B; S 1,2"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; E; S abc"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; E; S -1"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; E; S 1; T 1,2"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; E; S 1; Q 1,2,3"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; U No2 No3"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; E; S 1\nNo1; E; S 2"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("B; E; S 1"), std::invalid_argument);
  EXPECT_THROW(persistencia.Decodifica("No1; C; S 1,2"), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_informar_a_linha_do_erro)
{
  try {
    TPersistenciaCSG().Decodifica("No1; E; S 1\nNo2; U No1 No9\n");
    FAIL();
  } catch (const std::invalid_argument& erro) {
    EXPECT_EQ(std::string(erro.what()).rfind("Linha 2:", 0), 0u);
  }
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_salvar_e_carregar_arquivo)
{
  const std::filesystem::path arquivo =
    std::filesystem::temp_directory_path() / "persistencia_csg_teste.txt";
  const TPersistenciaCSG persistencia;
  const TArvoreCSG original = CriaOperacaoBooleana(
    EOperacaoBooleana::UNIAO,
    CriaBloco(1.0, 2.0, 3.0),
    Translada(CriaEsfera(1.0), { 0.0, 0.0, 2.0 })
  );

  persistencia.Salva(original, arquivo);
  const TArvoreCSG carregada = persistencia.Carrega(arquivo);
  std::filesystem::remove(arquivo);

  EXPECT_EQ(persistencia.Codifica(carregada), persistencia.Codifica(original));
}

//----------------------------------------------------------------------------------------------

TEST(PersistenciaCSGTest, deve_falhar_ao_carregar_arquivo_inexistente)
{
  EXPECT_THROW(
    TPersistenciaCSG().Carrega(std::filesystem::temp_directory_path() / "nao_existe_csg.txt"),
    std::runtime_error
  );
}

//----------------------------------------------------------------------------------------------
