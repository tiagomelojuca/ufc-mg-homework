# ADR-0001 — Plataforma e ambiente

- Estado: Aceito
- Data: 2026-09-22

## Contexto

O enunciado pede C++ e Linux. Estamos usando C++17 com CMake, GLFW, OpenGL e Dear ImGui. Vamos compilar, testar e executar o projeto em Linux.

## Decisão

Vamos continuar com C++17 e CMake. Também vamos manter GLFW para janela e entrada, OpenGL para os gráficos e Dear ImGui para a interface.

No Linux, vamos usar X11 como backend gráfico padrão. O suporte a Wayland continuará disponível pela opção `UFC_MG_USE_WAYLAND` do CMake.

## Consequências

- Manteremos o código de domínio portátil e independente das bibliotecas gráficas.
- Só vamos considerar a configuração atual validada depois de compilá-la e executá-la em Linux.
- Vamos verificar no Linux as dependências obtidas pelo CMake.
- Quem precisar de Wayland poderá configurar o projeto com `-DUFC_MG_USE_WAYLAND=ON`.
