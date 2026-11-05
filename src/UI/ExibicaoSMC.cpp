#include "ExibicaoSMC.h"

#include <optional>
#include <utility>

#include "Core/CaixaEnvolvente.h"
#include "Core/ClassificadorCSG.h"

namespace
{
  constexpr float ESPESSURA_LINHA = 3.0f;
  constexpr float RAIO_EXTREMIDADE = 4.0f;

  //--------------------------------------------------------------------------------------------

  TVetor3D ParaVetor(
    const std::array<float, 3>& coordenadas
  )
  {
    return { coordenadas[0], coordenadas[1], coordenadas[2] };
  }

  //--------------------------------------------------------------------------------------------

  ImU32 CorDaClasse(
    EClassificacao classificacao
  )
  {
    switch (classificacao) {
      case EClassificacao::IN:
        return IM_COL32(60, 210, 90, 255);

      case EClassificacao::ON:
        return IM_COL32(255, 210, 40, 255);

      case EClassificacao::OUT:
        return IM_COL32(230, 60, 60, 255);
    }

    return IM_COL32(255, 255, 255, 255);
  }

  //--------------------------------------------------------------------------------------------

  const char* NomeDaClasse(
    EClassificacao classificacao
  )
  {
    switch (classificacao) {
      case EClassificacao::IN:
        return "IN";

      case EClassificacao::ON:
        return "ON";

      case EClassificacao::OUT:
        return "OUT";
    }

    return "?";
  }

  //--------------------------------------------------------------------------------------------

  void DesenhaItemDaLegenda(
    EClassificacao classificacao
  )
  {
    const ImVec2 posicao = ImGui::GetCursorScreenPos();
    const float lado = ImGui::GetTextLineHeight();

    ImGui::GetWindowDrawList()->AddRectFilled(
      posicao,
      ImVec2(posicao.x + lado, posicao.y + lado),
      CorDaClasse(classificacao)
    );

    ImGui::Dummy(ImVec2(lado, lado));
    ImGui::SameLine();
    ImGui::TextUnformatted(NomeDaClasse(classificacao));
  }
}

//----------------------------------------------------------------------------------------------

TExibicaoSMC::TExibicaoSMC() :
  precisaClassificar(true),
  visivel(true)
{
}

//----------------------------------------------------------------------------------------------

