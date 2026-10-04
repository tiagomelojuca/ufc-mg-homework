# Arquitetura do modelador de octree

## Estado atual

O projeto contém uma aplicação gráfica com criação e operações sobre octrees:

```text
main.cpp
└── TMainWindow
    └── TWindow
        ├── GLFW / contexto OpenGL
        ├── TPainelModelador / controles Dear ImGui
        │   └── TModelador / lista de modelos e casos de uso
        ├── TJanelaEstruturaOctree / inspeção da octree
        ├── TRenderizadorAramado / desenho OpenGL
        └── loop de eventos e renderização
```

Também existem receitas CMake para baixar GLFW, Dear ImGui e GoogleTest. A configuração, a aplicação gráfica e os testes já foram executados em Linux.

O núcleo geométrico agora possui `TOctree`, `TNoOctree`, `TCubo`, a configuração do domínio e o contrato de classificação. A construção recursiva segue o algoritmo apresentado em aula. Os classificadores de bloco e esfera estão implementados com construção local. A persistência salva e carrega a representação DF em arquivos de texto compatíveis com o formato do professor.

As operações do núcleo já incluem união, escala e cálculo de volume. A união percorre duas árvores de forma sincronizada. A escala transforma as regiões das folhas cheias e reconstrói a ocupação no mesmo domínio. O volume é calculado diretamente por um percurso recursivo da árvore.

`TGeradorAramadoOctree` percorre a árvore e produz as 12 arestas de cada folha cheia. O núcleo gera segmentos independentes de OpenGL. `TRenderizadorAramado`, em `src/UI`, desenha esses segmentos com `GL_LINES` em uma vista ortográfica fixa.

`TGeradorSuperficieOctree` gera as faces das folhas cheias que não estão totalmente encostadas em outra região cheia. Quando o checkbox “Sólido” está marcado, `TRenderizadorAramado::DesenhaSolido` preenche essas faces e desenha as arestas visíveis por cima; o padrão continua sendo o aramado.

`TGeradorAramadoOctree::GeraEstrutura` também produz as caixas de todos os nós até um nível escolhido, separadas por estado, e `TAnaliseOctree::ContaNosPorNivel` conta os nós de cada nível. `TJanelaEstruturaOctree` usa essas funções em uma janela flutuante aberta pelo botão “Estrutura da octree”. A cena dessa janela é desenhada por um callback da ImGui, para respeitar a ordem de composição das janelas.

`TModelador`, em `src/Application`, mantém uma lista de modelos e coordena criação, profundidade, união, escala, arquivo e remoção. Cada `TModeloOctree` guarda nome, identificador, uma octree imutável, suas arestas, suas faces e seu volume. Os resultados são novos modelos, selecionados automaticamente. `TPainelModelador` apresenta os controles, as informações e os erros; a estratégia da janela chama o painel e o desenho do modelo selecionado a cada quadro.

## Arquitetura-alvo

```text
Aplicação / casos de uso
├── criação de primitivas
├── operações booleanas e geométricas
├── análise de volume e área
└── salvar / carregar
        │
        ▼
Núcleo geométrico
├── TOctree e TNoOctree
├── TCubo e TConfiguracaoOctree
├── domínio espacial e octantes
├── classificadores de primitivas
├── percursos e compactação
└── algoritmos geométricos
        │
        ├──────────────► Persistência DF
        ├──────────────► Renderização OpenGL
        └──────────────► Testes unitários
                              ▲
                              │
Interface Dear ImGui ─────────┘
```

## Responsabilidades

### Núcleo geométrico

- Representar a octree local e seus invariantes.
- Manter domínio e profundidade em uma configuração única, independente da interface.
- Classificar células contra primitivas.
- Executar operações nas árvores.
- Calcular propriedades geométricas.
- Não depender de GLFW, OpenGL ou Dear ImGui.

### Aplicação

- Coordenar casos de uso e validar pré-condições.
- Manter o modelo ativo e a composição do tema escolhido.
- Traduzir ações da interface em operações do núcleo.
- Preparar aramado e volume uma vez ao acrescentar cada modelo.
- Preservar as entradas de união e escala e manter a seleção por identificador estável.

### Persistência

