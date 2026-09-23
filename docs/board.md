# Kanban — Trabalho 1

O backlog está em ordem de execução. Os bônus só começam depois da entrega de graduação.

Um card está concluído quando atende ao critério indicado e passa pela compilação e pelos testes em Linux.

## Concluído

| ID | Card | Responsável |
| --- | --- | --- |
| `T01` | Organizar notas, decisões, ADRs e arquitetura | Marcos |
| `T02` | Configurar C++17, CMake, GLFW, OpenGL e Dear ImGui | Thiago |
| `T03` | Criar janela OpenGL com Dear ImGui | Thiago |
| `T04` | Configurar GoogleTest | Thiago |

## Em andamento

Nenhum card.

## A fazer — graduação

| ID | Card | Responsável | Critério de aceitação |
| --- | --- | --- | --- |
| `T05` | Implementar a estrutura da octree | A definir | Nós `B`, `W` e parcial; domínio `[-1,1]³`; profundidade padrão `5`; oito filhos na ordem definida |
| `T06` | Gerar bloco e esfera | A definir | Construção local respeita domínio e profundidade |
| `T07` | Salvar e carregar a representação DF | A definir | Leitura e escrita compatíveis com o formato do professor |
| `T08` | Implementar união | A definir | União opera sobre octrees de mesmo domínio |
| `T09` | Implementar escala | A definir | Escala gera uma nova octree e respeita o domínio |
| `T10` | Calcular volume | A definir | Volume calculado pelas folhas cheias |
| `T11` | Renderizar em aramado | A definir | Bloco e esfera podem ser visualizados |
| `T12` | Integrar operações à interface | A definir | Profundidade editável; criação, arquivo, união, escala e volume acessíveis |
| `T13` | Definir e construir o tema | A definir | Testar o Ford Escort e trocar o tema se ele não for viável |
| `T14` | Validar o MVP | A definir | Fluxo completo e testes aprovados em Linux |
| `T15` | Preparar apresentação e demonstração | A definir | Apresentação comprova os requisitos sem depender da execução |
| `T16` | Preparar entrega | A definir | Executável, bibliotecas, código e apresentação em um único arquivo |

## Bloqueado

Nenhum card.

## Bônus — após `T16`

| ID | Nível | Card |
| --- | --- | --- |
| `T17` | Extra | Visualizar a octree |
| `T18` | Mestrado | Adicionar cilindro, interseção, translação, área superficial e iluminação local |
| `T19` | Doutorado | Adicionar cone, diferença e rotação |
| `T20` | Doutorado | Avaliar modelagem 4D com tempo |
| `T21` | Doutorado | Avaliar iluminação global |
