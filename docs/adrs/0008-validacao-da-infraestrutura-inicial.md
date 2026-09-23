# ADR-0008 — Validação da infraestrutura inicial

- Estado: Aceito
- Data: 2026-09-22

## Contexto

Já temos a configuração CMake, a integração inicial de GLFW/OpenGL/ImGui e a estrutura do GoogleTest. A octree ainda está vazia e o único teste sempre passa. Ainda não validamos essa parte do projeto em Linux.

## Decisão

Vamos registrar a infraestrutura atual como trabalho iniciado. Só marcaremos os cards dessa etapa como concluídos depois de compilar, executar e testar o projeto em Linux. Até lá, eles ficarão em revisão.

## Consequências

- Não vamos apresentar progresso artificial na board.
- Vamos tratar as questões encontradas na infraestrutura inicial como trabalho de consolidação do projeto.
- Usaremos a compilação, a execução e os testes em Linux como evidência para mover esses cards.
