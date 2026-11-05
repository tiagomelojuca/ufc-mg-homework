#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <optional>

#include "Core/ArvoreCSG.h"
#include "Core/Bloco.h"
#include "Core/Esfera.h"
#include "Core/RayCasting.h"

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

  TArvoreCSG CriaEsfera(
    double raio
  )
  {
    return CriaPrimitiva(std::make_shared<TEsfera>(raio));
  }

  //--------------------------------------------------------------------------------------------

  TArvoreCSG Transladada(
    const TArvoreCSG& arvore,
    const TVetor3D& deslocamento
  )
  {
    return CriaMovimentoRigido(
      TMovimentoRigido(ETipoMovimentoRigido::TRANSLACAO, deslocamento),
      arvore
    );
  }

  //--------------------------------------------------------------------------------------------

  void EsperaVetor(
    const TVetor3D& obtido,
    const TVetor3D& esperado
  )
  {
    EXPECT_NEAR(obtido.x, esperado.x, TOLERANCIA);
    EXPECT_NEAR(obtido.y, esperado.y, TOLERANCIA);
    EXPECT_NEAR(obtido.z, esperado.z, TOLERANCIA);
  }

  //--------------------------------------------------------------------------------------------

  TCena CriaCena()
  {
    TCena cena;
    cena.camera = { { 0.0, 0.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, 60.0 };
    cena.luz = { { 0.0, 0.0, 10.0 }, { 1.0, 1.0, 1.0 } };
    cena.material = { { 1.0, 0.5, 0.0 }, 0.1, 0.7, 0.2, 10.0 };
    cena.corDeFundo = { 0.0, 0.0, 0.0 };
    cena.alcance = 20.0;
    return cena;
  }
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_encontrar_a_primeira_intersecao_com_a_esfera)
{
  const std::optional<TIntersecao> intersecao = PrimeiraIntersecao(
    CriaEsfera(1.0),
    TAresta({ 0.0, 0.0, 5.0 }, { 0.0, 0.0, -5.0 })
  );

  ASSERT_TRUE(intersecao.has_value());
  EXPECT_NEAR(intersecao->t, 0.4, TOLERANCIA);
  EsperaVetor(intersecao->ponto, { 0.0, 0.0, 1.0 });
  EsperaVetor(intersecao->normal, { 0.0, 0.0, 1.0 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_dar_a_normal_da_face_atingida_do_bloco)
{
  const std::optional<TIntersecao> intersecao = PrimeiraIntersecao(
    CriaBloco(2.0, 2.0, 2.0),
    TAresta({ -5.0, 0.3, 0.2 }, { 5.0, 0.3, 0.2 })
  );

  ASSERT_TRUE(intersecao.has_value());
  EsperaVetor(intersecao->ponto, { -1.0, 0.3, 0.2 });
  EsperaVetor(intersecao->normal, { -1.0, 0.0, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_nao_haver_intersecao_quando_o_raio_passa_ao_lado)
{
  EXPECT_FALSE(PrimeiraIntersecao(
    CriaEsfera(1.0),
    TAresta({ 2.0, 0.0, 5.0 }, { 2.0, 0.0, -5.0 })
  ).has_value());
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_nao_haver_intersecao_quando_o_raio_parte_de_dentro)
{
  EXPECT_FALSE(PrimeiraIntersecao(
    CriaEsfera(1.0),
    TAresta({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 5.0 })
  ).has_value());
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_atingir_o_filho_mais_proximo_da_uniao)
{
  const TArvoreCSG uniao = CriaOperacaoBooleana(
    EOperacaoBooleana::UNIAO,
    CriaBloco(2.0, 2.0, 2.0),
    Transladada(CriaEsfera(1.0), { 2.0, 0.0, 0.0 })
  );

  const std::optional<TIntersecao> pelaDireita =
    PrimeiraIntersecao(uniao, TAresta({ 5.0, 0.0, 0.0 }, { -5.0, 0.0, 0.0 }));
  ASSERT_TRUE(pelaDireita.has_value());
  EsperaVetor(pelaDireita->ponto, { 3.0, 0.0, 0.0 });
  EsperaVetor(pelaDireita->normal, { 1.0, 0.0, 0.0 });

  const std::optional<TIntersecao> pelaEsquerda =
    PrimeiraIntersecao(uniao, TAresta({ -5.0, 0.0, 0.0 }, { 5.0, 0.0, 0.0 }));
  ASSERT_TRUE(pelaEsquerda.has_value());
  EsperaVetor(pelaEsquerda->ponto, { -1.0, 0.0, 0.0 });
  EsperaVetor(pelaEsquerda->normal, { -1.0, 0.0, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_girar_a_normal_com_a_rotacao)
{
  // O bloco 2 x 4 x 2 girado 90 graus em z fica com 4 em x e 2 em y.
  const TArvoreCSG girado = CriaMovimentoRigido(
    TMovimentoRigido(ETipoMovimentoRigido::ROTACAO, { 0.0, 0.0, 90.0 }),
    CriaBloco(2.0, 4.0, 2.0)
  );

  const std::optional<TIntersecao> intersecao =
    PrimeiraIntersecao(girado, TAresta({ 0.0, -5.0, 0.0 }, { 0.0, 5.0, 0.0 }));

  ASSERT_TRUE(intersecao.has_value());
  EsperaVetor(intersecao->ponto, { 0.0, -1.0, 0.0 });
  EsperaVetor(intersecao->normal, { 0.0, -1.0, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_transformar_a_normal_pela_transposta_da_inversa_na_escala)
{
  // Elipsoide x²/4 + y² + z² = 1, cuja normal é proporcional a (x/4, y, z).
  const TArvoreCSG elipsoide = CriaEscala(TEscala({ 2.0, 1.0, 1.0 }), CriaEsfera(1.0));
  const double x = std::sqrt(2.0);

  const std::optional<TIntersecao> intersecao =
    PrimeiraIntersecao(elipsoide, TAresta({ x, 0.0, 5.0 }, { x, 0.0, -5.0 }));

  ASSERT_TRUE(intersecao.has_value());
  EsperaVetor(intersecao->ponto, { x, 0.0, std::sqrt(0.5) });
  EsperaVetor(intersecao->normal, Normaliza({ x / 4.0, 0.0, std::sqrt(0.5) }));
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_somar_ambiente_difusa_e_especular_de_frente_para_a_luz)
{
  const TCena cena = CriaCena();

  // Normal, luz e observador alinhados: difusa e especular máximas.
  const TVetor3D cor = Ilumina(
    cena.material,
    cena.luz,
    { 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { 0.0, 0.0, 5.0 }
  );

  EsperaVetor(cor, { 1.0, 0.6, 0.2 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_restar_apenas_o_ambiente_de_costas_para_a_luz)
{
  const TCena cena = CriaCena();

  const TVetor3D cor = Ilumina(
    cena.material,
    cena.luz,
    { 0.0, 0.0, 0.0 },
    { 0.0, 0.0, -1.0 },
    { 0.0, 0.0, 5.0 }
  );

  EsperaVetor(cor, { 0.1, 0.05, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_apontar_o_pixel_central_para_o_alvo)
{
  const TCena cena = CriaCena();

  const TAresta raio = RaioDoPixel(cena.camera, 10.0, 1, 1, 3, 3);

  EsperaVetor(raio.Inicio(), { 0.0, 0.0, 5.0 });
  EsperaVetor(raio.Fim(), { 0.0, 0.0, -5.0 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_gerar_a_imagem_com_o_solido_no_centro_e_o_fundo_nos_cantos)
{
  const TCena cena = CriaCena();

  const TImagem imagem = GeraImagem(CriaEsfera(1.0), cena, 9, 9);

  ASSERT_EQ(imagem.largura, 9);
  ASSERT_EQ(imagem.altura, 9);
  ASSERT_EQ(imagem.pixels.size(), 4u * 9u * 9u);

  // O pixel central vê a esfera de frente para a luz; o canto vê o fundo preto.
  const unsigned char* centro = &imagem.pixels[4u * (4 * 9 + 4)];
  EXPECT_EQ(centro[0], 255);
  EXPECT_GT(centro[1], 0);

  const unsigned char* canto = &imagem.pixels[0];
  EXPECT_EQ(canto[0], 0);
  EXPECT_EQ(canto[1], 0);
  EXPECT_EQ(canto[2], 0);
  EXPECT_EQ(canto[3], 255);
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_rejeitar_dimensoes_invalidas)
{
  EXPECT_THROW(GeraImagem(CriaEsfera(1.0), CriaCena(), 0, 10), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------
