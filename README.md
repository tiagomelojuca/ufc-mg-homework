# Modelador CSG

Modelador de sólidos por CSG (Constructive Solid Geometry) com modelagem local, em C++17 para Linux. As folhas da árvore são primitivas em posição canônica; os nós internos são operações booleanas regularizadas ou movimentos rígidos. Todas as operações e análises atuam sobre a árvore, não sobre as primitivas.

## Funcionalidades

- **Primitivas:** bloco, esfera e cilindro.
- **Operações:** união e interseção regularizadas, translação e rotação como nós da árvore, e escala pelos parâmetros da primitiva.
- **Classificação SMC** de arestas contra a árvore (`Tree_M`, `Prim_M` e `Combine_M`), com as partes IN, ON e OUT exibidas em cores distintas.
- **Renderização** por ray-casting com iluminação de Phong, ou em aramado pelos segmentos `EonS`.
- **Visualização da árvore CSG** do modelo selecionado.
- **Volume** por conversão da árvore em octree.
- **Arquivos:** salvar e ler árvores no formato definido em sala.

## Requisitos

- Linux (desenvolvido e testado no Ubuntu 26.04).
- CMake 3.16 ou mais recente.
- Compilador com suporte a C++17 (GCC ou Clang).
- Git e acesso à internet na primeira configuração: o CMake baixa a GLFW 3.5.1, a Dear ImGui v1.92.9 e o GoogleTest v1.18.0.
- Pacotes de desenvolvimento do X11 e do OpenGL, que a GLFW exige para compilar.

No Ubuntu ou no Debian, instale tudo de uma vez:

```bash
sudo apt install build-essential cmake git libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev
```

## Compilação

O build precisa ficar fora da pasta do código-fonte; a configuração dentro dela é recusada.

```bash
cmake -S . -B build
```

```bash
cmake --build build -j
```

O executável fica em `build/ufc-mg-csg`, e os testes em `build/test/ufc-mg-csg_tests`.

### Opções do CMake

| Opção | Padrão | Efeito |
| --- | --- | --- |
| `UFC_MG_USE_WAYLAND` | `OFF` | Compila a GLFW também com suporte a Wayland. Sem ela, a janela usa X11, que funciona também em sessões Wayland pelo XWayland. |
| `BUILD_TEST_SUIT` | `ON` | Compila a suíte de testes unitários. |

Exemplo: `cmake -S . -B build -DBUILD_TEST_SUIT=OFF`.

### Problemas comuns

**A configuração para em um erro da GLFW sobre cabeçalhos ausentes.** A GLFW verifica um pacote de cada vez: ao instalar só o pacote do primeiro erro, a próxima configuração para no seguinte. Instale de uma vez todos os pacotes listados em [Requisitos](#requisitos). Cada mensagem corresponde a um pacote:

| Mensagem | Pacote |
| --- | --- |
| `RandR headers not found` | `libxrandr-dev` |
| `Xinerama headers not found` | `libxinerama-dev` |
| `XKB headers not found` | `libx11-dev` |
| `Xcursor headers not found` | `libxcursor-dev` |
| `XInput headers not found` | `libxi-dev` |
| `X Shape headers not found` | `libxext-dev` |
| `Could NOT find OpenGL` | `libgl1-mesa-dev` |

**O erro continua depois de instalar os pacotes.** O CMake guarda em cache o resultado das buscas anteriores. Apague o cache e configure de novo:

```bash
rm build/CMakeCache.txt
```

```bash
cmake -S . -B build
```

**Compilar com Wayland.** Com `-DUFC_MG_USE_WAYLAND=ON`, a GLFW precisa também dos pacotes do Wayland:

```bash
sudo apt install pkg-config libwayland-dev libwayland-bin libxkbcommon-dev wayland-protocols
```

## Execução

```bash
./build/ufc-mg-csg
```

O programa abre com a lista de modelos vazia. Para começar por um modelo pronto, digite `modelos/ford.txt` no campo **Caminho** do painel **Modelos** e clique em **Carregar**.

### Painéis

- **Modelos:** lista e seleção dos modelos; criação de bloco, esfera e cilindro; união e interseção com um segundo modelo; translação, rotação (em graus) e escala do modelo selecionado; cálculo do volume com a profundidade da octree; salvar e carregar arquivos. Cada operação cria um novo modelo e não altera as entradas.
- **Visualização:** o modelo selecionado por ray-casting ou em aramado (`EonS`), com a câmera em órbita em torno do seu centro. Por cima da imagem, as arestas da classificação SMC, com IN em verde, ON em amarelo e OUT em vermelho; as arestas podem ser editadas, acrescentadas e removidas.
- **Árvore CSG:** o diagrama da árvore do modelo selecionado, com uma cor por tipo de nó.

## Testes

```bash
ctest --test-dir build
```

Também é possível executar o binário dos testes diretamente, o que mostra o resultado de cada teste:

```bash
./build/test/ufc-mg-csg_tests
```

Os testes do núcleo não abrem janela nem criam contexto OpenGL.

## Formato de arquivo

Uma linha por nó, com os campos separados por `;`. A raiz é o nó da última linha, e um nó só referencia nós definidos antes dele.

```text
No1; B; S 4,1,2; T 0,0.9,0
No2; E; S 0.5; T 1,0,0; R 0,0,90
No3; U No1 No2
```

| Categoria | Códigos |
| --- | --- |
| Primitiva | `B` bloco (`S` com os três lados), `E` esfera (`S` com o raio), `C` cilindro (`S` com raio e altura) |
| Booleana | `U` união, `I` interseção |
| Geométrica | `S` parâmetros, `T` translação, `R` rotação em graus |

`T` e `R` são aplicados na ordem em que aparecem. Bloco e esfera são centrados na origem; o cilindro tem eixo `z`, com a base em `z = 0`.

## Estrutura

```text
src/
├── Core/           núcleo geométrico: árvore CSG, primitivas, SMC, ray-casting, aramado, octree
├── Persistence/    leitura e escrita de arquivos
├── Application/    casos de uso e lista de modelos
└── UI/             janela GLFW, painéis Dear ImGui e renderização
test/               testes unitários (GoogleTest)
modelos/            modelos de exemplo
docs/               arquitetura, decisões (ADRs) e board
```

O núcleo não depende de GLFW, OpenGL ou Dear ImGui. A arquitetura está descrita em [docs/architecture.md](docs/architecture.md), e as decisões de projeto em [docs/adrs](docs/adrs/README.md).
