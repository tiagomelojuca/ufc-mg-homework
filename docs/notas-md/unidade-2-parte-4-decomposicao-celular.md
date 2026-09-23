# Modelagem Geométrica (MG)

## Slide 1 — Unidade 2: Modelagem de Sólidos por Decomposição

Parte 4: Decomposição Celular: Introdução e Conceitos

Joaquim Bento Cavalcante Neto  
joaquimb@dc.ufc.br

Grupo de Computação Gráfica, Realidade Virtual e Animação (CRAb)  
Departamento de Computação (DC)  
Universidade Federal do Ceará (UFC)

## Slide 2 — Decomposição Celular

- Introdução:
  - Abordagem semelhante à enumeração exaustiva
  - Primitiva básica: elementos básicos (não cubos)
  - Possui uma variedade de tipos de células básicas 🙂
  - Células podem ser quaisquer objetos sem buracos 🙂
  - Existem algumas restrições a serem respeitadas ☹
    - Células não podem se sobrepor
    - Células devem ser disjuntas
    - Interseções só em suas entidades:
      - Vértices
      - Arestas
      - Faces

## Slide 3 — Decomposição Celular

- Introdução:

Uma célula quádrica  
Uma Decomposição Celular

## Slide 4 — Decomposição Celular

- Usos:

## Slide 5 — Decomposição Celular

- Conceitos:
  - A precisão depende do número e grau dos elementos
  - As variações 2D e 3D são principalmente usadas em Simulações e Métodos Numéricos (não Modelagem)
  - Aplicações especiais, onde é necessário a simulação:
    - Simulação pelo Método dos Elementos Finitos
  - O sólido é gerado a partir da divisão do seu espaço
    - Necessários técnicas de subdivisão (geração de malhas)

## Slide 6 — Decomposição Celular

- Descrição:
  - Geram uma subdivisão (discreta) do espaço modelado
  - Essa subdivisão pode ser regular ou mesmo irregular
  - Essa subdivisão gera elementos discretos (básicos)
  - Ela é conhecida normalmente como malha de elementos
  - Existem várias técnicas para geração dessas malhas:
    - Delaunay
    - Advancing Front
    - Quadtrees, etc
    - Dentre outras

## Slide 7 — Decomposição Celular

- Algoritmos:
  - Geração de malha
    - Métodos Malhas Tri/Tet
      - Triângulos (2D)
      - Tetraedros (3D)
    - Métodos Malhas Quad/Hex
      - Quadriláteros (2D)
      - Hexaedros (3D)
    - Métodos Malhas Híbridas
      - Vários elementos

## Slide 8 — Decomposição Celular

- Algoritmos:

## Slide 9 — Decomposição Celular

- Métodos Malhas Tri/Tet:
  - Delaunay/Voronoi
    - Qualidade assegurada
    - Dificuldade na fronteira
  - Avanço de Fronteira
    - Robustez garantida
    - Facilidade na fronteira
  - Quadtree e Octree
    - Performance elevada
    - Uso de padrões definidos

## Slide 10 — Decomposição Celular

- Deulanay: para um conjunto de pontos P no plano é uma triangulação DT(P) onde nenhum ponto em P está dentro da circunferência formada por qualquer triângulo na DT(P).

## Slide 11 — Decomposição Celular

- Métodos Malhas Tri/Tet:
  - Delaunay/Voronoi

a

## Slide 12 — Decomposição Celular

- Métodos Malhas Tri/Tet:
  - Avanço de Fronteira

C  
D  
r  
A  
B

## Slide 13 — Decomposição Celular

- Métodos Malhas Tri/Tet:
  - Quadtree e Octree

## Slide 14 — Decomposição Celular

- Métodos Malhas Quad/Hex:

| Estruturada | Não-estruturada |
| --- | --- |
| Requer que geometria se conforme com requisitos | Não há requisitos específicos para a geometria do modelo |
| Padrões regulares para os quads/hexs pela geometria | Os quads/hexes são usados para se adaptar à geometria |

