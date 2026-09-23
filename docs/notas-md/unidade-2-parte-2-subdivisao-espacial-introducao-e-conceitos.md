# Modelagem Geométrica (MG)

## Slide 1 — Unidade 2: Modelagem de Sólidos por Decomposição

Parte 2: Subdivisão Espacial: Introdução e Conceitos

Joaquim Bento Cavalcante Neto  
joaquimb@dc.ufc.br

Grupo de Computação Gráfica, Realidade Virtual e Animação (CRAb)  
Departamento de Computação (DC)  
Universidade Federal do Ceará (UFC)

## Slide 2 — Subdivisão Espacial

- Introdução:
  - Subdivisão feita de maneira adaptativa
  - Perde-se mais tempo dividindo o espaço ☹
  - Área mais densa é mais subdividida 🙂
  - Poupa-se mais memória 🙂
  - Propriedade fundamental: o número de nós é proporcional à área de superfície
    - Isso acontece porque a subdivisão só é necessária para representar a borda do objeto sendo codificado
  - O sólido é gerado a partir da divisão do seu espaço

## Slide 3 — Subdivisão Espacial

- Enumeração exaustiva x Subdivisão espacial:

a) Enumeração exaustiva  
b) Subdivisão espacial

## Slide 4 — Subdivisão Espacial

- Principais estruturas de subdivisão espacial:
  - Árvores (Quadtree, Octree, etc)
  - Subdivisão Binária do Espaço
  - Ambas usam uma mediana espacial
  - Mediana espacial?
    - Aproximadamente metade do intervalo disponível em cada eixo considerado

## Slide 5 — Subdivisão Espacial

- Exemplo de uso de árvore (quadtree):

## Slide 6 — Subdivisão Espacial

- Quadtree:

```text
2 3
0 1
```

- Observação: árvore depende da convenção usada (0,1,2,3)

## Slide 7 — Subdivisão Espacial

- Octree:
- Observação: árvore depende da convenção usada (0...7)

A figura identifica os eixos `X`, `Y` e `Z`, o universo, o objeto, os octantes numerados de `0` a `7` e a árvore correspondente.

## Slide 8 — Subdivisão Espacial

- Octree:
- Observação:
  - Mesmo modelo
  - Diferente octree
  - Qual a convenção?

A figura apresenta o mesmo universo e objeto com outra numeração de regiões (`1`, `2`, `3` e `4`) e outra árvore correspondente.

## Slide 9 — Subdivisão Espacial

- Octree:

## Slide 10 — Subdivisão Espacial

- Octree:

## Slide 11 — Subdivisão Espacial

- Octree:

## Slide 12 — Subdivisão Espacial

- Conceitos:
  - É uma estrutura de dados hierárquica
  - Mostra como os objetos estão distribuídos na cena
  - Principais usos:
    - Modelagem de sólidos
    - Processamento de imagens

## Slide 13 — Subdivisão Espacial

- Modelador de Octree:

## Slide 14 — Subdivisão Espacial

- Modelador de Octree:
  - Gerador da árvore: cria a octree a partir da parametrização das primitivas fornecidas

## Slide 15 — Subdivisão Espacial

- Modelador de Octree:
  - Gerador da árvore: cria a octree a partir da parametrização das primitivas fornecidas
  - Operações Booleanas: calcula uma nova octree a partir da união, interseção e diferença (espaços idênticos)

## Slide 16 — Subdivisão Espacial

- Modelador de Octree:
  - Gerador da árvore: cria a octree a partir da parametrização das primitivas fornecidas
  - Operações Booleanas: calcula uma nova octree a partir da união, interseção e diferença (espaços idênticos)
  - Operações geométricas: calcula uma nova octree como resultado de uma translação, rotação, escala, perspectiva

## Slide 17 — Subdivisão Espacial

- Modelador de Octree:
  - Gerador da árvore: cria a octree a partir da parametrização das primitivas fornecidas
  - Operações Booleanas: calcula uma nova octree a partir da união, interseção e diferença (espaços idênticos)
  - Operações geométricas: calcula uma nova octree como resultado de uma translação, rotação, escala, perspectiva
  - Procedimentos de análise: calcula a área superficial, volume do objeto a partir da octree construída

