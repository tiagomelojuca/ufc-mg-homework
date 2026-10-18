# ADR-0007 — Volume por conversão em octree

- Estado: Aceito
- Data: 2026-10-18

## Contexto

O enunciado exige o cálculo de volume sobre a árvore. A aula "CSG: Set Membership Implementação" trata o volume como propriedade aproximada, obtida por conversão de modelos: a árvore CSG gera uma octree por classificação ponto–sólido.

## Decisão

Calculamos o volume convertendo a árvore CSG em octree:

- cada célula é classificada pelo seu centro;
- a classificação do ponto consulta a árvore CSG, com a mesma recursão da SMC;
- o volume é a soma dos volumes das folhas cheias.

A profundidade da octree controla a precisão da aproximação.

## Consequências

- O volume depende apenas da árvore CSG, não de fórmulas das primitivas.
- O resultado é uma aproximação; a apresentação mostra sua convergência com a profundidade.
- A octree do trabalho anterior serve de referência para a construção recursiva.
