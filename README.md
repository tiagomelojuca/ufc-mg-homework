# Trabalho 1 — Modelador com octree

Projeto desenvolvido para o Trabalho 1 da disciplina de Modelagem Geométrica da Universidade Federal do Ceará, ministrada pelo professor Joaquim Bento Cavalcante Neto.

O software é um modelador 3D baseado em subdivisão espacial por octree. A implementação prioriza os requisitos de graduação e mantém os recursos de mestrado, doutorado e outros bônus como extensões posteriores.

## Funcionalidades atuais

- aplicação gráfica inicial com GLFW, OpenGL e Dear ImGui;
- octree local com nós cheios, vazios e parciais;
- domínio e profundidade máxima configuráveis;
- construção recursiva baseada no algoritmo apresentado em aula;
- geração de octrees para blocos e esferas;
- persistência textual de octrees no formato DF;
- união de octrees com domínios compatíveis;
- escala uniforme de octrees em torno da origem;
- cálculo de volume pelas folhas cheias;
- visualização aramada de bloco e esfera a partir das folhas cheias;
- interface para criação, união, escala, arquivos e remoção de modelos;
- volume, profundidade e quantidade de células cheias do modelo selecionado;
- visualização sólida opcional, com o aramado como padrão;
- inspeção da estrutura completa da octree em uma janela flutuante (bônus);
- testes unitários do núcleo geométrico com GoogleTest.

A aplicação apresenta uma lista de modelos, controles agrupados por operação e uma visualização aramada do modelo selecionado, com vista fixa.

## Tecnologias

- C++17
- CMake 3.16 ou mais recente
- GLFW 3.5.1
- OpenGL
- Dear ImGui 1.92.9
- GoogleTest 1.18.0

O CMake baixa GLFW, Dear ImGui e GoogleTest durante a primeira configuração. Essa etapa requer acesso à internet.

## Obter o projeto

```bash
git clone https://github.com/tiagomelojuca/ufc-mg-homework.git
cd ufc-mg-homework
```

## Requisitos no Linux

Linux é o ambiente principal de compilação, teste e execução. O projeto também pode ser executado no Windows por meio do WSL2 com WSLg.

No Ubuntu, as dependências básicas podem ser instaladas com:

```bash
sudo apt update
sudo apt install build-essential cmake git pkg-config libgl1-mesa-dev xorg-dev
```

X11 é o backend gráfico padrão. No WSLg, aplicações X11 são encaminhadas pelo próprio ambiente gráfico do WSL.

## Compilar

Na raiz do repositório, execute:

```bash
cmake -S . -B build -DBUILD_TEST_SUIT=ON -DUFC_MG_USE_WAYLAND=OFF
cmake --build build --parallel
```

Os arquivos gerados pelo CMake devem permanecer em um diretório separado, como `build`. A configuração dentro da pasta de código-fonte é rejeitada pelo projeto.

## Executar

```bash
./build/ufc-mg-homework
```

A aplicação gráfica requer uma sessão X11, WSLg ou outro ambiente gráfico compatível.

Na janela, use **Criar** para definir um bloco ou uma esfera. A profundidade começa em 5 e pode variar de 1 a 8; ela afeta somente novas criações e leituras.

Em **Operar**, selecione duas entradas para união ou aplique escala ao modelo selecionado. Cada resultado entra na lista como um novo modelo e preserva as entradas. O volume aparece automaticamente junto à visualização.

Em **Arquivo**, informe o caminho para abrir ou salvar uma octree no formato DF. Caminhos relativos usam a pasta de execução. A substituição de um arquivo existente pede confirmação.

Marque **Sólido**, acima da visualização, para preencher as faces externas do modelo; desmarcado, a visualização volta ao aramado.

O botão **Estrutura da octree**, acima da visualização, abre uma janela flutuante com todos os nós do modelo selecionado: árvore navegável, quantidade de nós por nível e aramado colorido por estado. Clique em um nó da árvore para enquadrá-lo.

## Executar os testes

```bash
ctest --test-dir build --output-on-failure
```

Os testes do núcleo e da aplicação não abrem janelas nem criam um contexto OpenGL. A suíte atual contém 98 testes.

Para compilar sem os testes:

```bash
cmake -S . -B build -DBUILD_TEST_SUIT=OFF -DUFC_MG_USE_WAYLAND=OFF
cmake --build build --parallel
```

## Suporte adicional ao Wayland

O suporte a Wayland é opcional. Para habilitá-lo no Ubuntu:

```bash
sudo apt install libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules
cmake -S . -B build-wayland -DBUILD_TEST_SUIT=ON -DUFC_MG_USE_WAYLAND=ON
cmake --build build-wayland --parallel
```

## Estrutura do código

```text
src/Core/    núcleo geométrico e octree
src/Application/ modelos e coordenação das operações
src/Persistence/ persistência da representação DF
src/UI/      janela e interface gráfica
test/        testes unitários
```

## Características técnicas

- O núcleo geométrico não depende da interface nem da renderização.
- A octree usa subdivisão local: somente células parciais são subdivididas.
- O domínio padrão é o cubo `[-1,1]³`.
- A profundidade máxima padrão é `5`.
- Blocos e esferas são classificados contra cada célula como fora, dentro ou parcial.
- A união percorre duas octrees de mesmo domínio e produz uma nova árvore compactada.
- A escala transforma as folhas cheias e reconstrói a ocupação no domínio configurado.
- O volume é calculado recursivamente pela soma dos cubos representados pelas folhas cheias.
- União e escala acrescentam novos modelos à lista, preservando os modelos de entrada.
- Os algoritmos seguem as abstrações apresentadas em aula e na bibliografia da disciplina.
