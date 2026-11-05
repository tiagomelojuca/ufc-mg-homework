#include "ClassificadorCSG.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace
{
  constexpr double PI = 3.14159265358979323846;

  //--------------------------------------------------------------------------------------------

  EClassificacao Uniao(
    EClassificacao esquerda,
    EClassificacao direita
  )
  {
    if (esquerda == EClassificacao::IN || direita == EClassificacao::IN) {
      return EClassificacao::IN;
    }

    if (esquerda == EClassificacao::ON || direita == EClassificacao::ON) {
      return EClassificacao::ON;
    }

    return EClassificacao::OUT;
  }

  //--------------------------------------------------------------------------------------------

  // Trecho da aresta que contém o parâmetro t.
  const TSegmentoClassificado& SegmentoEm(
    const TClassificacaoAresta& classificacao,
    double t
  )
  {
    for (const TSegmentoClassificado& segmento : classificacao) {
      if (t >= segmento.tInicio && t <= segmento.tFim) {
        return segmento;
      }
    }

    throw std::logic_error("A classificação da aresta não cobre o parâmetro t");
  }

  //--------------------------------------------------------------------------------------------

  // Todos os parâmetros em que alguma das classificações muda, em ordem crescente e sem
  // repetições (dentro da tolerância), sempre incluindo 0 e 1.
  std::vector<double> CortesOrdenados(
    const TClassificacaoAresta& esquerda,
    const TClassificacaoAresta& direita
  )
  {
    std::vector<double> todos = { 0.0, 1.0 };
    for (const TClassificacaoAresta* classificacao : { &esquerda, &direita }) {
      for (const TSegmentoClassificado& segmento : *classificacao) {
        todos.push_back(std::clamp(segmento.tInicio, 0.0, 1.0));
        todos.push_back(std::clamp(segmento.tFim, 0.0, 1.0));
      }
    }

    std::sort(todos.begin(), todos.end());

    std::vector<double> cortes;
    for (double t : todos) {
      if (cortes.empty() || t - cortes.back() > TOLERANCIA_GEOMETRICA) {
        cortes.push_back(t);
      }
    }

    cortes.back() = 1.0;

    return cortes;
  }

  //--------------------------------------------------------------------------------------------

  // Arco de direções no plano perpendicular à aresta, de inicio até inicio + abertura (radianos).
  struct TArco
  {
    double inicio;
    double abertura;
  };

  //--------------------------------------------------------------------------------------------

  double AnguloNormalizado(
    double angulo
  )
  {
    const double resto = std::fmod(angulo, 2.0 * PI);
    return resto < 0.0 ? resto + 2.0 * PI : resto;
  }

  //--------------------------------------------------------------------------------------------

  // Interseção de dois arcos de abertura até PI, que é sempre um único arco; abertura nula indica
  // interseção vazia.
  TArco IntersectaArcos(
    const TArco& a,
    const TArco& b
  )
  {
    // Posição de b medida a partir do início de a; b pode também começar uma volta antes.
    const double deslocamento = AnguloNormalizado(b.inicio - a.inicio);

    TArco melhor = { a.inicio, 0.0 };
    for (double inicioB : { deslocamento, deslocamento - 2.0 * PI }) {
      const double inicio = std::max(0.0, inicioB);
      const double fim = std::min(a.abertura, inicioB + b.abertura);
      if (fim - inicio > melhor.abertura) {
        melhor = { a.inicio + inicio, fim - inicio };
      }
    }

    return melhor;
  }

  //--------------------------------------------------------------------------------------------

  // Projeta cada cone da vizinhança no plano perpendicular à aresta, obtendo um arco por cone.
  // A normal de uma face que contém a aresta é perpendicular a ela; o semiespaço n · v <= 0
  // corresponde ao semiplano oposto a n, um arco de abertura PI.
  std::vector<TArco> ArcosDaVizinhanca(
    const TVizinhanca& vizinhanca,
    const TVetor3D& direcao
  )
  {
    const TVetor3D eixo = (1.0 / Norma(direcao)) * direcao;

    // Base ortonormal (u, w) do plano perpendicular, a partir do eixo coordenado menos alinhado.
    const TVetor3D auxiliar =
      std::abs(eixo.x) < 0.5 ? TVetor3D{ 1.0, 0.0, 0.0 } : TVetor3D{ 0.0, 1.0, 0.0 };
    const TVetor3D produto = ProdutoVetorial(eixo, auxiliar);
    const TVetor3D u = (1.0 / Norma(produto)) * produto;
    const TVetor3D w = ProdutoVetorial(eixo, u);

    std::vector<TArco> arcos;
    for (const TConeVizinhanca& cone : vizinhanca) {
      if (cone.empty()) {
        continue;
      }

      TArco arco = {};
      for (std::size_t i = 0; i < cone.size(); ++i) {
        const double anguloNormal =
          std::atan2(ProdutoEscalar(cone[i], w), ProdutoEscalar(cone[i], u));
        const TArco semiplano = { anguloNormal + PI / 2.0, PI };
        arco = i == 0 ? semiplano : IntersectaArcos(arco, semiplano);
      }

      if (arco.abertura > TOLERANCIA_GEOMETRICA) {
        arcos.push_back(arco);
      }
    }

    return arcos;
  }

  //--------------------------------------------------------------------------------------------

  // Verifica se a união dos arcos cobre todas as direções em torno da aresta.
  bool CobreTodasAsDirecoes(
    std::vector<TArco> arcos
  )
  {
    if (arcos.empty()) {
      return false;
    }

    for (TArco& arco : arcos) {
      if (arco.abertura >= 2.0 * PI - TOLERANCIA_GEOMETRICA) {
        return true;
      }

      arco.inicio = AnguloNormalizado(arco.inicio);
    }

    std::sort(arcos.begin(), arcos.end(), [](const TArco& a, const TArco& b) {
      return a.inicio < b.inicio;
    });

    // Percorre a volta a partir do primeiro arco; os arcos repetidos uma volta adiante fecham o
    // círculo.
    const double partida = arcos.front().inicio;
    double alcance = partida;
    for (int volta = 0; volta < 2; ++volta) {
      for (const TArco& arco : arcos) {
        const double inicio = arco.inicio + volta * 2.0 * PI;
        if (inicio > alcance + TOLERANCIA_GEOMETRICA) {
          return false;
        }

        alcance = std::max(alcance, inicio + arco.abertura);
        if (alcance >= partida + 2.0 * PI - TOLERANCIA_GEOMETRICA) {
          return true;
        }
      }
    }

    return false;
  }

  //--------------------------------------------------------------------------------------------

  bool MesmoCone(
    const TConeVizinhanca& a,
    const TConeVizinhanca& b
  )
  {
    if (a.size() != b.size()) {
      return false;
    }

    for (std::size_t i = 0; i < a.size(); ++i) {
      if (Norma(a[i] - b[i]) > TOLERANCIA_GEOMETRICA) {
        return false;
      }
    }

    return true;
  }

  //--------------------------------------------------------------------------------------------

  bool MesmaVizinhanca(
    const TVizinhanca& a,
    const TVizinhanca& b
  )
  {
    if (a.size() != b.size()) {
      return false;
    }

    for (std::size_t i = 0; i < a.size(); ++i) {
      if (!MesmoCone(a[i], b[i])) {
        return false;
      }
    }

    return true;
  }

  //--------------------------------------------------------------------------------------------

  // Combina dois trechos de mesmo intervalo. Na união regularizada, um trecho ON dos dois lados
  // está no interior quando as vizinhanças, somadas, envolvem toda a aresta, como em duas faces
  // encostadas com normais opostas; caso contrário continua na fronteira.
  TSegmentoClassificado CombinaTrechos(
    EOperacaoBooleana operacao,
    const TSegmentoClassificado& esquerda,
    const TSegmentoClassificado& direita,
    const TVetor3D& direcao
  )
  {
    const EClassificacao classe =
      CombinaClassificacoes(operacao, esquerda.classificacao, direita.classificacao);

    TSegmentoClassificado resultado = { 0.0, 0.0, classe };
    if (classe != EClassificacao::ON) {
      return resultado;
    }

    if (esquerda.classificacao != EClassificacao::ON) {
      resultado.vizinhanca = direita.vizinhanca;
      return resultado;
    }

    if (direita.classificacao != EClassificacao::ON) {
      resultado.vizinhanca = esquerda.vizinhanca;
      return resultado;
    }

    // Sem a vizinhança de um dos lados não há como decidir; o trecho fica na fronteira.
    if (esquerda.vizinhanca.empty() || direita.vizinhanca.empty()) {
      return resultado;
    }

    // União das vizinhanças, sem repetir cones iguais, como os de faces coplanares do mesmo lado.
    resultado.vizinhanca = esquerda.vizinhanca;
    for (const TConeVizinhanca& cone : direita.vizinhanca) {
      const bool repetido = std::any_of(
        resultado.vizinhanca.begin(),
        resultado.vizinhanca.end(),
        [&cone](const TConeVizinhanca& outro) { return MesmoCone(cone, outro); }
      );
      if (!repetido) {
        resultado.vizinhanca.push_back(cone);
      }
    }

    if (CobreTodasAsDirecoes(ArcosDaVizinhanca(resultado.vizinhanca, direcao))) {
      return { 0.0, 0.0, EClassificacao::IN };
    }

    return resultado;
  }

  //--------------------------------------------------------------------------------------------

  // Leva as normais das vizinhanças do sistema do filho ao do pai.
  TClassificacaoAresta LevaVizinhancasAoSistemaDoPai(
    TClassificacaoAresta classificacao,
    const TMatriz4x4& inversa
  )
  {
    for (TSegmentoClassificado& segmento : classificacao) {
      for (TConeVizinhanca& cone : segmento.vizinhanca) {
        for (TVetor3D& normal : cone) {
          normal = Normaliza(inversa.TransformaNormalPelaTransposta(normal));
        }
      }
    }

    return classificacao;
  }
}

