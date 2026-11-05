#include "RayCasting.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "ClassificadorCSG.h"

namespace
{
  constexpr double PI = 3.14159265358979323846;

  //--------------------------------------------------------------------------------------------

  TVetor3D Multiplica(
    const TVetor3D& a,
    const TVetor3D& b
  )
  {
    return { a.x * b.x, a.y * b.y, a.z * b.z };
  }

  //--------------------------------------------------------------------------------------------

  // Base ortonormal da câmera e meias dimensões do plano de imagem a distância 1 dela.
  struct TReferencialCamera
  {
    TVetor3D frente;
    TVetor3D direita;
    TVetor3D cima;
    double meiaLargura;
    double meiaAltura;
  };

  //--------------------------------------------------------------------------------------------

  TReferencialCamera MontaReferencial(
    const TCamera& camera,
    int largura,
    int altura
  )
  {
    const TVetor3D frente = Normaliza(camera.alvo - camera.posicao);
    const TVetor3D direita = Normaliza(ProdutoVetorial(frente, camera.cima));
    const TVetor3D cima = ProdutoVetorial(direita, frente);

    const double meiaAltura = std::tan(camera.campoDeVisaoGraus * PI / 360.0);
    const double meiaLargura = meiaAltura * largura / altura;

    return { frente, direita, cima, meiaLargura, meiaAltura };
  }

  //--------------------------------------------------------------------------------------------

  unsigned char ComponenteEmByte(
    double componente
  )
  {
    return static_cast<unsigned char>(std::lround(std::clamp(componente, 0.0, 1.0) * 255.0));
  }
}

//----------------------------------------------------------------------------------------------

TVetor3D NormalNoPonto(
  const TArvoreCSG& arvore,
  const TVetor3D& ponto
)
{
  switch (arvore->Tipo()) {
    case ETipoNoCSG::PRIMITIVA:
      return arvore->Primitiva().Normal(ponto);

    case ETipoNoCSG::MOVIMENTO_RIGIDO: {
      const TMovimentoRigido& movimento = arvore->Movimento();
      const TVetor3D normalNoFilho =
        NormalNoPonto(arvore->Filho(), movimento.LevaAoSistemaDoFilho(ponto));
      return Normaliza(movimento.MatrizInversa().TransformaNormalPelaTransposta(normalNoFilho));
    }

    case ETipoNoCSG::ESCALA: {
      const TEscala& escala = arvore->Escala();
      const TVetor3D normalNoFilho =
        NormalNoPonto(arvore->Filho(), escala.LevaAoSistemaDoFilho(ponto));
      return Normaliza(escala.MatrizInversa().TransformaNormalPelaTransposta(normalNoFilho));
    }

    case ETipoNoCSG::OPERACAO_BOOLEANA: {
      if (arvore->Operacao() != EOperacaoBooleana::UNIAO) {
        throw std::logic_error("Operação booleana ainda não implementada");
      }

      // Na fronteira da união, o ponto está na fronteira de um filho e não no interior do outro.
      const bool sobreEsquerda =
        ClassificaPonto(arvore->Esquerda(), ponto) == EClassificacao::ON &&
        ClassificaPonto(arvore->Direita(), ponto) != EClassificacao::IN;

      return NormalNoPonto(sobreEsquerda ? arvore->Esquerda() : arvore->Direita(), ponto);
    }
  }

  throw std::logic_error("Tipo de nó desconhecido");
}

//----------------------------------------------------------------------------------------------

std::optional<TIntersecao> PrimeiraIntersecao(
  const TArvoreCSG& arvore,
  const TAresta& raio
)
{
  for (const TSegmentoClassificado& segmento : ClassificaAresta(arvore, raio)) {
    if (segmento.classificacao == EClassificacao::OUT) {
      continue;
    }

    if (segmento.tInicio <= TOLERANCIA_GEOMETRICA) {
      return std::nullopt;
    }

    const TVetor3D ponto = raio.Ponto(segmento.tInicio);
    return TIntersecao { segmento.tInicio, ponto, NormalNoPonto(arvore, ponto) };
  }

  return std::nullopt;
}

//----------------------------------------------------------------------------------------------

TAresta RaioDoPixel(
  const TCamera& camera,
  double alcance,
  int coluna,
  int linha,
  int largura,
  int altura
)
{
  const TReferencialCamera referencial = MontaReferencial(camera, largura, altura);

  // Coordenadas do centro do pixel no plano de imagem a distância 1 da câmera.
  const double x = (2.0 * (coluna + 0.5) / largura - 1.0) * referencial.meiaLargura;
  const double y = (1.0 - 2.0 * (linha + 0.5) / altura) * referencial.meiaAltura;

  const TVetor3D direcao =
    Normaliza(referencial.frente + x * referencial.direita + y * referencial.cima);

  return TAresta(camera.posicao, camera.posicao + alcance * direcao);
}

