# Arquitetura do modelador de octree

## Estado atual

O projeto contém uma infraestrutura inicial de aplicação gráfica:

```text
main.cpp
└── TMainWindow
    └── TWindow
        ├── GLFW / contexto OpenGL
        ├── Dear ImGui / janela de demonstração
        └── loop de eventos e renderização
```

Também existem receitas CMake para baixar GLFW, Dear ImGui e GoogleTest. A configuração, a aplicação gráfica e os testes já foram executados em Linux.

O núcleo geométrico agora possui `TOctree`, `TNoOctree`, `TCubo`, a configuração do domínio e o contrato de classificação. A construção recursiva segue o algoritmo apresentado em aula. Os classificadores de bloco e esfera estão implementados com construção local.

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

### Persistência

- Codificar e decodificar a representação DF.
- Validar caracteres, aridade e término completo da string.
- Manter o arquivo de intercâmbio restrito à string definida pelo professor.
- Não vamos acrescentar metadados particulares ao formato que precisamos compartilhar com os outros grupos.

### Renderização

- Converter o estado do modelo em comandos gráficos.
- Oferecer visualização aramada das primitivas.
- Oferecer visualização da octree como bônus.
- Não alterar o modelo geométrico.

### Interface

- Coletar parâmetros, acionar casos de uso e apresentar erros.
- Não conter algoritmos de octree.

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
- A representação persistida percorre a árvore em profundidade primeiro.
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
- Cada operação deve testar casos com folhas, nós internos e profundidades diferentes.
- Persistência deve possuir testes de ida e volta e rejeição de strings inválidas.
- A definição de concluído exigirá compilação e testes em Linux.
- Só vamos usar padrões de projeto quando eles reduzirem acoplamento ou duplicação; eles não são um objetivo isolado.

## Pendências arquiteturais

Não há decisão arquitetural bloqueando o início do MVP. Questões futuras continuam registradas em [decisoes-pendentes.md](decisoes-pendentes.md) sem serem tratadas como decisões concluídas.