//----------------------------------------------------------------------------------------------

EClassificacao CombinaClassificacoes(
  EOperacaoBooleana operacao,
  EClassificacao esquerda,
  EClassificacao direita
)
{
  switch (operacao) {
    case EOperacaoBooleana::UNIAO:
      return Uniao(esquerda, direita);

    case EOperacaoBooleana::INTERSECAO:
    case EOperacaoBooleana::DIFERENCA:
      throw std::logic_error("Operação booleana ainda não implementada");
  }

  throw std::logic_error("Operação booleana desconhecida");
}

//----------------------------------------------------------------------------------------------

TClassificacaoAresta CombinaArestas(
  EOperacaoBooleana operacao,
  const TClassificacaoAresta& esquerda,
  const TClassificacaoAresta& direita,
  const TAresta& aresta
)
{
  const std::vector<double> cortes = CortesOrdenados(esquerda, direita);

  TClassificacaoAresta resultado;
  for (std::size_t i = 0; i + 1 < cortes.size(); ++i) {
    // Dentro de um trecho nenhuma das duas classificações muda; o ponto médio basta.
    const double meio = (cortes[i] + cortes[i + 1]) / 2.0;
    TSegmentoClassificado trecho = CombinaTrechos(
      operacao,
      SegmentoEm(esquerda, meio),
      SegmentoEm(direita, meio),
      aresta.Direcao()
    );
    trecho.tInicio = cortes[i];
    trecho.tFim = cortes[i + 1];

    // Funde trechos vizinhos de mesma classe e mesma vizinhança, mantendo a lista minimal.
    if (!resultado.empty() &&
        resultado.back().classificacao == trecho.classificacao &&
        MesmaVizinhanca(resultado.back().vizinhanca, trecho.vizinhanca)) {
      resultado.back().tFim = trecho.tFim;
    } else {
      resultado.push_back(trecho);
    }
  }

  return resultado;
}

