# Arquitetura do modelador CSG

## Objetivo

Construir um modelador de sólidos por CSG em C++ e Linux, com modelagem local. As folhas da árvore são primitivas; os nós internos são operações booleanas ou movimentos rígidos. As operações e análises atuam sobre a árvore, não sobre as primitivas.

## Estado atual

A infraestrutura está pronta: CMake, janela GLFW/OpenGL com Dear ImGui e GoogleTest. O núcleo contém a árvore CSG: `TNoCSG` representa os três tipos de nó, e `TArvoreCSG` aponta para a raiz. Os nós são imutáveis e compartilháveis, de modo que uma operação nunca altera suas entradas. `TPrimitiva` define o contrato das primitivas: classificar um ponto e classificar uma aresta em partes IN, ON e OUT. `TBloco` e `TEsfera` implementam esse contrato em posição canônica. As camadas `Persistence` e `Application` ainda não têm fontes.

## Camadas

```text
Interface Dear ImGui
        │
        ▼
Aplicação / casos de uso
├── criação de primitivas
├── operações booleanas e geométricas
├── análise de volume
└── salvar / ler
        │
        ▼
Núcleo geométrico
├── árvore CSG (primitiva, operação booleana, movimento rígido)
├── primitivas canônicas e seus parâmetros
├── classificação SMC (IN, ON, OUT)
├── ray-casting
└── volume por conversão em octree
        │
        ├──────────────► Persistência
        ├──────────────► Renderização
        └──────────────► Testes unitários
```

## Responsabilidades

### Núcleo geométrico

- Representar a árvore CSG e seus invariantes.
- Classificar arestas e pontos contra primitivas.
- Combinar classificações nos nós booleanos e transformar entidades nos nós de movimento rígido.
- Calcular a primeira interseção de um raio e a normal no ponto.
- Calcular o volume pela octree gerada da árvore.
- Não depender de GLFW, OpenGL ou Dear ImGui.

### Aplicação

- Coordenar casos de uso e validar pré-condições.
- Manter a lista de modelos e a seleção por identificador estável.
- Produzir novos modelos como resultado, sem alterar as entradas.

### Persistência

- Salvar e ler árvores no formato do quadro.
- Rejeitar arquivos inválidos ou incompletos.

### Renderização

- Gerar a imagem por ray-casting.
- Exibir as partes IN, ON e OUT de arestas com cores distintas.
- Não alterar o modelo geométrico.

### Interface

- Coletar parâmetros, acionar casos de uso e apresentar erros.
- Não conter algoritmos do núcleo.

## Invariantes

- Toda folha é uma primitiva; toda primitiva é folha.
- Um nó booleano tem dois filhos; um nó de movimento rígido tem um filho.
- Primitivas são definidas em posição canônica; translação e rotação são nós.
- A escala altera os parâmetros da primitiva.
- O cilindro tem eixo `z`, raio `r` e altura `h`, com base em `z = 0`.
- Bloco e esfera são centrados na origem.
- A classificação de uma aresta contra uma primitiva cobre todo o intervalo `t ∈ [0, 1]`, em partes ordenadas e sem sobreposição.
- As operações booleanas são regularizadas.
- O volume é a soma das folhas cheias da octree, classificadas pelo centro.

## Fluxo principal

```text
Parâmetros da primitiva
→ folha canônica
→ nós de movimento rígido e operações booleanas
→ classificação SMC
→ ray-casting, exibição IN/ON/OUT, volume ou persistência
```

## Qualidade e validação

- Testes do núcleo não abrem janela nem criam contexto OpenGL.
- Cada primitiva, operação booleana e movimento rígido tem testes de classificação.
- A persistência tem testes de ida e volta e de rejeição.
- Um card só é concluído após compilação e testes em Linux.

## Deduções a verificar

- Bloco e esfera centrados na origem.
- Rotação `R = Rz · Ry · Rx`, em graus, conforme Mortenson.
- Movimento rígido em nó interno no arquivo, conforme a ADR-0005.
