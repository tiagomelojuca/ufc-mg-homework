#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <vector>

#include "Core/Aramado.h"
#include "Core/ArvoreCSG.h"
#include "Core/Bloco.h"
#include "Core/Cilindro.h"
#include "Core/ClassificadorCSG.h"
#include "Core/Esfera.h"

namespace
{
  constexpr double TOLERANCIA = 1e-9;

  //--------------------------------------------------------------------------------------------

  TArvoreCSG CriaBloco(
    double ladoX,
    double ladoY,
    double ladoZ
  )
  {
    return CriaPrimitiva(std::make_shared<TBloco>(ladoX, ladoY, ladoZ));
  }

  //--------------------------------------------------------------------------------------------

  double ComprimentoTotal(
    const std::vector<TAresta>& segmentos
  )
  {
    double total = 0.0;
    for (const TAresta& segmento : segmentos) {
      total += Norma(segmento.Direcao());
    }

    return total;
  }
}

//----------------------------------------------------------------------------------------------

TEST(AramadoTest, deve_dar_os_doze_lados_do_bloco)
{
  const TArvoreCSG bloco = CriaBloco(2.0, 4.0, 6.0);

  const std::vector<TArestaCandidata> candidatas = ArestasCandidatas(bloco);
  const std::vector<TAresta> segmentos = SegmentosEonS(bloco);

  ASSERT_EQ(candidatas.size(), 12u);
  for (const TArestaCandidata& candidata : candidatas) {
    EXPECT_FALSE(candidata.pontoDaCurva.has_value());
  }

  // Cada lado aparece quatro vezes.
  ASSERT_EQ(segmentos.size(), 12u);
  EXPECT_NEAR(ComprimentoTotal(segmentos), 4.0 * (2.0 + 4.0 + 6.0), TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(AramadoTest, deve_aproximar_a_esfera_por_cordas_com_o_ponto_da_curva_na_superficie)
{
  const TArvoreCSG esfera = CriaPrimitiva(std::make_shared<TEsfera>(1.5));

  const std::vector<TArestaCandidata> candidatas = ArestasCandidatas(esfera);

  ASSERT_FALSE(candidatas.empty());
  for (const TArestaCandidata& candidata : candidatas) {
    ASSERT_TRUE(candidata.pontoDaCurva.has_value());
    EXPECT_NEAR(Norma(*candidata.pontoDaCurva), 1.5, TOLERANCIA);
    EXPECT_NEAR(Norma(candidata.aresta.Inicio()), 1.5, TOLERANCIA);
  }

  // Sozinha, a esfera mostra todas as cordas.
  EXPECT_EQ(SegmentosEonS(esfera).size(), candidatas.size());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoTest, deve_dar_as_bordas_das_bases_e_geratrizes_do_cilindro)
{
  const TArvoreCSG cilindro = CriaPrimitiva(std::make_shared<TCilindro>(1.0, 3.0));

  const std::vector<TArestaCandidata> candidatas = ArestasCandidatas(cilindro);

  int geratrizes = 0;
  for (const TArestaCandidata& candidata : candidatas) {
    if (!candidata.pontoDaCurva) {
      ++geratrizes;
      EXPECT_NEAR(Norma(candidata.aresta.Direcao()), 3.0, TOLERANCIA);
    }
  }

  EXPECT_EQ(geratrizes, 8);
  EXPECT_EQ(SegmentosEonS(cilindro).size(), candidatas.size());
}

//----------------------------------------------------------------------------------------------

TEST(AramadoTest, deve_levar_as_arestas_pelos_movimentos_rigidos)
{
  const TArvoreCSG movido = CriaMovimentoRigido(
    TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, { 10.0, 0.0, 0.0 }),
    CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, { 0.0, 0.0, 90.0 }),
      CriaBloco(2.0, 4.0, 2.0)
    )
  );

  // Girado e deslocado, o bloco ocupa x em [8, 12], y em [-1, 1] e z em [-1, 1].
  for (const TArestaCandidata& candidata : ArestasCandidatas(movido)) {
    for (const TVetor3D& ponto : { candidata.aresta.Inicio(), candidata.aresta.Fim() }) {
      EXPECT_NEAR(std::abs(ponto.x - 10.0), 2.0, TOLERANCIA);
      EXPECT_NEAR(std::abs(ponto.y), 1.0, TOLERANCIA);
      EXPECT_NEAR(std::abs(ponto.z), 1.0, TOLERANCIA);
    }
  }

  EXPECT_EQ(SegmentosEonS(movido).size(), 12u);
}

//----------------------------------------------------------------------------------------------

TEST(AramadoTest, deve_esconder_as_arestas_de_primitiva_interna)
{
  const TArvoreCSG uniao = CriaOperacaoBooleana(
    EOperacaoBooleana::UNIAO,
    CriaBloco(2.0, 2.0, 2.0),
    CriaPrimitiva(std::make_shared<TEsfera>(0.5))
  );

  const std::vector<TAresta> segmentos = SegmentosEonS(uniao);

  ASSERT_EQ(segmentos.size(), 12u);
  EXPECT_NEAR(ComprimentoTotal(segmentos), 24.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(AramadoTest, deve_cortar_as_arestas_que_entram_no_outro_solido)
{
  // A barra 4 x 1 x 1 atravessa o cubo de lado 2 ao longo de x: os seus lados longos somem dentro
  // dele, entre x = -1 e x = 1.
  const TArvoreCSG uniao = CriaOperacaoBooleana(
    EOperacaoBooleana::UNIAO,
    CriaBloco(2.0, 2.0, 2.0),
    CriaBloco(4.0, 1.0, 1.0)
  );

  const std::vector<TAresta> segmentos = SegmentosEonS(uniao);

  // Cubo: 12 lados de 2. Barra: 4 lados longos cortados em 2 trechos de 1 e 8 lados curtos de 1.
  EXPECT_EQ(segmentos.size(), 12u + 8u + 8u);
  EXPECT_NEAR(ComprimentoTotal(segmentos), 24.0 + 8.0 + 8.0, TOLERANCIA);

  for (const TAresta& segmento : segmentos) {
    EXPECT_EQ(ClassificaPonto(uniao, segmento.Ponto(0.5)), EClassificacao::ON);
  }
}

//----------------------------------------------------------------------------------------------
