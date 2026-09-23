# Modelagem Geométrica (MG)

## Slide 1 — Unidade 1: Noções de Modelagem Geométrica

Parte 1: Introdução a Modelagem Geométrica

Joaquim Bento Cavalcante Neto  
joaquimb@lia.ufc.br

Grupo de Computação Gráfica, Realidade Virtual e Animação (CRAb)  
Departamento de Computação (DC)  
Universidade Federal do Ceará (UFC)

## Slide 2 — Definição de modelagem

- Modelagem:
  - Conjunto de métodos para descrever:
    - Formas e características de um objeto
  - Provê uma descrição ou modelo:
    - Mais analítico, matemático e abstrato que o real
  - Principais aplicações:
    - Sistemas CAD/CAM
    - Computação gráfica
    - Arte por computador
    - Simulação computacional
    - Aplicações de robótica
    - Animação computacional

## Slide 3 — Histórico de modelagem

- Primeiros passos: durante a Segunda Guerra Mundial
- Projeto Manhattan: modelar detonação nuclear
- Uma simulação utilizando 12 esferas duras:
  - Algoritmo de Monte Carlo

Random shots  
Algorithms  
Outcome

## Slide 4 — Histórico de modelagem

- Década de 50:
  - Pesquisas sobre desenho auxiliado por computador
  - Ferramentas bidimensionais para gerar caminhos de ferramentas em máquinas de controle numérico

## Slide 5 — Histórico de modelagem

- Década de 60:
  - Computação Gráfica Interativa
  - Modelagem por wireframes
  - Surgem CADs baseados em Mainframes
  - Principais utilizadores:
    - Indústria automobilística
    - Programa Espacial

## Slide 6 — Histórico de modelagem

- Década de 60:
  - Ivan Sutterland cria o sketchpad
  - Poucos testes de integridade nos modelos
  - Utilização de menus em cascata
  - Desenho baseado em restrições
  - Modelagem Hierárquica

## Slide 7 — Histórico de modelagem

- Década de 60:
  - Desenho Baseado em Restrições:
    - Modelo paramétrico
    - Geometria pode ser obtida:
      - Partindo da programação do relacionamento dos parâmetros
    - Dois tipos de restrições:
      - Dimensionais (uni, bi e tridimensionais)
      - Geométricas (linhas, planos e superfícies)

## Slide 8 — Histórico de modelagem

- Década de 60:
  - Desenvolvimento da Análise de Elementos Finitos
  - Primitivas geométricas para modelagem
  - Vídeo gráfico com memória
  - Modelagem por superfícies

## Slide 9 — Histórico de modelagem

- Década de 70:
  - Surge a Geometria Computacional
  - Desenvolvimento de programas 3D baseados em primitivas geométricas: cubos, esferas, cilindros, etc
  - Aparecimento de vídeo com tecnologia Raster
  - Modelagem de Sólidos:
    - Informações do fechamento e conectividade dos objetos:
      - Implícita ou explicitamente
    - Garante a realização física dos objetos modelados

## Slide 10 — Histórico de modelagem

- Década de 80:
  - Modelagem de dimensão mista ou non-manifold
    - Permite representar objetos com estruturas internas ou com elementos pendentes de dimensão inferior ao modelo dado
    - Sólido delimitado por superfícies não necessariamente planas localmente (vizinhança de pontos na fronteira em um disco)
  - Lançado o AutoCAD nos Estados Unidos (~US$ 10000,00)
  - Lançado primeiro sistema exploratório de modelagem 3D

## Slide 11 — Histórico de modelagem

- Década de 90:
  - Os softwares de modelagem passam a se basear nas estações de trabalho (maior poder de processamento)
  - Desenvolvimento e pesquisa de softwares junto à indústria (Ford, Chevrolet, Renault, Boeing, etc)
  - Proliferação de desenvolvimento de pacotes
- Hoje:
  - Hardware poderosos (placas gráficas, etc)
  - Memórias baratas (RAM, discos rígidos, etc)

## Slide 12 — Definição de modelos

- Modelos:
  - Objetos construídos artificialmente
  - Representações do mundo físico e do virtual
  - Visualização e estudo de objetos e fenômenos
  - Os modelos podem ser de vários tipos, como:
    - Físicos: baseados nas características

## Slide 13 — Definição de modelos

