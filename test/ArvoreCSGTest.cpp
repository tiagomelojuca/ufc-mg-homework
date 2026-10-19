#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core/ArvoreCSG.h"

namespace
{
  class TPrimitivaTeste : public TPrimitiva
  {
    public:
      explicit TPrimitivaTeste(
        ETipoPrimitiva tipo
      ) :
        tipo(tipo)
      {
      }

      ETipoPrimitiva Tipo() const override
      {
        return tipo;
      }

      EClassificacao ClassificaPonto(
        const TVetor3D&
      ) const override
      {
        return EClassificacao::OUT;
      }

      std::vector<double> IntersecoesComFronteira(
        const TAresta&
      ) const override
      {
        return {};
      }

    private:
      ETipoPrimitiva tipo;
  };

  TArvoreCSG CriaFolha(
    ETipoPrimitiva tipo
  )
  {
    return CriaPrimitiva(std::make_shared<TPrimitivaTeste>(tipo));
  }
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_criar_folha_com_a_primitiva)
{
  const TArvoreCSG folha = CriaFolha(ETipoPrimitiva::ESFERA);

  EXPECT_EQ(folha->Tipo(), ETipoNoCSG::PRIMITIVA);
  EXPECT_EQ(folha->Primitiva().Tipo(), ETipoPrimitiva::ESFERA);
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_criar_operacao_booleana_com_dois_filhos_ordenados)
{
  const TArvoreCSG bloco = CriaFolha(ETipoPrimitiva::BLOCO);
  const TArvoreCSG esfera = CriaFolha(ETipoPrimitiva::ESFERA);

  const TArvoreCSG uniao = CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, bloco, esfera);

  EXPECT_EQ(uniao->Tipo(), ETipoNoCSG::OPERACAO_BOOLEANA);
  EXPECT_EQ(uniao->Operacao(), EOperacaoBooleana::UNIAO);
  EXPECT_EQ(uniao->Esquerda(), bloco);
  EXPECT_EQ(uniao->Direita(), esfera);
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_criar_movimento_rigido_com_um_filho)
{
  const TArvoreCSG cilindro = CriaFolha(ETipoPrimitiva::CILINDRO);
  const TMovimentoRigido translacao(ETipoMovimentoRigido::TRANSLACAO, { 1.0, 2.0, 3.0 });

  const TArvoreCSG movido = CriaMovimentoRigido(translacao, cilindro);

  EXPECT_EQ(movido->Tipo(), ETipoNoCSG::MOVIMENTO_RIGIDO);
  EXPECT_EQ(movido->Movimento().Tipo(), ETipoMovimentoRigido::TRANSLACAO);
  EXPECT_DOUBLE_EQ(movido->Movimento().Valores().x, 1.0);
  EXPECT_DOUBLE_EQ(movido->Movimento().Valores().y, 2.0);
  EXPECT_DOUBLE_EQ(movido->Movimento().Valores().z, 3.0);
  EXPECT_EQ(movido->Filho(), cilindro);
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_aceitar_movimento_rigido_sobre_operacao_booleana)
{
  const TArvoreCSG uniao = CriaOperacaoBooleana(
    EOperacaoBooleana::UNIAO,
    CriaFolha(ETipoPrimitiva::BLOCO),
    CriaFolha(ETipoPrimitiva::ESFERA)
  );
  const TMovimentoRigido rotacao(ETipoMovimentoRigido::ROTACAO, { 0.0, 90.0, 0.0 });

  const TArvoreCSG movido = CriaMovimentoRigido(rotacao, uniao);

  EXPECT_EQ(movido->Filho()->Tipo(), ETipoNoCSG::OPERACAO_BOOLEANA);
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_preservar_as_entradas_ao_compartilhar_subarvores)
{
  const TArvoreCSG bloco = CriaFolha(ETipoPrimitiva::BLOCO);
  const TArvoreCSG esfera = CriaFolha(ETipoPrimitiva::ESFERA);

  const TArvoreCSG uniao = CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, bloco, esfera);
  const TArvoreCSG intersecao = CriaOperacaoBooleana(EOperacaoBooleana::INTERSECAO, bloco, esfera);

  EXPECT_EQ(bloco->Tipo(), ETipoNoCSG::PRIMITIVA);
  EXPECT_EQ(uniao->Esquerda(), intersecao->Esquerda());
  EXPECT_EQ(uniao->Operacao(), EOperacaoBooleana::UNIAO);
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_rejeitar_subarvores_vazias)
{
  const TArvoreCSG bloco = CriaFolha(ETipoPrimitiva::BLOCO);
  const TMovimentoRigido translacao(ETipoMovimentoRigido::TRANSLACAO, { 0.0, 0.0, 0.0 });

  EXPECT_THROW(CriaPrimitiva(nullptr), std::invalid_argument);
  EXPECT_THROW(CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, bloco, nullptr), std::invalid_argument);
  EXPECT_THROW(CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, nullptr, bloco), std::invalid_argument);
  EXPECT_THROW(CriaMovimentoRigido(translacao, nullptr), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_rejeitar_movimento_rigido_com_valores_nao_finitos)
{
  const double infinito = std::numeric_limits<double>::infinity();
  const double indefinido = std::numeric_limits<double>::quiet_NaN();

  EXPECT_THROW(
    TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, { infinito, 0.0, 0.0 }),
    std::invalid_argument
  );
  EXPECT_THROW(
    TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, { 0.0, 0.0, indefinido }),
    std::invalid_argument
  );
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_rejeitar_acesso_incompativel_com_o_tipo_do_no)
{
  const TArvoreCSG bloco = CriaFolha(ETipoPrimitiva::BLOCO);
  const TArvoreCSG uniao = CriaOperacaoBooleana(EOperacaoBooleana::UNIAO, bloco, bloco);
  const TArvoreCSG movido = CriaMovimentoRigido(
    TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, { 1.0, 0.0, 0.0 }),
    bloco
  );

  EXPECT_THROW(bloco->Esquerda(), std::logic_error);
  EXPECT_THROW(bloco->Movimento(), std::logic_error);
  EXPECT_THROW(uniao->Primitiva(), std::logic_error);
  EXPECT_THROW(uniao->Filho(), std::logic_error);
  EXPECT_THROW(movido->Operacao(), std::logic_error);
  EXPECT_THROW(movido->Direita(), std::logic_error);
}

//----------------------------------------------------------------------------------------------

TEST(ArvoreCSGTest, deve_usar_os_codigos_do_arquivo)
{
  EXPECT_EQ(static_cast<char>(ETipoPrimitiva::BLOCO), 'B');
  EXPECT_EQ(static_cast<char>(ETipoPrimitiva::ESFERA), 'E');
  EXPECT_EQ(static_cast<char>(ETipoPrimitiva::CILINDRO), 'C');
  EXPECT_EQ(static_cast<char>(EOperacaoBooleana::UNIAO), 'U');
  EXPECT_EQ(static_cast<char>(EOperacaoBooleana::INTERSECAO), 'I');
  EXPECT_EQ(static_cast<char>(EOperacaoBooleana::DIFERENCA), 'D');
  EXPECT_EQ(static_cast<char>(ETipoMovimentoRigido::TRANSLACAO), 'T');
  EXPECT_EQ(static_cast<char>(ETipoMovimentoRigido::ROTACAO), 'R');
}

//----------------------------------------------------------------------------------------------
