# Trabalho 1 — Modelador com octree

Projeto desenvolvido para o Trabalho 1 da disciplina de Modelagem Geométrica da Universidade Federal do Ceará, ministrada pelo professor Joaquim Bento Cavalcante Neto.

O software é um modelador 3D baseado em subdivisão espacial por octree. A implementação prioriza os requisitos de graduação e mantém os recursos de mestrado, doutorado e outros bônus como extensões posteriores.

## Funcionalidades atuais

- aplicação gráfica inicial com GLFW, OpenGL e Dear ImGui;
- octree local com nós cheios, vazios e parciais;
- domínio e profundidade máxima configuráveis;
- construção recursiva baseada no algoritmo apresentado em aula;
- geração de octrees para blocos e esferas;
- testes unitários do núcleo geométrico com GoogleTest.

A interface gráfica ainda está em desenvolvimento. A versão atual abre a janela da aplicação, mas a criação e a manipulação dos modelos ainda não estão disponíveis pela interface.

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

## Executar os testes

```bash
ctest --test-dir build --output-on-failure
```

Os testes do núcleo não abrem janelas nem criam um contexto OpenGL. A suíte atual contém 24 testes.

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
src/UI/      janela e interface gráfica
test/        testes unitários
```

## Características técnicas

- O núcleo geométrico não depende da interface nem da renderização.
- A octree usa subdivisão local: somente células parciais são subdivididas.
- O domínio padrão é o cubo `[-1,1]³`.
- A profundidade máxima padrão é `5`.
- Blocos e esferas são classificados contra cada célula como fora, dentro ou parcial.
- Os algoritmos seguem as abstrações apresentadas em aula e na bibliografia da disciplina.
