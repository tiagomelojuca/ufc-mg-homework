# Modelagem Geométrica (MG)

## Slide 1 — Unidade 0: Apresentação Geral da Disciplina

Parte 2: Introdução à Modelagem Geométrica

Joaquim Bento Cavalcante Neto  
joaquimb@lia.ufc.br

Grupo de Computação Gráfica, Realidade Virtual e Animação (CRAb)  
Departamento de Computação (DC)  
Universidade Federal do Ceará (UFC)

## Slide 2 — Roteiro

- Conceitos de modelagem
- Modelagem de sólidos
  - Modelos de decomposição
  - Modelos de construção
  - Modelos de fronteira
- Modelagem de curvas
- Modelagem de superfícies
- Conclusões

## Slide 3 — Conceitos de modelagem

## Slide 4 — Definição de modelo

- O que são modelos?
  - Objetos artificialmente construídos
  - Facilitam a análise de fenômenos, situações

## Slide 5 — Definição de modelo

- O que são modelos?
  - Objetos artificialmente construídos
  - Facilitam a análise de fenômenos, situações
- Quais são os tipos de modelos?
  - Modelos físicos - prédios, navios, carros
  - Modelos moleculares - arranjo de átomos
  - Modelos matemáticos - equações e dados

## Slide 6 — Definição de modelo

- O que são modelos?
  - Objetos artificialmente construídos
  - Facilitam a análise de fenômenos, situações
- Quais são os tipos de modelos?
  - Modelos físicos - prédios, navios, carros
  - Modelos moleculares - arranjo de átomos
  - Modelos matemáticos - equações e dados
- Para que usar modelos?
  - Estudo de características de coisas reais
  - Simulação do comportamento de coisas reais

## Slide 7 — Modelos computacionais

- Definição
  - Dados armazenados no computador
  - Podem representar vários tipos de modelos

## Slide 8 — Modelos computacionais

- Definição
  - Dados armazenados no computador
  - Podem representar vários tipos de modelos
- Modelagem geométrica
  - Modelagem para resolver problemas geométricos
  - Responde a perguntas do tipo:
    - a) que parte do corpo é visível para o usuário?
    - b) qual cor é associada a cada elemento?
  - Exemplos: curvas, superfícies e sólidos
  - Trata da representação (não renderização)

## Slide 9 — Modelos computacionais

- Modelagem de sólidos
  - Braço da modelagem geométrica
  - Trata de coisas completas, fechadas
  - Responde às questões “algoritmicamente”

## Slide 10 — Modelos computacionais

- Modelagem de sólidos
  - Braço da modelagem geométrica
  - Trata de coisas completas, fechadas
  - Responde às questões “algoritmicamente”
- Níveis de abstração em modelagem
  - Físico - sólido propriamente dito
  - Contínuo - representação matemática
  - Representação - armazenamento (ptos, coefs, …)
  - Implementação - código, estrutura de dados

## Slide 11 — Modelagem de sólidos

## Slide 12 — Modelos computacionais

- Classificação dos tipos de modelos
  - Modelos de decomposição
    - Uso de primitivas básicas (cubos, etc…)
    - Sólido descrito através de operações de “gluing”

## Slide 13 — Modelos computacionais

- Classificação dos tipos de modelos
  - Modelos de decomposição
    - Uso de primitivas básicas (cubos, etc…)
    - Sólido descrito através de operações de “gluing”
  - Modelos de fronteira
    - Uso de hierarquia (sólido, faces, arestas, etc…)
    - Sólido descrito através de seu contorno

## Slide 14 — Modelos computacionais

- Classificação dos tipos de modelos
  - Modelos de decomposição
    - Uso de primitivas básicas (cubos, etc…)
    - Sólido descrito através de operações de “gluing”
  - Modelos de fronteira
    - Uso de hierarquia (sólido, faces, arestas, etc…)
    - Sólido descrito através de seu contorno
  - Modelos de construção
    - Uso de primitivas básicas mais elaboradas (cone, etc…)
    - Sólido descrito através de operações de construção

