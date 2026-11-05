#include "PainelRayCasting.h"

#include <cmath>
#include <cstdint>
#include <utility>

namespace
{
  constexpr double PI = 3.14159265358979323846;
}

//----------------------------------------------------------------------------------------------

TPainelRayCasting::TPainelRayCasting(
  TArvoreCSG arvore
) :
  arvore(std::move(arvore)),
  azimuteGraus(35.0f),
  elevacaoGraus(25.0f),
  distancia(8.0f),
  largura(480),
  altura(360),
  precisaRenderizar(true),
  textura(0)
{
}

//----------------------------------------------------------------------------------------------

void TPainelRayCasting::DefineArvore(
  TArvoreCSG novaArvore
)
{
  arvore = std::move(novaArvore);
  precisaRenderizar = true;
}

//----------------------------------------------------------------------------------------------

void TPainelRayCasting::Desenha()
{
  ImGui::Begin("Ray-casting");

  // Cada controle alterado pede uma nova imagem.
  precisaRenderizar |= ImGui::SliderFloat("Azimute", &azimuteGraus, -180.0f, 180.0f, "%.0f°");
  precisaRenderizar |= ImGui::SliderFloat("Elevação", &elevacaoGraus, -89.0f, 89.0f, "%.0f°");
  precisaRenderizar |= ImGui::SliderFloat("Distância", &distancia, 2.0f, 30.0f, "%.1f");

  if (arvore && precisaRenderizar) {
    AtualizaTextura();
    precisaRenderizar = false;
  }

  if (textura != 0) {
    ImGui::Image(
      static_cast<ImTextureID>(static_cast<std::uintptr_t>(textura)),
      ImVec2(static_cast<float>(largura), static_cast<float>(altura))
    );
  } else {
    ImGui::TextUnformatted("Nenhum modelo para exibir.");
  }

  ImGui::End();
}

//----------------------------------------------------------------------------------------------

TCena TPainelRayCasting::MontaCena() const
{
  const double azimute = azimuteGraus * PI / 180.0;
  const double elevacao = elevacaoGraus * PI / 180.0;

  // Câmera sobre a esfera de raio distancia em torno da origem, com y para cima.
  const TVetor3D posicao = {
    distancia * std::cos(elevacao) * std::sin(azimute),
    distancia * std::sin(elevacao),
    distancia * std::cos(elevacao) * std::cos(azimute)
  };

  TCena cena;
  cena.camera = { posicao, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, 45.0 };

  // A luz acompanha a câmera, um pouco acima dela, para que a face vista esteja sempre iluminada.
  cena.luz = { posicao + TVetor3D { 0.0, distancia, 0.0 }, { 1.0, 1.0, 1.0 } };
  cena.material = { { 0.85, 0.45, 0.2 }, 0.15, 0.7, 0.3, 32.0 };
  cena.corDeFundo = { 0.055, 0.075, 0.11 };
  cena.alcance = 2.0 * distancia + 20.0;

  return cena;
}

//----------------------------------------------------------------------------------------------

void TPainelRayCasting::AtualizaTextura()
{
  const TImagem imagem = GeraImagem(arvore, MontaCena(), largura, altura);

  if (textura == 0) {
    glGenTextures(1, &textura);
  }

  glBindTexture(GL_TEXTURE_2D, textura);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    imagem.largura,
    imagem.altura,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    imagem.pixels.data()
  );
}

//----------------------------------------------------------------------------------------------