//----------------------------------------------------------------------------------------------

TPontoImagem ProjetaPonto(
  const TCamera& camera,
  const TVetor3D& ponto,
  int largura,
  int altura
)
{
  const TReferencialCamera referencial = MontaReferencial(camera, largura, altura);
  const TVetor3D relativo = ponto - camera.posicao;

  // Divisão perspectiva: o ponto é levado ao plano de imagem a distância 1 da câmera.
  const double profundidade = ProdutoEscalar(relativo, referencial.frente);
  const double x = ProdutoEscalar(relativo, referencial.direita) / profundidade;
  const double y = ProdutoEscalar(relativo, referencial.cima) / profundidade;

  return {
    (x / referencial.meiaLargura + 1.0) * largura / 2.0,
    (1.0 - y / referencial.meiaAltura) * altura / 2.0
  };
}

//----------------------------------------------------------------------------------------------

std::optional<TAresta> RecortaAFrenteDaCamera(
  const TCamera& camera,
  const TAresta& aresta
)
{
  // Profundidade mínima que evita a divisão por valores próximos de zero na projeção.
  constexpr double PROFUNDIDADE_MINIMA = 1e-3;

  const TVetor3D frente = Normaliza(camera.alvo - camera.posicao);
  const double profundidadeInicio = ProdutoEscalar(aresta.Inicio() - camera.posicao, frente);
  const double profundidadeFim = ProdutoEscalar(aresta.Fim() - camera.posicao, frente);

  const bool inicioAFrente = profundidadeInicio >= PROFUNDIDADE_MINIMA;
  const bool fimAFrente = profundidadeFim >= PROFUNDIDADE_MINIMA;

  if (inicioAFrente && fimAFrente) {
    return aresta;
  }

  if (!inicioAFrente && !fimAFrente) {
    return std::nullopt;
  }

  // A profundidade varia linearmente com t; o corte fica onde ela vale o mínimo.
  const double t =
    (PROFUNDIDADE_MINIMA - profundidadeInicio) / (profundidadeFim - profundidadeInicio);
  const TVetor3D corte = aresta.Ponto(t);

  return inicioAFrente ? TAresta(aresta.Inicio(), corte) : TAresta(corte, aresta.Fim());
}

//----------------------------------------------------------------------------------------------

TVetor3D Ilumina(
  const TMaterial& material,
  const TLuz& luz,
  const TVetor3D& ponto,
  const TVetor3D& normal,
  const TVetor3D& observador
)
{
  const TVetor3D paraLuz = Normaliza(luz.posicao - ponto);
  const TVetor3D paraObservador = Normaliza(observador - ponto);

  const double difusa = std::max(0.0, ProdutoEscalar(normal, paraLuz));

  // Reflexão da direção da luz em torno da normal: r = 2 (n · l) n - l.
  double especular = 0.0;
  if (difusa > 0.0) {
    const TVetor3D reflexao = 2.0 * ProdutoEscalar(normal, paraLuz) * normal - paraLuz;
    especular = std::pow(
      std::max(0.0, ProdutoEscalar(reflexao, paraObservador)),
      material.expoenteBrilho
    );
  }

  const TVetor3D corDifusa = Multiplica(material.cor, luz.cor);

  return material.coeficienteAmbiente * material.cor +
    material.coeficienteDifuso * difusa * corDifusa +
    material.coeficienteEspecular * especular * luz.cor;
}

//----------------------------------------------------------------------------------------------

TImagem GeraImagem(
  const TArvoreCSG& arvore,
  const TCena& cena,
  int largura,
  int altura
)
{
  if (largura <= 0 || altura <= 0) {
    throw std::invalid_argument("As dimensões da imagem devem ser positivas");
  }

  TImagem imagem { largura, altura, std::vector<unsigned char>(4u * largura * altura) };

  for (int linha = 0; linha < altura; ++linha) {
    for (int coluna = 0; coluna < largura; ++coluna) {
      const TAresta raio = RaioDoPixel(cena.camera, cena.alcance, coluna, linha, largura, altura);
      const std::optional<TIntersecao> intersecao = PrimeiraIntersecao(arvore, raio);

      const TVetor3D cor = intersecao
        ? Ilumina(
            cena.material,
            cena.luz,
            intersecao->ponto,
            intersecao->normal,
            cena.camera.posicao
          )
        : cena.corDeFundo;

      unsigned char* pixel = &imagem.pixels[4u * (linha * largura + coluna)];
      pixel[0] = ComponenteEmByte(cor.x);
      pixel[1] = ComponenteEmByte(cor.y);
      pixel[2] = ComponenteEmByte(cor.z);
      pixel[3] = 255;
    }
  }

  return imagem;
}

//----------------------------------------------------------------------------------------------
