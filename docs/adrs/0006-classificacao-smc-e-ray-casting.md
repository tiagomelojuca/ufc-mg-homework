# ADR-0006 — Classificação SMC e ray-casting

- Estado: Aceito
- Data: 2026-10-18

## Contexto

O enunciado exige renderização por ray-casting com SMC de aresta contra a árvore CSG e a exibição gráfica das partes IN, ON e OUT. As aulas "CSG: Set Membership Classification" e "CSG: Set Membership Implementação" apresentam o algoritmo por divisão e conquista.

## Decisão

A classificação segue `Tree_M(S, E)`:

- se `S` é primitiva, retorna `Prim_M(S, E)`;
- se `S` é operação booleana, retorna `Combine_M(Tree_M(S->Left, E), Tree_M(S->Right, E), S->Op)`.

O nó de movimento rígido, exigido pela modelagem local, classifica a aresta transformada pela inversa do movimento contra o filho.

`Prim_M` usa as equações da primitiva sobre a aresta paramétrica `E(t) = p1 + t(p2 − p1)`, com `t ∈ [0, 1]`:

- bloco: seis testes raio–plano;
- esfera: um teste quadrático;
- cilindro: dois testes raio–plano e um quadrático.

`Combine_M` segue os passos da aula: ordenar os segmentos de cada classificação, aplicar a operação booleana segmento a segmento e juntar os resultados. A combinação é regularizada.

O ray-casting usa a mesma recursão com uma aresta semi-infinita, o raio, e toma a primeira interseção. A cor resulta de um modelo de iluminação com normal, propriedades da superfície e posição da luz.

A exibição da SMC classifica arestas finitas contra a árvore e desenha cada parte com a cor de IN, ON ou OUT.

## Consequências

- Ray-casting e exibição da SMC compartilham `Prim_M` e `Combine_M`.
- Cada primitiva implementa a interseção com a aresta e a normal no ponto.
- Testes cobrem cada primitiva, cada operação booleana e cada movimento rígido.
