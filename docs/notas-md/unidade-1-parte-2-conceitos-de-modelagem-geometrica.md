# Modelagem Geométrica (MG)

## Slide 1 — Unidade 1: Noções de Modelagem Geométrica

Parte 2: Conceitos de Modelagem Geométrica

Joaquim Bento Cavalcante Neto  
joaquimb@lia.ufc.br

Grupo de Computação Gráfica, Realidade Virtual e Animação (CRAb)  
Departamento de Computação (DC)  
Universidade Federal do Ceará (UFC)

## Slide 2 — Modelagem de Sólidos

- Ramo da modelagem geométrica:
  - Trata da aplicabilidade geral dos sólidos
  - Criação somente de modelos fechados
- Modelos gráficos estão mais voltados ao desenho do objeto do que próprio objeto (sem estrutura interna)
- Modelos de superfícies são muito específicos
- Vale ressaltar que os métodos para modelagem em geral têm bases na modelagem de sólidos

## Slide 3 — Modelagem de Sólidos

- O que é um sólido?
  - Um conjunto tridimensional de pontos
  - Conjuntos de pontos podem ser descritos
    - Por suas fronteiras
    - Por campos escalares:
      - Definidos por equações
      - Dados por amostragem
  - Originam três tipos de representação:
    - Por enumeração do espaço em células (BSP-trees, Octrees, etc)
    - Operações de conjuntos (CSG – Constructive Solid Geometry)
    - Por definição da fronteira (B-rep – Boundary Representation)

## Slide 4 — Modelagem de Sólidos

- Problemas:
  - Completude
  - Integridade
  - Complexidade e Cobertura Geométrica
  - Natureza da Computação Geométrica

## Slide 5 — Modelagem de Sólidos

- Completude:
  - Informações obtidas da geometria devem ser suficientes para responder perguntas geométricas arbitrárias
  - O modelo de um sólido não deve gerar ambigüidades
  - Modelos gráficos (representados freqüentemente em wireframes) são ambíguos (aresta da frente e de trás?)

## Slide 6 — Modelagem de Sólidos

- Integridade:
  - “Qual parte do modelo é vista pelo usuário?”
    - Remova as faces ocultas (hidden-face removal)
  - Como?
    - Usando modelos poliédricos (em geral)
    - São construídos por primitivas geométricas
  - Surge um problema:
    - Os algoritmos de remoção de faces ocultas supõem que as primitivas não se sobrepõem (nem sempre isso é verdade)
    - Como garantir a restrição?

## Slide 7 — Modelagem de Sólidos

- Complexidade e Cobertura geométrica:
  - É uma necessidade da indústria trabalhar com modelos que tenham cobertura geométrica precisa
  - A complexidade da construção de um modelo poliédrico está relacionada com a integridade
  - Modelos simples podem ter milhares de polígonos
  - Gerar as informações sobre a cobertura geométrica manualmente é difícil, chato e propenso a erros

## Slide 8 — Modelagem de Sólidos

- Natureza da Computação Geométrica:
  - Modelos de sólidos devem responder algoritmicamente às perguntas de natureza geométrica (sem o usuário)
  - O resultado dessas perguntas pode ser uma imagem, um número ou uma constante booleana
  - As operações de um modelador devem lidar com essas respostas mantendo o sistema fechado, correto e válido

## Slide 9 — Modelagem de Sólidos

- Perguntas de natureza geométrica (exemplos):
  - Como o objeto se parece?
  - Qual a área superficial do objeto?
  - Como o objeto se comporta ao colidir com outro?
  - O objeto é forte o suficiente para carregar tal carga?
  - O que é preciso para fabricar esse objeto?

## Slide 10 — Modelagem de Sólidos

- Características desejáveis em um modelo:
  - O domínio deve ser grande o suficiente para a descrição
  - A representação deve ser não-ambígua (completa), única (não representa dois sólidos), compacta e precisa
  - Não permitir a criação de representações inválidas
  - Facilidade na criação de representações válidas
  - Permitir o uso de algoritmos para cálculo de propriedades físicas e geração de imagens

## Slide 11 — Modelagem de Sólidos

- A combinação de objetos para a criação de outros é uma ferramenta poderosa (muito útil em modelagem)
- Aplicar uma operação Booleana em dois sólidos válidos não gera necessariamente um sólido válido

Interseção gerando sólidos inválidos

## Slide 12 — Modelagem de Sólidos

- Utiliza-se operações regularizadas (união, etc)
- Essas operações sempre produzem um sólido válido