## Slide 18 — Subdivisão Espacial

- Modelador de Octree:
  - Gerador da árvore: cria a octree a partir da parametrização das primitivas fornecidas
  - Operações Booleanas: calcula uma nova octree a partir da união, interseção e diferença (espaços idênticos)
  - Operações geométricas: calcula uma nova octree como resultado de uma translação, rotação, escala, perspectiva
  - Procedimentos de análise: calcula a área superficial, volume do objeto a partir da octree construída
  - Gerador de imagem: cria uma imagem gráfica a partir da estrutura de dados representada pela octree construída

## Slide 19 — Subdivisão Espacial

- Octree-R:
  - Possui a mesma estrutura da octree convencional
  - O espaço não é subdividido pela mediana
    - É subdividido entre a mediana dos objetos e a mediana espacial (McDonald e Roth)
  - Plano de divisão reduz número de interseções:
    - Interseções raio-objeto

## Slide 20 — Subdivisão Espacial

- Subdivisão Binária do Espaço (BSP):
  - Uma alternativa à octree
  - Um nó cinza é dividido em duas metades
    - A subdivisão é executada sucessivamente nas direções x,y,z
  - Comparando à octree, a subdivisão binária é menor

## Slide 21 — Subdivisão Espacial

- BSP - Exemplo:

```text
{1, 2, 3, 4, 5, 6}
```

Conjunto de polígonos

## Slide 22 — Subdivisão Espacial

- BSP- Exemplo:

Seleciona um polígono e particiona o espaço e os polígonos

## Slide 23 — Subdivisão Espacial

- BSP- Exemplo:

Particiona cada sub-árvore até todos os polígonos serem percorridos

## Slide 24 — Subdivisão Espacial

- BSP- Exemplo 2:

## Slide 25 — Subdivisão Espacial

- BSP:
  - Dividem recursivamente o espaço em pares de subespaço
  - Os pares são separados por:
    - um plano de orientação e posição arbitrárias
  - Originalmente usado para determinar superfícies visíveis
  - Cada nó interno de uma BSP é associado com um plano e tem um ponteiro para cada lado do plano
  - Pode representar um sólido côncavo arbitrário como uma união de regiões convexas

## Slide 26 — Subdivisão Espacial

- Kd-tree:
  - árvore binária na qual cada nó é um ponto k -dimensional
- Diferenças:

```text
Uniform Spatial Sub | Quadtree/Octree | kd-tree | BSP-tree
```

## Slide 27 — Subdivisão Espacial

- kdtree - Exemplo:

```text
A (40, 45)
B (15, 70)
C (70, 10)
D (69, 50)
E (66, 85)
F (85, 90)
```

A figura apresenta a subdivisão espacial dos pontos `A` a `F` e a árvore com divisões alternadas em `x` e `y`.

## Slide 28 — Subdivisão Espacial

- Propriedades:
  - Poder de Expressão:
    - Gera modelos aproximados, mas melhores que com voxels
  - Validade:
    - Gera sempre modelos válidos, pois são sempre cubos
  - Ambiguidade:
    - Não gera modelos ambíguos, por usar sempre cubos
  - Unicidade:
    - Gera sempre modelos únicos por sua própria descrição (caso seja usada mesma profundidade e mesma convenção)
    - Caso contrário, a árvore pode variar para mesmo modelo

## Slide 29 — Subdivisão Espacial

- Propriedades:
  - Linguagem de descrição:
    - Em geral é dado por uma árvore (quadtrees, octrees, etc...)
  - Concisão:
    - Não gera modelos concisos mas melhor do que com voxels
  - Operações fechadas:
    - A própria definição da técnica suporta Operações Booleanas
  - Custo computacional:
    - Mais barato do que usar voxels, mas ainda um pouco caro
  - Aplicabilidade:
    - Muito usado em modelagem e como estruturas auxiliares
    - Por exemplo, são muitos usados como estruturas de busca