## Slide 15 — Modelos de decomposição

## Slide 16 — Tipos de modelos

- Enumeração exaustiva
  - Primitiva básica - cubos de mesmo tamanho
  - Usadas em visualização volumétrica (voxels), etc.

## Slide 17 — Tipos de modelos

- Enumeração exaustiva
  - Primitiva básica - cubos de mesmo tamanho
  - Usadas em visualização volumétrica (voxels), etc.
- Decomposição celular
  - Primitiva básica - qualquer célula (triângulo, quadrilátero, etc.)
  - Usadas em simulações numéricas (MEF), etc.

## Slide 18 — Tipos de modelos

- Enumeração exaustiva
  - Primitiva básica - cubos de mesmo tamanho
  - Usadas em visualização volumétrica (voxels), etc.
- Decomposição celular
  - Primitiva básica - qualquer célula (triângulo, quadrilátero, etc.)
  - Usadas em simulações numéricas (MEF), etc.
- Subdivisão espacial
  - Primitiva básica - cubos de tamanho variável
  - Usadas em modelagem propriamente dita

## Slide 19 — Subdivisão espacial

- Quadtrees
- Célula vazia

## Slide 20 — Subdivisão espacial

- Quadtrees
- Célula cheia

## Slide 21 — Subdivisão espacial

- Quadtrees
- Célula cheia

## Slide 22 — Subdivisão espacial

- Quadtrees
- Célula parcial

## Slide 23 — Subdivisão espacial

- Octree

## Slide 24 — Subdivisão espacial

- Octree

## Slide 25 — Características

- Baixa precisão, porque são aproximadas
- Geram modelos válidos
- Não é ambíguo e a representação é única
- Não é conciso (árvore com muitas células)
- Realiza operações fechadas (Booleanas)
- Útil para modelagem auxiliar (buscar, localizar, etc.)

## Slide 26 — Modelos de fronteira

## Slide 27 — Tipos de modelos

- Baseados em polígonos
  - Lista de faces

## Slide 28 — Tipos de modelos

- Baseados em polígonos
  - Lista de faces
- Baseados em vértices
  - Lista de vértices

## Slide 29 — Tipos de modelos

- Baseados em polígonos
  - Lista de faces
- Baseados em vértices
  - Lista de vértices
- Baseados em arestas
  - Aresta “alada” (winged-edge) - Wed
  - Meia aresta (half-edge) - Hed

## Slide 30 — Lista de faces

## Slide 31 — Lista de faces

## Slide 32 — Lista de faces

## Slide 33 — Lista de faces

## Slide 34 — Lista de faces

## Slide 35 — Winged-edge

## Slide 36 — Half-edge

## Slide 37 — Half-edge

## Slide 38 — Half-edge

## Slide 39 — Características

- Precisão muito alta, representação eficiente
- Geram modelos válidos
- Não é ambíguo e a representação é única
- Não é muito conciso (Hed é grande, etc.)
- Poderoso para modelagens complexas

## Slide 40 — Modelos de construção

## Slide 41 — Tipos de modelos

- Modelos de semi-espaço
  - Primitiva básica - semi-espaços (semi-espaço planar, semi-espaço cilíndrico, etc.)
  - O modelo é definido pela combinação dos semi-espaços em uma árvore por op. Booleanas

## Slide 42 — Tipos de modelos

- Modelos de semi-espaço
  - Primitiva básica - semi-espaços (semi-espaço planar, semi-espaço cilíndrico, etc.)
  - O modelo é definido pela combinação dos semi-espaços em uma árvore por op. Booleanas
- Modelos CSG (Constructive Solid Geometry)
  - Primitiva básica - quaisquer objetos construídos a partir de uma combinação de semi-espaços
  - O modelo é definido pela combinação das primitivas em uma árvore usando op. Booleanas

## Slide 43 — CSG

## Slide 44 — CSG

## Slide 45 — CSG

## Slide 46 — CSG

## Slide 47 — CSG

## Slide 48 — CSG

## Slide 49 — CSG

