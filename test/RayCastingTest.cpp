#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <optional>

#include "Core/ArvoreCSG.h"
#include "Core/Bloco.h"
#include "Core/Cilindro.h"
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

TEST(RayCastingTest, deve_atingir_a_primitiva_escalada_pelos_parametros)
{
  // O bloco 2 x 2 x 6 deslocado de 1 em z tem a face de cima em z = 4.
  const TArvoreCSG alto = Transladada(
    Escala(CriaBloco(2.0, 2.0, 2.0), { 1.0, 1.0, 3.0 }),
    { 0.0, 0.0, 1.0 }
  );

  const std::optional<TIntersecao> intersecao =
    PrimeiraIntersecao(alto, TAresta({ 0.2, 0.3, 10.0 }, { 0.2, 0.3, -10.0 }));

  ASSERT_TRUE(intersecao.has_value());
  EsperaVetor(intersecao->ponto, { 0.2, 0.3, 4.0 });
  EsperaVetor(intersecao->normal, { 0.0, 0.0, 1.0 });
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

TEST(RayCastingTest, deve_projetar_o_alvo_no_centro_da_imagem)
{
  const TCena cena = CriaCena();

  const TPontoImagem projetado = ProjetaPonto(cena.camera, { 0.0, 0.0, 0.0 }, 480, 360);

  EXPECT_NEAR(projetado.coluna, 240.0, TOLERANCIA);
  EXPECT_NEAR(projetado.linha, 180.0, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_projetar_um_ponto_do_raio_no_centro_do_seu_pixel)
{
  TCena cena = CriaCena();
  cena.camera = { { 3.0, 2.0, 6.0 }, { 0.5, -0.5, 0.0 }, { 0.0, 1.0, 0.0 }, 50.0 };

  // Qualquer ponto do raio de um pixel aparece no centro desse pixel.
  const TAresta raio = RaioDoPixel(cena.camera, 10.0, 37, 211, 320, 240);
  const TPontoImagem projetado = ProjetaPonto(cena.camera, raio.Ponto(0.6), 320, 240);

  EXPECT_NEAR(projetado.coluna, 37.5, 1e-6);
  EXPECT_NEAR(projetado.linha, 211.5, 1e-6);
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_manter_a_aresta_toda_a_frente_da_camera)
{
  const TCena cena = CriaCena();
  const TAresta aresta({ -1.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 });

  const std::optional<TAresta> recortada = RecortaAFrenteDaCamera(cena.camera, aresta);

  ASSERT_TRUE(recortada.has_value());
  EsperaVetor(recortada->Inicio(), aresta.Inicio());
  EsperaVetor(recortada->Fim(), aresta.Fim());
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_descartar_a_aresta_toda_atras_da_camera)
{
  const TCena cena = CriaCena();

  EXPECT_FALSE(RecortaAFrenteDaCamera(
    cena.camera,
    TAresta({ -1.0, 0.0, 7.0 }, { 1.0, 0.0, 9.0 })
  ).has_value());
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_recortar_a_aresta_que_passa_por_tras_da_camera)
{
  // A câmera está em z = 5 olhando para -z; a parte com z > 5 fica atrás dela.
  const TCena cena = CriaCena();

  const std::optional<TAresta> recortada = RecortaAFrenteDaCamera(
    cena.camera,
    TAresta({ 0.0, 1.0, 0.0 }, { 0.0, 1.0, 10.0 })
  );

  ASSERT_TRUE(recortada.has_value());
  EsperaVetor(recortada->Inicio(), { 0.0, 1.0, 0.0 });
  EXPECT_NEAR(recortada->Fim().z, 5.0 - 1e-3, TOLERANCIA);
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_dar_a_normal_do_topo_e_da_lateral_do_cilindro)
{
  const TArvoreCSG cilindro = CriaPrimitiva(std::make_shared<TCilindro>(1.0, 2.0));

  const std::optional<TIntersecao> peloTopo =
    PrimeiraIntersecao(cilindro, TAresta({ 0.2, 0.3, 10.0 }, { 0.2, 0.3, -10.0 }));
  ASSERT_TRUE(peloTopo.has_value());
  EsperaVetor(peloTopo->ponto, { 0.2, 0.3, 2.0 });
  EsperaVetor(peloTopo->normal, { 0.0, 0.0, 1.0 });

  const std::optional<TIntersecao> pelaLateral =
    PrimeiraIntersecao(cilindro, TAresta({ 5.0, 0.0, 1.0 }, { -5.0, 0.0, 1.0 }));
  ASSERT_TRUE(pelaLateral.has_value());
  EsperaVetor(pelaLateral->ponto, { 1.0, 0.0, 1.0 });
  EsperaVetor(pelaLateral->normal, { 1.0, 0.0, 0.0 });
}

//----------------------------------------------------------------------------------------------

TEST(RayCastingTest, deve_dar_a_normal_do_filho_que_limita_a_intersecao)
{
  // A esfera de raio 1,2 corta os cantos do bloco 2 x 2 x 2, mas não o centro das faces.
  const TArvoreCSG intersecao = CriaOperacaoBooleana(
    EOperacaoBooleana::INTERSECAO,
    CriaBloco(2.0, 2.0, 2.0),
    CriaEsfera(1.2)
  );

  const std::optional<TIntersecao> pelaFace =
    PrimeiraIntersecao(intersecao, TAresta({ 5.0, 0.0, 0.0 }, { -5.0, 0.0, 0.0 }));
  ASSERT_TRUE(pelaFace.has_value());
  EsperaVetor(pelaFace->ponto, { 1.0, 0.0, 0.0 });
  EsperaVetor(pelaFace->normal, { 1.0, 0.0, 0.0 });

  const std::optional<TIntersecao> peloCanto =
    PrimeiraIntersecao(intersecao, TAresta({ 5.0, 5.0, 5.0 }, { -5.0, -5.0, -5.0 }));
  const double coordenada = 1.2 / std::sqrt(3.0);
  ASSERT_TRUE(peloCanto.has_value());
  EsperaVetor(peloCanto->ponto, { coordenada, coordenada, coordenada });
  EsperaVetor(peloCanto->normal, Normaliza({ 1.0, 1.0, 1.0 }));
}

//----------------------------------------------------------------------------------------------
