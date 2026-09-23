# Modelagem Geométrica (MG)

## Slide 1 — Unidade 2: Modelagem de Sólidos por Decomposição

Parte 3: Subdivisão Espacial: Implementação Octree

Joaquim Bento Cavalcante Neto  
joaquimb@dc.ufc.br

Grupo de Computação Gráfica, Realidade Virtual e Animação (CRAb)  
Departamento de Computação (DC)  
Universidade Federal do Ceará (UFC)

## Slide 2 — Subdivisão Espacial

- Modelador de Octree:
  - Gerador da árvore: cria a octree a partir da parametrização das primitivas fornecida
  - Operações Booleanas: calcula uma nova octree a partir da união, interseção e diferença (espaços idênticos)
  - Operações geométricas: calcula uma nova octree como resultado de uma translação, rotação, escala, perspectiva
  - Procedimentos de análise: calcula a área superficial, volume do objeto a partir da octree construída
  - Gerador de imagem: cria uma imagem gráfica a partir da estrutura de dados representada pela octree construída

## Slide 3 — Subdivisão Espacial

- Gerador da árvore
  - O espaço é subdividido usando uma mediana em cada eixo, obtendo-se oito subespaços (octree => oito filhos)
  - O nó raiz representa o espaço maior, cada subespaço é representado por um filho (cada filho é 1/8 do seu pai)
  - Se o espaço de cada filho possui um número maior de elementos que o permitido, também é subdividido
  - As regras de subdivisão podem ser relaxadas para gerar objetos mais compactos mas com uma menor precisão

## Slide 4 — Subdivisão Espacial

- Limitantes
  - Número de objetos interceptados
  - Profundidade máxima da árvore
- Cada nó pode ser classificado em dois tipos
  - Internos:
    - Contém a posição dos subespaços dos nós
  - Folhas:
    - Contém lista de objetos interceptados pelos nós

## Slide 5 — Subdivisão Espacial

- Estrutura:

```text
root
level 0
level 1
level 2
point list
```

Notation

- non-empty node
- empty node
- point list

Features of an octree leaf node

## Slide 6 — Subdivisão Espacial

- Para cada tipo de primitiva, é necessário que seja usado um método de classificação contra um nó arbitrário
  - O nó está completamente fora da primitiva?
  - O nó está completamente dentro da primitiva?
  - O nó intercepta a primitiva?
- O método de classificação é usado de maneira recursiva
- Essa classificação é que determina se haverá subdivisão

## Slide 7 — Subdivisão Espacial

- Recursão
  - O nó inicial é marcado como branco
  - Cada nó é classificado contra as primitivas
    - Se estiver fora deixe-o branco, termine a recursão
    - Se estiver dentro marque-o como preto, termine a recursão
    - Se interceptar, marque-o como cinza e subdivida-o
  - A subdivisão é feita até encontrar um limitante

## Slide 8 — Subdivisão Espacial

- Níveis da árvore:

```text
Raiz
Nível 1
Nível 2
```

## Slide 9 — Subdivisão Espacial