## Slide 50 — CSG

## Slide 51 — CSG

## Slide 52 — CSG

## Slide 53 — Características

- Precisão depende das primitivas, se existirem muitas primitivas a precisão pode ser bem grande
- Podem gerar modelos não-válidos
- Não é ambíguo e a representação não é única
- É bem mais conciso que as demais, mas em modelagens práticas tende a crescer
- Uma modelagem por CSG pode ser bem complexa, dependendo do problema

## Slide 54 — Conclusões

## Slide 55 — Conclusões

- Modelar NÃO é somente usar um software

## Slide 56 — Conclusões

- Modelar NÃO é somente usar um software
- Modelagem trata da representação do modelo

## Slide 57 — Conclusões

- Modelar NÃO é somente usar um software
- Modelagem trata da representação do modelo
- O tipo de modelagem que se aplica a um caso específico depende de vários fatores:
  - precisão desejada
  - memória disponível
  - custo computacional

## Slide 58 — Conclusões

- Modelar NÃO é somente usar um software
- Modelagem trata da representação do modelo
- O tipo de modelagem que se aplica a um caso específico depende de vários fatores:
  - precisão desejada
  - memória disponível
  - custo computacional
- Modelagem é um passo fundamental para aplicações em computação gráfica e várias outras áreas de aplicação

## Slide 59 — Modelagem de curvas

## Slide 60 — Spline

- Uma spline é uma curva paramétrica definida por pontos de controle.
- O termo vem da área de desenho em engenharia, onde uma spline é um pedaço de madeira ou metal flexível usado para desenhar curvas suaves.
- Pontos de controle são ajustados pelo usuário para controlar a forma da curva usando-se pesos.

## Slide 61 — Bézier

- Segmento de curva definido por 4 ptos de controle.
- As funções de base são sempre positivas e sua soma é sempre 1 (polinômios de Bernstein). Por causa disso, curva está localizada no fecho convexo dos seus pontos de controle.

## Slide 62 — B-splines Uniformes Não-racionais

- B-splines, ao contrário das splines naturais, tem controle local, isto é, a mudança de um ponto de controle só afeta alguns segmentos da curva.
- B-splines cúbicas aproximam (ao contrário das splines naturais) uma série de m+1 pontos de controle P0, ..., Pm, m ≤ 3, por uma curva com m-2 segmentos de curvas polinomiais cúbicos Q3, ..., Qm.
- O termo uniforme significa que os nós estão espaçados em intervalos iguais do parâmetro t.

## Slide 63 — B-splines Uniformes Não-racionais

## Slide 64 — B-splines Não-uniformes Não-racionais

- Intervalos dos parâmetros t não precisam ser iguais.
- Vantagens sobre as uniformes:
  - Continuidade em pontos de junção é selecionada.
  - Pontos iniciais e finais podem ser interpolados mais facilmente.
  - Mais controle na sua modificação.

## Slide 65 — B-splines Não-uniformes Racionais (NURBS)

- Segmentos de curvas cúbicas racionais são razões de polinômios:

```text
x(t) = X(t)/W(t), y(t) = Y(t)/W(t), z(t) = Z(t)/W(t)
```

- X(t), X(t), X(t), X(t), são curvas polinomiais cúbicas com pontos de controle especificados em coordenadas homogêneas.
- São invariantes a rotação, translação, scaling e transformações perspectivas.
- Conseguem representar cônicas precisamente (importante em CAD).

## Slide 66 — Modelagem de superfícies

## Slide 67 — Superfícies Bicúbicas Paramétricas

- Superfícies bicúbicas paramétricas são generalizações de curvas cúbicas paramétricas.

## Slide 68 — Superfícies de Bézier

- Definida por 16 pontos de controle.

## Slide 69 — Superfícies de B-splines

- As equações para superfícies B-splines são obtidas de modo similar.
- Em superfícies B-splines, deve-se evitar pontos de controle duplicados, que criam discontinuidades.
- Superfícies de B-splines bicúbicas não-uniformes e racionais são análogas a suas formas cúbicas.
