# Arquitetura do modelador CSG

## Objetivo

Construir um modelador de sólidos por CSG em C++ e Linux. O modelo é uma árvore: as folhas são primitivas e os nós internos são operações booleanas ou geométricas. As operações e análises atuam sobre a árvore, não sobre as primitivas.

## Estado atual

A infraestrutura está pronta: CMake, janela GLFW/OpenGL com Dear ImGui e GoogleTest. As camadas `Core`, `Persistence` e `Application` existem no CMake e ainda não têm fontes. Os detalhes dos algoritmos CSG serão confirmados nas notas da unidade 3 antes da implementação.

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
├── árvore CSG (folhas e nós internos)
├── primitivas e seus parâmetros
├── transformações
├── classificação SMC (IN, ON, OUT)
└── análise
        │
        ├──────────────► Persistência
        ├──────────────► Renderização (ray-casting)
        └──────────────► Testes unitários
```

## Responsabilidades

### Núcleo geométrico

- Representar a árvore CSG e seus invariantes.
- Classificar pontos e segmentos contra primitivas.
- Combinar classificações nos nós da árvore (SMC).
- Executar operações e análises sobre a árvore.
- Não depender de GLFW, OpenGL ou Dear ImGui.

### Aplicação

- Coordenar casos de uso e validar pré-condições.
- Manter a lista de modelos e a seleção por identificador estável.
- Produzir novos modelos como resultado, sem alterar as entradas.

### Persistência

- Salvar e ler árvores no formato definido em sala.
- Rejeitar arquivos inválidos ou incompletos.

### Renderização

- Gerar a imagem por ray-casting com classificação SMC.
- Exibir as partes IN, ON e OUT com cores distintas.
- Não alterar o modelo geométrico.

### Interface

- Coletar parâmetros, acionar casos de uso e apresentar erros.
- Não conter algoritmos do núcleo.

## Fluxo principal

```text
Parâmetros da primitiva
→ folha da árvore CSG
→ composição por operações
→ classificação SMC
→ análise, persistência ou renderização
```

## Qualidade e validação

- Testes do núcleo não abrem janela nem criam contexto OpenGL.
- Cada operação é testada com folhas e com nós internos.
- A persistência tem testes de ida e volta e de rejeição.
- Um card só é concluído após compilação e testes em Linux.

## Pendências

- Técnica de modelagem local ou global, definida em sala.
- Formato de arquivo, definido em sala.
- Algoritmos de SMC, volume e transformações, conforme as notas da unidade 3.
