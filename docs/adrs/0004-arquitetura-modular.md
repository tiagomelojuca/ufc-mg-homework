# ADR-0004 — Arquitetura modular

- Estado: Aceito
- Data: 2026-09-22

## Contexto

Hoje já temos a infraestrutura gráfica, mas a octree ainda está vazia. Precisamos conseguir testar os algoritmos geométricos sem abrir uma janela ou criar um contexto OpenGL.

## Decisão

Vamos separar o projeto em núcleo geométrico, aplicação, persistência, renderização e interface. O núcleo não vai depender de GLFW, OpenGL ou Dear ImGui. A interface e a renderização acessarão o modelo por meio dos casos de uso e de representações somente de leitura.

Só vamos transformar classificadores e operações em componentes substituíveis quando houver uma variação real de algoritmo. Também não vamos introduzir padrões de projeto sem uma responsabilidade concreta que os justifique.

## Consequências

- Poderemos executar testes unitários rápidos e determinísticos no núcleo.
- Poderemos modificar persistência e renderização sem alterar os invariantes da árvore.
- Vamos configurar no CMake ao menos uma biblioteca para o núcleo e executáveis separados para a aplicação e os testes.