- Representação
  - Uma maneira comum de descrevermos uma Octree é através de uma representação DF (Depth First, profundidade primeiro). Por exemplo, usando B para blocos cheios (Black), W para blocos vazios (White) e ( para blocos parciais poderíamos descrever a figura apresentada abaixo ficaria com a seguinte representação:

```text
(BWWBBW(BWWBBBWWB
```

## Slide 10 — Subdivisão Espacial

- Algoritmo:

```cpp
void constroi_OCT(Tsolido s, Toctree *filho, GLint profundidade) {
  /*montar arvore*/
  Toctree *pai;
  char estado;
  int i;
  pai=filho;
  estado='B';
  if (profundidade>1)
    estado=classifica_solido(s, (*pai).coordenadas, (*pai).lado);
  (*pai).estado=estado;
  if (estado=='('){
    subdivide(pai);
    for (i=0;i<8;i++)
      constroi_OCT(e, (*pai).filhos[i],profundidade-1);
  }
}
```

## Slide 11 — Subdivisão Espacial

- Operações Booleanas
  - Toma duas octrees como entrada e produz uma saída
  - As árvores de entrada são percorridas em ordem sincronizada, de cima para baixo, subdividindo os nós quando necessário

Operações Booleanas

```text
(c) = (a) ∪ (b)
(c) = (a) ∩ (b)
```

## Slide 12 — Subdivisão Espacial

- Exemplo: interseção (comparando os nós n1 e n2)
  - n1 e n2 são ambos folhas:

```text
B ∩ B = B
B ∩ W = W
W ∩ B = W
W ∩ W = W
```

## Slide 13 — Subdivisão Espacial

- Exemplo: interseção (comparando os nós n1 e n2)
  - n1 é folha, mas n2 não é:

n1  
n2

```text
B ∩ ( = (
W ∩ ( = W
```

## Slide 14 — Subdivisão Espacial

- Exemplo: interseção (comparando os nós n1 e n2)
  - n1 é folha, mas n2 não é:

n1  
n2

- Se nenhum dos dois for folha -> recursão!
- Outras operações são similares à interseção
- União, Diferença, etc...

## Slide 15 — Subdivisão Espacial

- Procedimentos de análise podem ser calculados usando um algoritmo de percurso (funciona melhor para os volumes):
  - Octree => bom para estimativa de volumes
  - Octree => ruim para estimativa de áreas
- Operações geométricas: calcula uma nova octree como resultado de uma translação, rotação, escala, perspectiva
  - Usa mesmas matrizes de transformação
- Geração de imagem:
  - Simplesmente mostrar as folhas
  - Mostrar só as arestas dos nós
  - Exemplo de técnica: Ray Tracing

## Slide 16 — Subdivisão Espacial

- Algoritmo - busca:

1. Comece com o nó raiz como o nó atual.
2. Se o ponto fornecido não estiver no limite representado pelo nó atual, pare a pesquisa com erro.
3. Determine o nó filho apropriado para armazenar o ponto.
4. Se o nó filho for um nó vazio, retorne FALSE.
5. Se o nó filho for um nó folha e corresponder ao ponto fornecido, retorne TRUE, caso contrário, retorne FALSE.
6. Se o nó filho for um nó interno, defina o nó atual como o nó filho. Vá para o passo 2.

## Slide 17 — Subdivisão Espacial

- Ray tracing
  - Técnica de renderização para modelos 3D
  - Simula a iluminação existente na cena
  - Segue os raios de luz através da cena

Ray Tracing

## Slide 18 — Subdivisão Espacial

- Problemas
  - Deve calcular a interseção do raio com cada objeto presente na tela (usa contribuição de cada objeto)
  - Deve calcular a intersecção com cada raio
  - Requer muito tempo de processamento
  - Técnica para melhorar a eficiência: Octree-R

## Slide 19 — Subdivisão Espacial

- Ray Tracing usando Octree
- É um processo em duas etapas
  - Identificar na vizinhança, o próximo nó a ser visitado
  - Localizar o nó vizinho através de algum tipo de operação
- Esse processo é chamado de octree traversal scheme

## Slide 20 — Subdivisão Espacial

- Raio transversal a octree:

## Slide 21 — Subdivisão Espacial

- Raio transversal a octree:

## Slide 22 — Subdivisão Espacial

- Modelo de Custo para o ray tracing usando octree
  - nv: número de nós pelos quais o raio passou
  - nt: número de intersecções raio-objeto
  - Tv: tempo para o raio ir de um nó a outro
  - Ti: tempo do teste de intersecção raio-objeto
- Custo do ray tracing (custo total estimado)

```text
custo = nv.Tv + nt.Ti
```

## Slide 23 — Subdivisão Espacial

- Modelo de Custo para o ray tracing usando octree

```text
custo = nv.Tv + nt.Ti
Assumption: nv ∝ total_voxels ∝ memory
```

## Slide 24 — Subdivisão Espacial

- Suponha a construção de uma octree pela subdivisão repetitiva de um dado espaço (de forma recursiva)
- Considere duas octrees:
  - Uma com N nós
  - Outra com menos de N nós

## Slide 25 — Subdivisão Espacial

- Quanto mais nós, mais o espaço foi subdividido
- O número de objetos que intercepta cada nó é menor aqui (consequentemente, nt diminui)
- Em compensação o número de nós pelo qual o raio passa aumenta (consequetemente, nv sobe)
- Em resumo: nv e nt têm uma relação inversa!!!

## Slide 26 — Subdivisão Espacial

- Logo, é razoável comparar duas octrees com o mesmo número de nós (octrees semelhantes)
- Assim, a que tiver menor nt terá menor custo
- Para um dado espaço (com um mesmo número de nós) podem existir diversas subdivisões
- A otimização está em encontrar uma subdivisão que reduza o número de intersecções raio-objeto

## Slide 27 — Subdivisão Espacial

- Octree-R:
  - Possui a mesma estrutura da octree convencional
  - O espaço não é subdividido pela mediana
    - É subdividido entre a mediana dos objetos e a mediana espacial (McDonald e Roth)
  - Plano de divisão reduz número de interseções:
    - Interseções raio-objeto
  - Ray-tracing usando Octree-R é melhor do que ray-tracing usando uma Octree convencional

## Slide 28 — Subdivisão Espacial

- Algoritmo iterativo, paralelo, baseado em busca em largura e octree traversal:
