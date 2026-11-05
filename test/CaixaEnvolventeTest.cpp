#include <gtest/gtest.h>

#include <memory>

#include "Core/ArvoreCSG.h"
#include "Core/Bloco.h"
#include "Core/CaixaEnvolvente.h"
#include "Core/Esfera.h"

namespace
{
  constexpr double TOLERANCIA = 1e-12;

  //--------------------------------------------------------------------------------------------

  void EsperaCaixa(
    const TCaixa& obtida,
    const TCaixa& esperada
  )
  {
    EXPECT_NEAR(obtida.minimo.x, esperada.minimo.x, TOLERANCIA);
    EXPECT_NEAR(obtida.minimo.y, esperada.minimo.y, TOLERANCIA);
    EXPECT_NEAR(obtida.minimo.z, esperada.minimo.z, TOLERANCIA);
    EXPECT_NEAR(obtida.maximo.x, esperada.maximo.x, TOLERANCIA);
    EXPECT_NEAR(obtida.maximo.y, esperada.maximo.y, TOLERANCIA);
    EXPECT_NEAR(obtida.maximo.z, esperada.maximo.z, TOLERANCIA);
  }
}

//----------------------------------------------------------------------------------------------

TEST(CaixaEnvolventeTest, deve_dar_a_caixa_canonica_das_primitivas)
{
  EsperaCaixa(
    CaixaEnvolvente(CriaPrimitiva(std::make_shared<TBloco>(2.0, 4.0, 6.0))),
    { { -1.0, -2.0, -3.0 }, { 1.0, 2.0, 3.0 } }
  );
  EsperaCaixa(
    CaixaEnvolvente(CriaPrimitiva(std::make_shared<TEsfera>(1.5))),
    { { -1.5, -1.5, -1.5 }, { 1.5, 1.5, 1.5 } }
  );
}

//----------------------------------------------------------------------------------------------

TEST(CaixaEnvolventeTest, deve_mover_a_caixa_com_a_translacao_e_a_rotacao)
{
  const TArvoreCSG bloco = CriaPrimitiva(std::make_shared<TBloco>(2.0, 4.0, 6.0));

  const TArvoreCSG girado = CriaMovimentoRigido(
    TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, { 0.0, 0.0, 90.0 }),
    bloco
  );
  const TArvoreCSG movido = CriaMovimentoRigido(
    TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, { 10.0, 0.0, 0.0 }),
    girado
  );

  // Girado 90 graus em z, o bloco troca as extensões de x e y.
  EsperaCaixa(CaixaEnvolvente(movido), { { 8.0, -1.0, -3.0 }, { 12.0, 1.0, 3.0 } });
}

//----------------------------------------------------------------------------------------------

TEST(CaixaEnvolventeTest, deve_unir_as_caixas_na_uniao)
{
  const TArvoreCSG uniao = CriaOperacaoBooleana(
    EOperacaoBooleana::UNIAO,
    CriaPrimitiva(std::make_shared<TBloco>(2.0, 2.0, 2.0)),
    CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, { 3.0, 0.0, 0.0 }),
      CriaPrimitiva(std::make_shared<TEsfera>(1.0))
    )
  );

  EsperaCaixa(CaixaEnvolvente(uniao), { { -1.0, -1.0, -1.0 }, { 4.0, 1.0, 1.0 } });
}

//----------------------------------------------------------------------------------------------

TEST(CaixaEnvolventeTest, deve_intersectar_as_caixas_na_intersecao)
{
  const TArvoreCSG intersecao = CriaOperacaoBooleana(
    EOperacaoBooleana::INTERSECAO,
    CriaPrimitiva(std::make_shared<TBloco>(2.0, 2.0, 2.0)),
    CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, { 1.0, 0.0, 0.0 }),
      CriaPrimitiva(std::make_shared<TEsfera>(1.0))
    )
  );

  EsperaCaixa(CaixaEnvolvente(intersecao), { { 0.0, -1.0, -1.0 }, { 1.0, 1.0, 1.0 } });
}

//----------------------------------------------------------------------------------------------
