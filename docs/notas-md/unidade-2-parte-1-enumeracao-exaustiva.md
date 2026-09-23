# Modelagem Geométrica (MG)

## Slide 1 — Unidade 2: Modelagem de Sólidos por Decomposição

Parte 1: Enumeração Exaustiva: Introdução e Conceitos

Joaquim Bento Cavalcante Neto  
joaquimb@dc.ufc.br

Grupo de Computação Gráfica, Realidade Virtual e Animação (CRAb)  
Departamento de Computação (DC)  
Universidade Federal do Ceará (UFC)

## Slide 2 — Modelagem por Decomposição

- Descrevem sólidos pela combinação de blocos colados
- O tipo de bloco utilizado leva a diferentes abordagens:
  - Enumeração Exaustiva
  - Subdivisão Espacial
  - Decomposição Celular
- A ideia base é a subdividir o volume de visão
- Como gerar a imagem?
  - Exibir interseções
  - Ray Tracing
  - Outros métodos

## Slide 3 — Modelagem por Decomposição

- Como gerar a imagem:

## Slide 4 — Modelagem por Decomposição

- Aplicações:
  - Visualização volumétrica
  - Detecção de colisões
  - Modelagem (raro)

## Slide 5 — Modelagem por Decomposição

- Enumeração Exaustiva:
  - Espaço é dividido em blocos iguais (voxels)
  - Sólido é gerado a partir da divisão do espaço
- Subdivisão Espacial:
  - Espaço é recursivamente dividido (trees)
  - Blocos podem ter tamanhos distintos
  - Sólido é gerado a partir da divisão do espaço
- Decomposição Celular:
  - Objeto é dividido em células irregulares (elementos)
  - Sólido é gerado a partir da divisão do objeto

## Slide 6 — Modelagem por Decomposição

- Enumeração Exaustiva
- Subdivisão Espacial
- Decomposição Celular

## Slide 7 — Enumeração Exaustiva

- Introdução:
  - O volume de visão é subdividido uniformemente
  - Primitiva básica: cubos não sobrepostos (voxels)
  - Eficaz ao identificar uma interseção no modelo 🙂
  - Só é preciso armazenar um dos vértices do voxel 🙂
  - Gasto de memória (uso de muitos voxels) ☹
  - Áreas vazias podem ter muitos voxels ☹

## Slide 8 — Enumeração Exaustiva

- Conceitos:
  - A precisão depende do número de voxels do modelo
  - As variações 2D e 3D são principalmente usadas em Processamento Digital de Imagens (não Modelagem)
  - Aplicações especiais, onde é necessário ver o interior:
    - Visualização Volumétrica
  - Serve como catalisador de outras representações:
    - Pode ser usado como representação intermediária em outros modelos como por exemplo modelos de semi-espaço e CSG
  - O sólido é gerado a partir da divisão do seu espaço

## Slide 9 — Enumeração Exaustiva

- Descrição:
  - Geram uma subdivisão regular do espaço modelado
  - Em geral somente um dos cantos deve ser armazenado
  - Para um espaço fixo de interesse, só é necessário um array 3D (`Cijk`) de dados binários e as coordenadas de cada um dos cantos dos voxels que geram o modelo:

```text
Cijk = 1, se o cubo ijk intercepta o sólido
Cijk = 0, se o cubo ijk é um cubo vazio
```

## Slide 10 — Enumeração Exaustiva

- Voxelização:
  - O voxel (volumetric pixel ou volumetric picture element) é um elemento de volume representando um valor em um grid regular no espaço de três dimensões (3D space)
  - É análogo ao texel, que representa dados de imagens 2D em um bitmap (muitas vezes referenciado como pixmap)
  - Assim como pixels em um bitmap, voxels muitas vezes não possuem suas posições explicitamente definidas, e sim são inferidas por seus centros ou suas vizinhanças
  - Voxels são frequentementes usados em visualização e por isso desenhos volumétricos usam voxels para descrever a sua resolução (por exemplo, resolução de 512 x 512 x 512)

## Slide 11 — Enumeração Exaustiva

