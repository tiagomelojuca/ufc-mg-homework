# Kanban — Modelador CSG

Os cards seguem a ordem de execução. Os bônus começam após a entrega de graduação.

Um card está concluído quando atende aos requisitos, compila e passa nos testes em Linux.

## Concluído

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| `C01` | Configurar o projeto | C++17, CMake, GLFW, OpenGL, Dear ImGui e GoogleTest. Compilação e testes em Linux. | Marcos |

## Em andamento

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| — | — | — | — |

## A fazer — graduação

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| `C02` | Definir a árvore CSG | Folhas com primitivas. Nós internos com operações booleanas e transformações. Estrutura conforme a aula. | A definir |
| `C03` | Gerar bloco e esfera | Primitivas parametrizadas. Classificação de ponto e de segmento contra cada primitiva. | A definir |
| `C04` | Aplicar a técnica de modelagem | Técnica local ou global, conforme definida em sala. | A definir |
| `C05` | Salvar e ler árvores | Formato definido em sala. Rejeição de arquivos inválidos. | A definir |
| `C06` | Implementar a união | Operação sobre as árvores, não sobre as primitivas. | A definir |
| `C07` | Implementar escala e translação | Escala pelos parâmetros da primitiva. Translação por matriz. Operação sobre as árvores. | A definir |
| `C08` | Calcular o volume | Cálculo sobre a árvore, não sobre as primitivas. | A definir |
| `C09` | Implementar a classificação SMC | Classificação de segmento contra a árvore CSG em IN, ON e OUT. Combinação conforme a aula. | A definir |
| `C10` | Renderizar por ray-casting | Raios classificados por SMC. Exibição das primitivas. | A definir |
| `C11` | Exibir a SMC graficamente | Cores distintas para IN, ON e OUT. | A definir |
| `C12` | Integrar operações à interface | Criação, operações, análise e arquivos acessíveis pela interface. | A definir |
| `C13` | Construir o tema | Ford Escort em 3D com as operações implementadas. | A definir |
| `C14` | Validar o MVP | Fluxo completo e testes aprovados em Linux. | A definir |
| `C15` | Preparar a apresentação | Metodologia, modelagem e evidência de cada requisito. Falas separadas por integrante. Explicação clara e direta do código. | A definir |
| `C16` | Preparar a entrega | Executável, bibliotecas, código-fonte e apresentação em um único arquivo compactado. | A definir |

## Bloqueado

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| — | — | — | — |

## Bônus — após `C16`

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| `C17` | Adicionar cilindro e interseção | Requisitos de mestrado. | A definir |
| `C18` | Calcular a área de superfície | Requisito de mestrado. Cálculo sobre a árvore. | A definir |
| `C19` | Renderizar em aramado | Requisito de mestrado. | A definir |
| `C20` | Renderizar a árvore CSG | Visualização da árvore, além das primitivas. | A definir |
| `C21` | Adicionar cone, diferença e rotação | Requisitos de doutorado. Rotação por matriz. | A definir |
| `C22` | Renderizar por ray-tracing | Requisito de doutorado. | A definir |
