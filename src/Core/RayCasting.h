#ifndef RAYCASTING_H_
#define RAYCASTING_H_

#include <optional>
#include <vector>

#include "ArvoreCSG.h"
#include "Classificacao.h"
#include "Vetor3D.h"

//----------------------------------------------------------------------------------------------

struct TIntersecao
{
  double t;
  TVetor3D ponto;

  // Normal exterior unitária da fronteira no ponto.
  TVetor3D normal;
};

//----------------------------------------------------------------------------------------------

// Normal exterior da fronteira da árvore no ponto, que deve estar sobre ela.
TVetor3D NormalNoPonto(
  const TArvoreCSG& arvore,
  const TVetor3D& ponto
);

// Primeira interseção do raio com a árvore. O raio é uma aresta que parte do observador e vai até
// além da cena, classificada por Tree_M; a interseção é o início do primeiro trecho não OUT. Não há
// interseção se o raio não toca o sólido ou se parte de dentro dele.
std::optional<TIntersecao> PrimeiraIntersecao(
  const TArvoreCSG& arvore,
  const TAresta& raio
);

//----------------------------------------------------------------------------------------------

struct TCamera
{
  TVetor3D posicao;
  TVetor3D alvo;
  TVetor3D cima;
  double campoDeVisaoGraus;
};

//----------------------------------------------------------------------------------------------

// Cores com componentes em [0, 1].
struct TMaterial
{
  TVetor3D cor;
  double coeficienteAmbiente;
  double coeficienteDifuso;
  double coeficienteEspecular;
  double expoenteBrilho;
};

//----------------------------------------------------------------------------------------------

struct TLuz
{
  TVetor3D posicao;
  TVetor3D cor;
};

//----------------------------------------------------------------------------------------------

struct TCena
{
  TCamera camera;
  TLuz luz;
  TMaterial material;
  TVetor3D corDeFundo;

  // Comprimento dos raios: deve passar de toda a cena vista da câmera.
  double alcance;
};

//----------------------------------------------------------------------------------------------

// Imagem com quatro bytes por pixel (RGBA), linha a linha a partir do topo.
struct TImagem
{
  int largura;
  int altura;
  std::vector<unsigned char> pixels;
};

//----------------------------------------------------------------------------------------------

// Raio da câmera que passa pelo centro do pixel (coluna, linha), com a linha 0 no topo.
TAresta RaioDoPixel(
  const TCamera& camera,
  double alcance,
  int coluna,
  int linha,
  int largura,
  int altura
);

// Posição na imagem, em pixels, com a origem no canto superior esquerdo.
struct TPontoImagem
{
  double coluna;
  double linha;
};

// Inverso de RaioDoPixel: onde o ponto aparece na imagem. O ponto deve estar à frente da câmera.
TPontoImagem ProjetaPonto(
  const TCamera& camera,
  const TVetor3D& ponto,
  int largura,
  int altura
);

// Parte da aresta à frente da câmera, que pode ser projetada; vazio se a aresta fica toda atrás.
std::optional<TAresta> RecortaAFrenteDaCamera(
  const TCamera& camera,
  const TAresta& aresta
);

// Modelo de Phong: ambiente, difusa pelo ângulo entre normal e luz, especular pelo ângulo entre
// a reflexão da luz e o observador.
TVetor3D Ilumina(
  const TMaterial& material,
  const TLuz& luz,
  const TVetor3D& ponto,
  const TVetor3D& normal,
  const TVetor3D& observador
);

TImagem GeraImagem(
  const TArvoreCSG& arvore,
  const TCena& cena,
  int largura,
  int altura
);

//----------------------------------------------------------------------------------------------

#endif
