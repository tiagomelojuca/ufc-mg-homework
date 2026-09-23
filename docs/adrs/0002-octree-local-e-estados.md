# ADR-0002 — Octree local e estados

- Estado: Aceito
- Data: 2026-09-22

## Contexto

O trabalho permite escolher entre modelagem local e global. Escolhemos a estratégia local. Na aula, as células são classificadas como brancas, pretas ou cinzas, e apenas as células que interceptam parcialmente a primitiva são subdivididas.

## Decisão

Vamos implementar uma octree local/adaptativa:

- `W`: célula vazia e folha;
- `B`: célula cheia e folha;
- `(`: célula parcial/interna com exatamente oito filhos.

Vamos subdividir somente os nós parciais. A recursão termina quando encontramos uma célula cheia, uma célula vazia ou o limite de profundidade. Nesse limite, vamos seguir o algoritmo apresentado em aula e considerar o bloco terminal cheio.

## Consequências

- Representaremos regiões homogêneas com um único nó.
- A fronteira será uma aproximação dependente da profundidade.
- Ao considerar cheia uma célula de fronteira no último nível, podemos superestimar parte do sólido.
- Podemos compactar oito filhos homogêneos em uma folha equivalente.