- Codificar e decodificar a representação DF.
- Validar caracteres, aridade e término completo da string.
- Manter o arquivo de intercâmbio restrito à string definida pelo professor.
- Não vamos acrescentar metadados particulares ao formato que precisamos compartilhar com os outros grupos.

### Renderização

- Converter o estado do modelo em comandos gráficos.
- Oferecer visualização aramada das primitivas como padrão e visualização sólida opcional.
- Oferecer visualização da octree como bônus, em uma janela flutuante que não ocupa a interface principal.
- Não alterar o modelo geométrico.
- Manter o percurso geométrico separado dos comandos OpenGL.
- Usar as regiões das folhas cheias, preservando as arestas internas e compartilhadas nesta versão.

### Interface

- Coletar parâmetros, acionar casos de uso e apresentar erros.
- Não conter algoritmos de octree.
- Usar profundidade de 1 a 8 para novas criações e leituras, sem reconstruir árvores existentes.
- Confirmar a substituição de arquivos existentes.

## Invariantes conhecidos

- Um nó `B` ou `W` é folha e não possui filhos.
- Um nó `(` é parcial/interno e possui exatamente oito filhos ordenados.
- O domínio padrão da raiz é `[-1,1]³`, mas os algoritmos recebem esse domínio pela configuração.
- A profundidade máxima padrão é `5` e pode ser escolhida na interface antes da construção.
- Os filhos seguem a ordem `x + 2z + 4y`, com bits `0` para as metades inferiores e `1` para as superiores.
- A subdivisão ocorre somente em células parciais, caracterizando a estratégia local.
- Ao atingir o limitante de profundidade, o algoritmo mostrado pelo professor assume o bloco terminal como cheio.
- O bloco é alinhado aos eixos e parametrizado por centro e três lados.
- A esfera é parametrizada por centro e raio.
- Uma célula que apenas toca a fronteira de uma primitiva, sem compartilhar volume, é considerada vazia.
- A classificação da esfera compara o raio com as distâncias da célula mais próxima e mais distante do centro.
- Operações booleanas percorrem árvores de forma sincronizada e exigem espaços compatíveis.
- A união aceita profundidades diferentes, produz uma árvore com a maior profundidade configurada e compacta filhos homogêneos.
- A escala é uniforme, usa a origem como ponto fixo e aceita somente fatores finitos e positivos.
- A escala transforma as folhas cheias da octree, não as primitivas usadas na construção inicial.
- Uma operação geométrica é rejeitada quando o volume transformado ultrapassa o domínio.
- Uma folha vazia contribui com volume zero e uma folha cheia contribui com o cubo do lado de sua região.
- O volume de um nó parcial é a soma dos volumes dos oito filhos.
- O volume calculado corresponde às células representadas pela octree e mantém a aproximação definida pela profundidade da árvore.
- A representação persistida percorre a árvore em profundidade primeiro.
- O arquivo DF contém somente os símbolos `B`, `W` e `(`, sem cabeçalho, separadores ou marcador de fechamento.
- Cada `(` deve ser seguido pelas representações de exatamente oito filhos.
- A leitura rejeita entrada vazia, símbolos desconhecidos, árvores incompletas, conteúdo excedente e profundidade incompatível com a configuração.
- A mesma string só preserva a geometria entre modeladores quando todos compartilham o domínio raiz e a ordem espacial dos oito filhos.

## Fluxo principal

```text
Parâmetros da primitiva
→ classificação célula/primitiva
→ construção recursiva da octree
→ operação/análise sobre a árvore
→ persistência ou renderização
```

## Qualidade e validação

- Testes do núcleo não devem abrir janela nem criar contexto OpenGL.
- Testes da aplicação devem cobrir seleção, preservação das entradas, falhas de operações e o fluxo de arquivos.
- Cada operação deve testar casos com folhas, nós internos e profundidades diferentes.
- Persistência deve possuir testes de ida e volta e rejeição de strings inválidas.
- A definição de concluído exigirá compilação e testes em Linux.
- Só vamos usar padrões de projeto quando eles reduzirem acoplamento ou duplicação; eles não são um objetivo isolado.

## Pendências arquiteturais

Não há decisão arquitetural bloqueando o MVP. A classificação das células terminais está em avaliação. Questões futuras continuam registradas em [decisoes-pendentes.md](decisoes-pendentes.md) sem serem tratadas como decisões concluídas.
