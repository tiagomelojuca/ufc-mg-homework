#include "PainelRayCasting.h"

#include <cmath>
#include <cstdint>
#include <utility>

#include "Core/Aramado.h"
#include "Core/CaixaEnvolvente.h"

namespace
{
  constexpr double PI = 3.14159265358979323846;
}

//----------------------------------------------------------------------------------------------

TPainelRayCasting::TPainelRayCasting() :
  modo(EModoExibicao::RAY_CASTING),
  alvo({ 0.0, 0.0, 0.0 }),
  raioDaCena(1.0),
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
  exibicaoSMC.DefineArvore(arvore);
  precisaRenderizar = true;
  aramado.clear();

  if (arvore) {
    aramado = SegmentosEonS(arvore);

    const TCaixa caixa = CaixaEnvolvente(arvore);
    alvo = Centro(caixa);
    raioDaCena = 0.5 * Norma(caixa.maximo - caixa.minimo);
    distancia = static_cast<float>(3.0 * raioDaCena);
  }
}

//----------------------------------------------------------------------------------------------

void TPainelRayCasting::Desenha()
{
  ImGui::SetNextWindowPos(ImVec2(460.0f, 10.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(510.0f, 700.0f), ImGuiCond_FirstUseEver);
  ImGui::Begin("Visualização");

  if (!arvore) {
    ImGui::TextDisabled("Selecione um modelo na lista para visualizá-lo.");
    ImGui::End();
    return;
  }

  int modoEscolhido = static_cast<int>(modo);
  ImGui::RadioButton("Ray-casting", &modoEscolhido, static_cast<int>(EModoExibicao::RAY_CASTING));
  ImGui::SameLine();
  ImGui::RadioButton("Aramado (EonS)", &modoEscolhido, static_cast<int>(EModoExibicao::ARAMADO));
  modo = static_cast<EModoExibicao>(modoEscolhido);

  // Cada controle alterado pede uma nova imagem.
  precisaRenderizar |= ImGui::SliderFloat("Azimute", &azimuteGraus, -180.0f, 180.0f, "%.0f°");
  precisaRenderizar |= ImGui::SliderFloat("Elevação", &elevacaoGraus, -89.0f, 89.0f, "%.0f°");
  precisaRenderizar |= ImGui::SliderFloat(
    "Distância",
    &distancia,
    0.1f,
    1000.0f,
    "%.2f",
    ImGuiSliderFlags_Logarithmic
  );

  const TCamera camera = MontaCena().camera;
  const ImVec2 tamanho(static_cast<float>(largura), static_cast<float>(altura));

  if (modo == EModoExibicao::RAY_CASTING) {
    if (precisaRenderizar) {
      AtualizaTextura();
      precisaRenderizar = false;
    }

    ImGui::Image(static_cast<ImTextureID>(static_cast<std::uintptr_t>(textura)), tamanho);
  } else {
    // Reserva a área da imagem e desenha o aramado nela.
    ImGui::Dummy(tamanho);
    DesenhaAramado(camera, ImGui::GetItemRectMin());
  }

  exibicaoSMC.DesenhaSobreImagem(camera, ImGui::GetItemRectMin(), largura, altura);

  if (modo == EModoExibicao::ARAMADO) {
    ImGui::Text("%zu segmentos EonS", aramado.size());
  }

  exibicaoSMC.DesenhaControles();

  ImGui::End();
}

//----------------------------------------------------------------------------------------------

TCena TPainelRayCasting::MontaCena() const
{
  const double azimute = azimuteGraus * PI / 180.0;
  const double elevacao = elevacaoGraus * PI / 180.0;

  // Câmera sobre a esfera de raio distancia em torno do alvo, com y para cima.
  const TVetor3D posicao = alvo + TVetor3D {
    distancia * std::cos(elevacao) * std::sin(azimute),
    distancia * std::sin(elevacao),
    distancia * std::cos(elevacao) * std::cos(azimute)
  };

  TCena cena;
  cena.camera = { posicao, alvo, { 0.0, 1.0, 0.0 }, 45.0 };

  // A luz acompanha a câmera, um pouco acima dela, para que a face vista esteja sempre iluminada.
  cena.luz = { posicao + TVetor3D { 0.0, distancia, 0.0 }, { 1.0, 1.0, 1.0 } };
  cena.material = { { 0.85, 0.45, 0.2 }, 0.15, 0.7, 0.3, 32.0 };
  cena.corDeFundo = { 0.055, 0.075, 0.11 };
  cena.alcance = distancia + 2.0 * raioDaCena + 1.0;

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

void TPainelRayCasting::DesenhaAramado(
  const TCamera& camera,
  const ImVec2& origem
) const
{
  const ImVec2 fim(origem.x + largura, origem.y + altura);
  const TVetor3D fundo = MontaCena().corDeFundo;

  ImDrawList* desenho = ImGui::GetWindowDrawList();
  desenho->PushClipRect(origem, fim, true);
  desenho->AddRectFilled(
    origem,
    fim,
    ImGui::ColorConvertFloat4ToU32(ImVec4(
      static_cast<float>(fundo.x),
      static_cast<float>(fundo.y),
      static_cast<float>(fundo.z),
      1.0f
    ))
  );

  const auto naTela = [&](const TVetor3D& ponto) {
    const TPontoImagem projetado = ProjetaPonto(camera, ponto, largura, altura);
    return ImVec2(
      origem.x + static_cast<float>(projetado.coluna),
      origem.y + static_cast<float>(projetado.linha)
    );
  };

  for (const TAresta& segmento : aramado) {
    const std::optional<TAresta> visivel = RecortaAFrenteDaCamera(camera, segmento);
    if (visivel) {
      desenho->AddLine(
        naTela(visivel->Inicio()),
        naTela(visivel->Fim()),
        IM_COL32(225, 225, 230, 255),
        1.5f
      );
    }
  }

  desenho->PopClipRect();
}

//----------------------------------------------------------------------------------------------
