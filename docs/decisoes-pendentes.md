# Decisões pendentes

## Classificação das células terminais

`TOctree::ConstroiNo` segue literalmente o pseudocódigo do slide 10 de “Subdivisão Espacial: Implementação Octree”: no último nível, o estado vira `B` sem que a célula seja classificada ([ADR-0002](adrs/0002-octree-local-e-estados.md)). Com isso, os oito filhos de uma célula parcial do penúltimo nível ficam cheios, inclusive os que estão totalmente fora da primitiva.

A visualização sólida ([ADR-0014](adrs/0014-visualizacao-solida-opcional.md)) deixou o efeito evidente: uma esfera de raio `0,5` na profundidade `5` vira o cubo `[-0,5; 0,5]³`, com volume `1,0`, em vez de aproximadamente `0,52`.

Alternativa em avaliação: classificar também as células do último nível, manter vazias as que estiverem fora e marcar como cheias somente as parciais. Precisamos decidir se mantemos a leitura literal do algoritmo da aula ou se adotamos essa alternativa, que muda volumes, arquivos DF gerados e testes existentes.
