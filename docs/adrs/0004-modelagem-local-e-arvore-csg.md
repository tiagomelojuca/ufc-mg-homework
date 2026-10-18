# ADR-0004 — Modelagem local e árvore CSG

- Estado: Aceito
- Data: 2026-10-18

## Contexto

O enunciado pede a escolha entre modelagem local e global; em sala, escolhemos a local. A aula "CSG: Introdução e Conceitos" define os dois termos:

- local (opção 1): a transformação geométrica é incorporada à árvore, como em `<cyl><translação>`;
- global (opção 2): a transformação é incorporada à matriz da primitiva.

A mesma aula define a gramática da árvore:

```text
<CSG tree> ::= <primitive> | <CSG tree> <set operation> <CSG tree> | <CSG tree> <rigid motion>
```

A primitiva é um identificador de tipo com uma sequência de parâmetros de dimensão. O movimento rígido é translação ou rotação.

## Decisão

A árvore CSG tem três tipos de nó:

- primitiva: sempre folha, com tipo e parâmetros de dimensão;
- operação booleana: nó interno com dois filhos (união, interseção, diferença);
- movimento rígido: nó interno com um filho (translação, rotação).

As primitivas são definidas em posição canônica. Translação e rotação são nós da árvore, nunca parte da primitiva. A escala altera os parâmetros de dimensão, conforme o enunciado, e não é um nó.

As operações booleanas são regularizadas, para que o resultado seja sempre um sólido regular.

O cilindro segue a aula "Semi-espaços": eixo `z`, raio `r`, base em `z = 0` e topo em `z = h`, isto é, `C = H1 ∩ H2 ∩ H3`, com `H1: x² + y² − r² ≤ 0`, `H2: z ≥ 0` e `H3: z − h ≤ 0`. O slide traz `z − h ≥ 0`; o professor corrigiu o sinal em sala.

## Consequências

- Operações booleanas e geométricas apenas criam nós; não alteram as entradas.
- Algoritmos recursivos tratam três casos: primitiva, operação booleana e movimento rígido.
- Em um nó de movimento rígido, a classificação leva a entidade ao sistema do filho pela transformação inversa.

As aulas não definem a posição canônica do bloco e da esfera nem a convenção de rotação. Deduzimos:

- bloco centrado na origem, com lados paralelos aos eixos;
- esfera centrada na origem;
- rotação conforme Mortenson, Apêndice C, "Successive Rotations about the Principal Axes" (eq. C.32): primeiro em torno de `x`, depois de `y`, depois de `z`, ou seja, `R = Rz · Ry · Rx`; ângulos positivos no sentido anti-horário, pela regra da mão direita.

Essas deduções devem ser verificadas antes da entrega, pois afetam a troca de arquivos entre grupos.