- Voxelização:
  - A resolução de um modelo é dada pela quantidade de voxels existentes para a representação de um modelo

## Slide 12 — Enumeração Exaustiva

- Voxelização:
  - Um voxel representa um dado em um grid 3D regular
  - Esse dado pode ser simples, como opacidade ou pode ser múltiplo, como cor e outras propriedades adicionais
  - O voxel representa somente um ponto não o volume: o espaço entre voxels não é representado pelo voxels e se for necessário, os dados faltando devem ser interpolados
  - Apesar dos voxels fornecerem o benefício de precisão e de profundidade, são dados tipicamente muito grandes: entretanto, hoje em dia são melhor tratados em paralelo
  - Existem outros dados que podem ser armazenados em voxels e que são úteis como o vetor normal e as cores

## Slide 13 — Enumeração Exaustiva

- Voxelização:
  - A voxelização converte os objetos geométricos da sua representação geométrica contínua para um conjunto de voxels que melhor aproximam essa representação
  - É também conhecida como 3D scan-conversion porque é semelhante ao processo de rasterização de objetos em 2D
  - É mais difícil do que o caso 2D porque não existe uma sequência de voxels e também não uma adjacência fixa

Rasterização de curva em 2D  
Sequência e adjacência bem definidas

- Teoria que lida com isso é chamada 3D discrete topology

## Slide 14 — Enumeração Exaustiva

- Voxelização:
  - A 3D discrete topology define um espaço 3D discreto onde cada voxel é um cubo centrado em um ponto
  - O voxel é mapeado em {0, 1} sendo que 1 são voxels “pretos” que representam objetos “opacos” e 0 são “brancos” que representam objetos “transparentes”
  - Dois voxels podem ser 6, 18 ou 26 adjacentes, isto é, dependendo do número de voxels adjacentes a ele

```text
(1) => 6
(2) => 18
(3) => 26
```

## Slide 15 — Enumeração Exaustiva

- Voxelização:
  - Um volume contendo voxels pode ser visualizado por:
    - Extração de superfícies (ES)
    - Visualização direta de volumes (VD)
  - Essas técnicas diferem pela utilização ou não de alguma representação intermediária de dados para visualização:
    - Extração de superfícies representa o volume através de alguma aproximação poligonal encima dos voxels (Ex: Marching cubes)
    - Visualização direta de volumes exibe o interior dos modelos que devem ser visualizados e suas propriedades (Ex: Ray-casting)
  - Cada uma dessas técnicas tem vantagens e desvantagens dependendo do modelo que deseja-se ter a visualização

## Slide 16 — Enumeração Exaustiva

- Voxelização

## Slide 17 — Enumeração Exaustiva

- Usos:
  - Modelagem Geométrica (raro)

## Slide 18 — Enumeração Exaustiva

- Usos:
  - Modelagem Geométrica (raro)

## Slide 19 — Enumeração Exaustiva

- Usos:
  - Visualização Volumétrica (comum)

## Slide 20 — Enumeração Exaustiva

- Usos:
  - Visualização Volumétrica (comum)

## Slide 21 — Enumeração Exaustiva

- Propriedades:
  - Poder de Expressão:
    - Gera modelos de baixa precisão, pois são aproximados
  - Validade:
    - Gera sempre modelos válidos, pois são sempre cubos
  - Ambiguidade:
    - Não gera modelos ambíguos, por usar sempre cubos
  - Unicidade:
    - Gera sempre modelos únicos por sua própria descrição (uso de cubos de mesmo tamanho (voxels) garante isso)

## Slide 22 — Enumeração Exaustiva

- Propriedades:
  - Linguagem de descrição:
    - Em geral é dado por um array binário 3D que é simples
  - Concisão:
    - Não gera modelos concisos (pode usar milhões de voxels)
  - Operações fechadas:
    - A própria definição da técnica suporta Operações Booleanas
  - Custo computacional:
    - Se for usado para modelagem pode ser bastante caro
  - Aplicabilidade:
    - Mais usado em processamento de imagens e outras áreas
