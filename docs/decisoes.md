# Decisões do projeto

Este documento consolida as decisões que tomamos a partir do enunciado, das aulas e do estado atual do projeto. As justificativas completas estão nos ADRs vinculados.

## Decisões aceitas

| Tema | Decisão | ADR |
| --- | --- | --- |
| Plataforma | Estamos usando C++17 com CMake e vamos compilar, testar e executar em Linux | [ADR-0001](adrs/0001-plataforma-e-ambiente.md) |
| Interface e gráficos | Vamos manter GLFW, OpenGL 3 e Dear ImGui | [ADR-0001](adrs/0001-plataforma-e-ambiente.md) |
| Subdivisão | Escolhemos uma octree local/adaptativa | [ADR-0002](adrs/0002-octree-local-e-estados.md) |
| Estados | Usaremos `B` para cheio, `W` para vazio e `(` para parcial/interno | [ADR-0002](adrs/0002-octree-local-e-estados.md) |
| Persistência da árvore | Vamos intercambiar somente a string DF definida pelo professor, sem cabeçalho ou metadados próprios | [ADR-0003](adrs/0003-codificacao-df-da-octree.md) |
| Arquitetura | Vamos manter o núcleo geométrico independente de UI, OpenGL e persistência | [ADR-0004](adrs/0004-arquitetura-modular.md) |
| Escopo | Vamos concluir todo o nível de graduação como MVP antes de começar os bônus | [ADR-0005](adrs/0005-estrategia-de-escopo-e-bonus.md) |
| Tema | Vamos tentar modelar um Ford Escort e trocar o tema se o custo for excessivo com as ferramentas concluídas | [ADR-0006](adrs/0006-tema-ford-escort.md) |
| Critério de decisão | Quando o enunciado não exigir sofisticação, escolheremos a alternativa mais básica, simples e portátil | [ADR-0005](adrs/0005-estrategia-de-escopo-e-bonus.md) |
| Apresentação | Vamos preparar uma apresentação autossuficiente e fazer a demonstração ao vivo depois | [ADR-0007](adrs/0007-apresentacao-autossuficiente.md) |
| Validação | Só vamos considerar a infraestrutura inicial concluída depois de compilar e executar em Linux | [ADR-0008](adrs/0008-validacao-da-infraestrutura-inicial.md) |
| Domínio raiz | Usaremos o cubo `[-1,1]³`, mantendo o domínio em uma configuração fácil de substituir | [ADR-0009](adrs/0009-dominio-profundidade-e-ordem-dos-octantes.md) |
| Profundidade | Começaremos com profundidade máxima `5`, alterável pela interface | [ADR-0009](adrs/0009-dominio-profundidade-e-ordem-dos-octantes.md) |
| Ordem dos octantes | Usaremos a numeração `0...7` da aula, formalizada por `x + 2z + 4y` | [ADR-0009](adrs/0009-dominio-profundidade-e-ordem-dos-octantes.md) |
| União | Vamos percorrer duas octrees de mesmo domínio em ordem sincronizada e compactar o resultado | [ADR-0010](adrs/0010-uniao-e-escala-de-octrees.md) |
| Escala | Vamos escalar as folhas cheias uniformemente em torno da origem e reconstruir uma nova octree | [ADR-0010](adrs/0010-uniao-e-escala-de-octrees.md) |

## Restrições operacionais

- Não vamos misturar requisitos obrigatórios e bônus nos critérios de conclusão.
- Não faremos benchmark para escolher parâmetros quando o trabalho não exigir isso.
- Vamos executar as operações booleanas e geométricas sobre as octrees, não sobre as primitivas originais.
- Não vamos acrescentar cabeçalhos ou campos ao formato de intercâmbio sem confirmar que fazem parte do padrão comum aos outros grupos.
- Vamos manter `docs/notas-md` como transcrição do material do professor, sem inserir decisões internas do projeto.

## Estado desta consolidação

Não há decisões pendentes neste momento.