//----------------------------------------------------------------------------------------------

EClassificacao ClassificaPonto(
  const TArvoreCSG& arvore,
  const TVetor3D& ponto
)
{
  switch (arvore->Tipo()) {
    case ETipoNoCSG::PRIMITIVA:
      return arvore->Primitiva().ClassificaPonto(ponto);

    case ETipoNoCSG::MOVIMENTO_RIGIDO:
      return ClassificaPonto(arvore->Filho(), arvore->Movimento().LevaAoSistemaDoFilho(ponto));

    case ETipoNoCSG::OPERACAO_BOOLEANA:
      return CombinaClassificacoes(
        arvore->Operacao(),
        ClassificaPonto(arvore->Esquerda(), ponto),
        ClassificaPonto(arvore->Direita(), ponto)
      );
  }

  throw std::logic_error("Tipo de nó desconhecido");
}

//----------------------------------------------------------------------------------------------

TClassificacaoAresta ClassificaAresta(
  const TArvoreCSG& arvore,
  const TAresta& aresta
)
{
  // O movimento rígido é afim: o t da aresta original é o mesmo no sistema do filho.
  switch (arvore->Tipo()) {
    case ETipoNoCSG::PRIMITIVA:
      return arvore->Primitiva().ClassificaAresta(aresta);

    case ETipoNoCSG::MOVIMENTO_RIGIDO:
      return LevaVizinhancasAoSistemaDoPai(
        ClassificaAresta(arvore->Filho(), arvore->Movimento().LevaAoSistemaDoFilho(aresta)),
        arvore->Movimento().MatrizInversa()
      );

    case ETipoNoCSG::OPERACAO_BOOLEANA:
      return CombinaArestas(
        arvore->Operacao(),
        ClassificaAresta(arvore->Esquerda(), aresta),
        ClassificaAresta(arvore->Direita(), aresta),
        aresta
      );
  }

  throw std::logic_error("Tipo de nó desconhecido");
}

//----------------------------------------------------------------------------------------------