void TExibicaoSMC::DefineArvore(
  TArvoreCSG novaArvore
)
{
  arvore = std::move(novaArvore);
  precisaClassificar = true;
  arestas.clear();

  if (!arvore) {
    return;
  }

  // Cada aresta atravessa a caixa do modelo e passa um quarto dela de cada lado.
  const TCaixa caixa = CaixaEnvolvente(arvore);
  const TVetor3D centro = Centro(caixa);
  const TVetor3D minimo = caixa.minimo - 0.25 * (caixa.maximo - caixa.minimo);
  const TVetor3D maximo = caixa.maximo + 0.25 * (caixa.maximo - caixa.minimo);

  const auto ponto = [](double x, double y, double z) {
    return std::array<float, 3> { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
  };

  arestas.push_back({ ponto(minimo.x, centro.y, centro.z), ponto(maximo.x, centro.y, centro.z) });
  arestas.push_back({ ponto(centro.x, minimo.y, centro.z), ponto(centro.x, maximo.y, centro.z) });
  arestas.push_back({ ponto(centro.x, centro.y, minimo.z), ponto(centro.x, centro.y, maximo.z) });
}

//----------------------------------------------------------------------------------------------

void TExibicaoSMC::DesenhaControles()
{
  ImGui::SeparatorText("Classificação SMC");
  ImGui::Checkbox("Exibir arestas sobre a imagem", &visivel);

  DesenhaItemDaLegenda(EClassificacao::IN);
  ImGui::SameLine();
  DesenhaItemDaLegenda(EClassificacao::ON);
  ImGui::SameLine();
  DesenhaItemDaLegenda(EClassificacao::OUT);

  const std::vector<TClassificacaoAresta>& resultado = Classificacoes();

  std::optional<std::size_t> removida;
  for (std::size_t i = 0; i < arestas.size(); ++i) {
    ImGui::PushID(static_cast<int>(i));

    ImGui::Text("Aresta %zu", i + 1);
    ImGui::SameLine();
    if (ImGui::SmallButton("Remover")) {
      removida = i;
    }

    precisaClassificar |= ImGui::InputFloat3("Início", arestas[i].inicio.data(), "%.2f");
    precisaClassificar |= ImGui::InputFloat3("Fim", arestas[i].fim.data(), "%.2f");

    // Trechos em ordem de t, cada um com a cor da sua classe.
    if (i < resultado.size()) {
      for (const TSegmentoClassificado& segmento : resultado[i]) {
        ImGui::TextColored(
          ImGui::ColorConvertU32ToFloat4(CorDaClasse(segmento.classificacao)),
          "  t em [%.3f, %.3f]: %s",
          segmento.tInicio,
          segmento.tFim,
          NomeDaClasse(segmento.classificacao)
        );
      }
    }

    ImGui::PopID();
  }

  if (removida) {
    arestas.erase(arestas.begin() + static_cast<std::ptrdiff_t>(*removida));
    precisaClassificar = true;
  }

  if (ImGui::Button("Adicionar aresta")) {
    arestas.push_back({ { -3.0f, 0.0f, 0.0f }, { 3.0f, 0.0f, 0.0f } });
    precisaClassificar = true;
  }
}

//----------------------------------------------------------------------------------------------

void TExibicaoSMC::DesenhaSobreImagem(
  const TCamera& camera,
  const ImVec2& origem,
  int largura,
  int altura
)
{
  if (!visivel) {
    return;
  }

  ImDrawList* desenho = ImGui::GetWindowDrawList();
  desenho->PushClipRect(origem, ImVec2(origem.x + largura, origem.y + altura), true);

  const auto naTela = [&](const TVetor3D& ponto) {
    const TPontoImagem projetado = ProjetaPonto(camera, ponto, largura, altura);
    return ImVec2(
      origem.x + static_cast<float>(projetado.coluna),
      origem.y + static_cast<float>(projetado.linha)
    );
  };

  const std::vector<TClassificacaoAresta>& resultado = Classificacoes();
  for (std::size_t i = 0; i < arestas.size() && i < resultado.size(); ++i) {
    const TAresta aresta(ParaVetor(arestas[i].inicio), ParaVetor(arestas[i].fim));

    for (const TSegmentoClassificado& segmento : resultado[i]) {
      const TAresta trecho(aresta.Ponto(segmento.tInicio), aresta.Ponto(segmento.tFim));
      const std::optional<TAresta> visivelNaCamera = RecortaAFrenteDaCamera(camera, trecho);
      if (!visivelNaCamera) {
        continue;
      }

      desenho->AddLine(
        naTela(visivelNaCamera->Inicio()),
        naTela(visivelNaCamera->Fim()),
        CorDaClasse(segmento.classificacao),
        ESPESSURA_LINHA
      );
    }

    // Marca as extremidades para indicar onde a aresta começa e termina.
    for (const TVetor3D& extremidade : { aresta.Inicio(), aresta.Fim() }) {
      if (RecortaAFrenteDaCamera(camera, TAresta(extremidade, extremidade))) {
        desenho->AddCircleFilled(naTela(extremidade), RAIO_EXTREMIDADE, IM_COL32(255, 255, 255, 255));
      }
    }
  }

  desenho->PopClipRect();
}

//----------------------------------------------------------------------------------------------

const std::vector<TClassificacaoAresta>& TExibicaoSMC::Classificacoes()
{
  if (precisaClassificar) {
    classificacoes.clear();
    if (arvore) {
      for (const TArestaEditavel& aresta : arestas) {
        classificacoes.push_back(
          ClassificaAresta(arvore, TAresta(ParaVetor(aresta.inicio), ParaVetor(aresta.fim)))
        );
      }
    }

    precisaClassificar = false;
  }

  return classificacoes;
}

//----------------------------------------------------------------------------------------------