## Slide 15 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Algoritmo

- Interpolação Transfinita (TFI)
- Mapeia os quads para o polígono

Requisitos Geométricos

- 4 lados topológicos
- Lados opostos devem ser iguais

Mapeamento 2D

## Slide 16 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- 6 superfícies topológicas
- Superfícies opostas devem ter malhas semelhantes

Mapeamento 3D

## Slide 17 — Decomposição Celular

- Métodos Malhas Hexa/Penta

## Slide 18 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 19 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

linking surfaces  
target  
source

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 20 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 21 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 22 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 23 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 24 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 25 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Estruturada

Requisitos Geométricos

- Superfícies inicial e final devem ter mesma topologia
- Superfícies de ligação mapable ou submapable

Sweeping

## Slide 26 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Não-estruturada

Costuras (Seams)  
Paving

- Avanço de fronteira: começa de uma fronteira
- Forma linhas de elementos usando ângulos
- Deve ter um número de lados pares no início

## Slide 27 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Não-estruturada

Plastering

- Extensão 3D do algoritmo de “paving”
- Linha por linha ou elemento por elemento

## Slide 28 — Decomposição Celular

- Métodos Malhas Quad/Hex:
  - Não-estruturada

Divisão de tetraedros (Tetrahedra splitting)

- Cada tetraedro é dividido em 4 hexaedros
- Geralmente resulta em elementos ruins

## Slide 29 — Decomposição Celular

- Métodos Malhas Híbridas:

Elementos diferentes

## Slide 30 — Decomposição Celular

- Adaptive mesh refinement: método de adaptação da precisão de uma solução dentro de certas regiões sensíveis ou turbulentas de simulação, dinamicamente
- Maior economia computacional em relação a uma abordagem de grade estática (malhas otimizadas)

## Slide 31 — Decomposição Celular

- Usos:
  - Modelagem Geométrica (raro)

## Slide 32 — Decomposição Celular

- Usos:
  - Simulação numérica (comum)

## Slide 33 — Decomposição Celular

- Usos:
  - Simulação numérica (comum)

## Slide 34 — Decomposição Celular

- Meshfree methods: não requerem conexão entre os nós do domínio de simulação, ou seja, uma malha
- Massa/energia cinética, não são mais atribuídas aos elementos da malha, mas sim aos nós
- Se a malha degenerar durante a simulação, os operadores definidos nela podem não fornecer mais valores corretos

## Slide 35 — Decomposição Celular

- Propriedades:
  - Poder de Expressão:
    - Gera modelos de precisão alta, pois usam elementos variados
  - Validade:
    - Gera sempre modelos válidos, pois são sempre malhas
  - Ambiguidade:
    - Não gera modelos ambíguos, por usar sempre malhas
  - Unicidade:
    - Pode gerar modelos diferentes por sua própria descrição (uso de malhas variáveis faz com que isso possa acontecer)

## Slide 36 — Decomposição Celular

- Propriedades:
  - Linguagem de descrição:
    - Em geral é dado por um conjunto de elementos discretos
  - Concisão:
    - Não gera modelos concisos (pode usar milhões de elementos)
  - Operações fechadas:
    - Suporta Operações Booleanas mas podem ser complexos
  - Custo computacional:
    - Se for usado para modelagem pode ser bastante caro
  - Aplicabilidade:
    - Mais usado em simulações computacionais e outras áreas

## Slide 37 — Decomposição Celular

- Propriedades:
  - Linguagem de descrição:
    - Em geral é dado por um conjunto de elementos discretos
  - Concisão:
    - Não gera modelos concisos (pode usar milhões de elementos)
  - Operações fechadas:
    - Suporta Operações Booleanas mas podem ser complexos
  - Custo computacional:
    - Se for usado para modelagem pode ser bastante caro
  - Aplicabilidade:
    - Mais usado em simulações computacionais e outras áreas