- Modelos:
  - Objetos construídos artificialmente
  - Representações do mundo físico e do virtual
  - Visualização e estudo de objetos e fenômenos
  - Os modelos podem ser de vários tipos, como:
    - Físicos: baseados nas características
    - Moleculares: baseado no arranjo de átomos

## Slide 14 — Definição de modelos

- Modelos:
  - Objetos construídos artificialmente
  - Representações do mundo físico e do virtual
  - Visualização e estudo de objetos e fenômenos
  - Os modelos podem ser de vários tipos, como:
    - Físicos: baseados nas características
    - Moleculares: baseado no arranjo de átomos
    - Matemáticos: baseados em dados e equações

## Slide 15 — Tipos de modelos

- Modelos Físicos:
  - Representam objetos que podem existir
  - Compartilha as características de suas contra-partes, exceto, às vezes, o tamanho
  - Ex: prédios, carros, sistema solar, etc

## Slide 16 — Tipos de modelos

- Modelos Moleculares:
  - Permitem a visualização de moléculas e também de objetos microscópicos
  - Compartilham as características químicas de suas contra-partes

## Slide 17 — Tipos de modelos

- Modelos Matemáticos:
  - Representam os aspectos comportamentais de fenômenos
  - São baseados em dados coletados, equações matemáticas e previsões
  - Ex: fluxo turbulento de um fluido

## Slide 18 — Uso de modelos

- Por que usar modelos?
  - Facilitam o estudo de fenômenos
  - Auxiliam o projeto de novos objetos
  - Uso em simulações computacionais
  - Desenvolvimento de treinamentos
  - Usados para áreas de entretenimento

## Slide 19 — Composição de modelos

- Composição de um Modelo:
  - Primitivas geométrica simples
  - Composição de objetos complexos
  - Conjunto de partículas
  - Parametrização de subespaços matemáticos

## Slide 20 — Modelagem computacional

- Modelos no Computador:
  - Dados armazenados em um arquivo
  - Uso intensivo de estruturas de dados
  - Podem ser, matemáticos, moleculares ou físicos
  - São normalmente de propósito geral
  - Suportam uma ampla variedade de aplicações

## Slide 21 — Abordagens em Modelagem

- Abordagens:
  - Modelagem Geométrica
  - Modelagem Computacional

## Slide 22 — Abordagens em Modelagem

- Modelagem Geométrica:
  - Resolve problemas geométricos
  - Principal preocupação:
    - Como armazenar o modelo
  - Responde a perguntas do tipo:
    - Qual parte do modelo é vista pelo usuário?
    - Qual cor está associada a determinado elemento?

## Slide 23 — Abordagens em Modelagem

- Modelagem Computacional:
  - Trata da simulação de soluções para problemas científicos (uso de simulações computacionais)
  - Analisa fenômenos e elabora modelos matemáticos para sua descrição (modelagem matemática adotada)
  - Principal preocupação é elaborar algoritmos para as soluções (uso de soluções computacionais)

## Slide 24 — Processo de Modelagem

```text
Modelador
Descrição -> Modelagem -> Representação

Representação -> Algo 1 -> Resultado
Representação -> Algo 2 -> Resultado
Representação -> Algo n -> Resultado

Pergunta Geométrica -> Algo n

Sistema Geométrico
```

## Slide 25 — Processo de Modelagem

- Modelar:
  - A descrição do objeto é o ponto de partida
  - Construir um modelo artificial do modelo
  - Achar a representação adequada (estrutura de dados)
  - Responder as questões geométricas algoritmicamente
  - Exibir / Armazenar o resultado da modelagem

## Slide 26 — Processo de Modelagem

- Níveis de abstração em modelagem:
  - Físico: sólido propriamente dito
  - Contínuo: representação matemática
  - Representação: armazenamento (pontos, coeficientes, etc)
  - Implementação: código, estrutura de dados

## Slide 27 — Processo de Modelagem

- A geometria de um objeto é fonte de muitas informações potenciais (entrada do processo)
- Técnicas de armazenamento e processamento de dados geométricos são independentes das aplicações
- Métodos idênticos podem ser usados para construir modelos de navios, barcos, casas, panelas, etc
- Faz sentido separar os dados geométricos:
  - Modelo do objeto: todos os dados
  - Modelo geométrico: dados geométricos

## Slide 28 — Aplicações de modelagem

Temperatura dos oceanos durante o el niño

Treinamento para cirurgias

Um braço robô e seu modelo; este modelo serviu de auxílio para o projeto

Gp de Mônaco em um jogo de corridas