Operações Booleanas Regularizadas

## Slide 13 — Modelagem de Sólidos

- A regularização de um conjunto é o fechamento dos pontos interiores ao conjunto (garantir validade)
- Em interseções regularizadas, bordas só são incluídas se dois objetos estiverem do mesmo lado da porção da borda compartilhada (elimina irregularidades)
- Isso pode ser feito usando-se normais das superfícies

## Slide 14 — Modelagem de Sólidos

- Classificação dos tipos de modelos sólidos:
  - Modelos de Decomposição:
    - Primitivas básicas (quadrado e cubo, por exemplo)
    - Sólidos descritos por operações de “gluing” (colagem)

## Slide 15 — Modelagem de Sólidos

- Classificação dos tipos de modelos sólidos:
  - Modelos de Decomposição:
    - Primitivas básicas (quadrado e cubo, por exemplo)
    - Sólidos descritos por operações de “gluing” (colagem)
  - Modelos de Construção:
    - Primitivas mais elaboradas (esfera, cilindro, etc)
    - Sólido descrito por operações Booleanas (união, etc)

## Slide 16 — Modelagem de Sólidos

- Classificação dos tipos de modelos sólidos:
  - Modelos de Decomposição:
    - Primitivas básicas (quadrado e cubo, por exemplo)
    - Sólidos descritos por operações de “gluing” (colagem)
  - Modelos de Construção:
    - Primitivas mais elaboradas (esfera, cilindro, etc)
    - Sólido descrito por operações Booleanas (união, etc)
  - Modelos de Fronteira:
    - Uso da hierarquia (sólido, face, aresta, vértice, etc)
    - Sólido descrito por operações no contorno (Euler)

## Slide 17 — Modelagem de Sólidos

- Propriedades das técnicas de modelagem:
  - Poder de Expressão:
    - A técnica consegue representar vários tipos de sólidos? (exemplo: consegue representar fogo, fumaça, etc...???)
  - Validade:
    - A técnica sempre consegue gerar modelos válidos?
  - Ambiguidade:
    - A técnica sempre consegue gerar modelos não-ambíguos? (significando que cada combinação determina mesmo sólido)
  - Unicidade:
    - A técnica sempre consegue gerar modelos que são únicos? (significando que cada sólido não pode ser representado por combinações diferentes, isto é, mais de uma representação)

## Slide 18 — Modelagem de Sólidos

- Propriedades das técnicas de modelagem:
  - Linguagem de descrição:
    - A técnica usa que tipo de descrição para os sólidos?
  - Concisão:
    - A representação dos modelos gerados é concisa?
  - Operações fechadas:
    - A técnica suporta conjunto de operações fechadas? (Operações Booleanas, por exemplo, são exemplos)
  - Custo computacional:
    - Qual o custo computacional para gerar os modelos?
  - Aplicabilidade:
    - A técnica é mais aplicável para que tipos de modelos?

## Slide 19 — Modelagem de Curvas e Superfícies

- Ramo da modelagem geométrica:
  - Trata da aplicabilidade de curvas e superfícies
  - Criação somente da borda dos modelos

## Slide 20 — Modelagem de Curvas e Superfícies

- Representação:
  - Explícita: duas variáveis, a dependente e a independente:
    - Obtida uma em função da outra (para o caso de uma curva)
    - y = f(x); ou z = f(x,y) (para o caso de uma superfície)

## Slide 21 — Modelagem de Curvas e Superfícies

- Representação:
  - Explícita: duas variáveis, a dependente e a independente:
    - Obtida uma em função da outra (para o caso de uma curva)
    - y = f(x); ou z = f(x,y) (para o caso de uma superfície)
  - Implícita: uma curva ou superfície pode ser representada implicitamente na forma f(x,y) = 0 onde a função f é uma função de teste membership (pertence ou não à entidade)

## Slide 22 — Modelagem de Curvas e Superfícies

- Representação:
  - Explícita: duas variáveis, a dependente e a independente:
    - Obtida uma em função da outra (para o caso de uma curva)
    - y = f(x); ou z = f(x,y) (para o caso de uma superfície)
  - Implícita: uma curva ou superfície pode ser representada implicitamente na forma f(x,y) = 0 onde a função f é uma função de teste membership (pertence ou não à entidade)
  - Paramétrica: representa o valor de cada variável (x, y, z) no espaço em função de um variável independente u
    - x = x(u); y = y(u); z = z(u)
