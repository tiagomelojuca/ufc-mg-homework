# Kanban — Modelador CSG

Os cards seguem a ordem de execução. Os bônus começam após a entrega de graduação.

Um card está concluído quando atende aos requisitos, compila e passa nos testes em Linux.

## Concluído

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| `C01` | Configurar o projeto | C++17, CMake, GLFW, OpenGL, Dear ImGui e GoogleTest. Compilação e testes em Linux. | Marcos |
| `C02` | Definir a árvore CSG | Nós de primitiva, operação booleana e movimento rígido. Primitivas sempre folhas. | Marcos |
| `C03` | Gerar bloco e esfera | Primitivas canônicas parametrizadas. Classificação de ponto e de aresta contra cada primitiva. | Marcos |
| `C04` | Aplicar a modelagem local | Translação e rotação como nós da árvore. Entidade levada ao sistema do filho pela inversa. | Marcos |
| `C05` | Salvar e ler árvores | Formato do quadro. Rejeição de arquivos inválidos. | Marcos |
| `C06` | Implementar a união | Operação sobre as árvores, não sobre as primitivas. | Luis |
## Em andamento

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| — | — | — | — |

## A fazer — graduação

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| `C07` | Implementar escala e translação | Escala pelos parâmetros da primitiva. Translação como nó da árvore. | A definir |
| `C08` | Calcular o volume | Conversão em octree. Classificação pelo centro da célula. | A definir |
| `C09` | Implementar a classificação SMC | `Tree_M`, `Prim_M` e `Combine_M` regularizado. Partes IN, ON e OUT. | A definir |
| `C10` | Renderizar por ray-casting | Primeira interseção do raio com a árvore. Iluminação pela normal. | A definir |
| `C11` | Exibir a SMC graficamente | Cores distintas para IN, ON e OUT. | A definir |
| `C12` | Integrar operações à interface | Criação, operações, análise e arquivos acessíveis pela interface. | A definir |
| `C13` | Construir o tema | Ford Escort em 3D com as operações implementadas. | A definir |
| `C14` | Validar o MVP | Fluxo completo e testes aprovados em Linux. | A definir |
| `C15` | Revisar antes da entrega | Conferir cada requisito contra enunciado e aulas. Verificar as deduções listadas abaixo. Código sem referências internas. Comentários claros e diretos. | A definir |
| `C16` | Preparar a apresentação | Metodologia, modelagem e evidência de cada requisito. Falas separadas por integrante. Explicação clara e direta do código. | A definir |
| `C17` | Preparar a entrega | Executável, bibliotecas, código-fonte e apresentação em um único arquivo compactado. Sem a pasta `docs`. | A definir |

### Deduções a verificar em `C15`

As aulas não definem estes pontos. Adotamos deduções que precisam ser confirmadas antes da entrega.

| Dedução | Registro |
| --- | --- |
| Bloco e esfera centrados na origem. | ADR-0004 |
| Rotação `R = Rz · Ry · Rx`, em graus, conforme Mortenson. | ADR-0004 |
| Raiz do arquivo na última linha. | ADR-0005 |
| Parâmetros de `S` por primitiva. | ADR-0005 |
| `T` e `R` após linha booleana ou sobre nó já definido. | ADR-0005 |

## Bloqueado

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| — | — | — | — |

## Bônus — após `C17`

| ID | Título | Requisitos | Responsável |
| --- | --- | --- | --- |
| `C18` | Adicionar cilindro e interseção | Requisitos de mestrado. | A definir |
| `C19` | Calcular a área de superfície | Requisito de mestrado. Cálculo sobre a árvore. | A definir |
| `C20` | Renderizar em aramado | Requisito de mestrado. Segmentos `EonS`. | A definir |
| `C21` | Renderizar a árvore CSG | Visualização da árvore, além das primitivas. | A definir |
| `C22` | Adicionar cone, diferença e rotação | Requisitos de doutorado. Rotação como nó da árvore. | A definir |
| `C23` | Renderizar por ray-tracing | Requisito de doutorado. | A definir |
