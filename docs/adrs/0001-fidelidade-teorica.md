# ADR-0001 — Fidelidade teórica

- Estado: Aceito
- Data: 2026-10-17

## Contexto

O trabalho é avaliado pelo professor Joaquim Bento. Existem algoritmos mais eficientes do que os apresentados em sala, mas eles podem afastar a implementação do conteúdo avaliado.

## Decisão

Adotamos a fidelidade teórica como critério principal. Seguimos esta ordem de referência:

1. enunciado e orientações do professor;
2. algoritmos, representações e exemplos das notas de aula;
3. Hoffmann e Mortenson, para esclarecer lacunas;
4. ADRs do projeto;
5. práticas gerais de engenharia de software.

Não substituímos um algoritmo da aula por outro mais eficiente. Otimizações só entram quando não ocultam o modelo ensinado.

## Consequências

- O código mantém reconhecíveis as estruturas e os passos das aulas.
- Ambiguidades das notas são registradas em ADR antes da implementação.
- A apresentação relaciona cada parte do código ao conteúdo da aula.
