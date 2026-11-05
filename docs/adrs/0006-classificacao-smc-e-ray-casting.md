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

A regularização usa a vizinhança dos segmentos ON. Cada segmento ON leva as normais exteriores das faces que o contêm: uma sobre uma face, duas sobre uma aresta do bloco. Nos nós de movimento rígido, as normais voltam ao sistema do pai pela transposta da inversa. Quando os dois lados de um trecho são ON, as vizinhanças são unidas no plano perpendicular à aresta. Se a união cobre todas as direções, como em faces encostadas com normais opostas, o trecho passa a IN; caso contrário continua ON com a vizinhança unida.

Na interseção, as vizinhanças são intersectadas cone a cone: a interseção de dois cones junta as suas normais. Cones sem abertura são descartados; se nenhum resta, como em faces encostadas com normais opostas, o trecho passa a OUT. Caso contrário, continua ON com os cones que restaram.

No cilindro, um trecho ON sobre uma base leva a normal `±z`; sobre a superfície lateral, a normal radial.

O ray-casting usa a mesma recursão com uma aresta semi-infinita, o raio, e toma a primeira interseção. A cor resulta de um modelo de iluminação com normal, propriedades da superfície e posição da luz.

O raio é representado por uma aresta finita que parte da câmera e passa de toda a cena, o que equivale à aresta semi-infinita para a primeira interseção. A interseção é o início do primeiro trecho não OUT; um raio que parte de dentro do sólido não gera interseção. A normal no ponto vem da primitiva cuja fronteira contém o ponto e volta ao sistema do pai pela transposta da inversa de cada movimento rígido. A iluminação segue o modelo de Phong, com termos ambiente, difuso e especular.

A exibição da SMC classifica arestas finitas contra a árvore e desenha cada parte com a cor de IN, ON ou OUT.

As arestas são projetadas com a mesma câmera do ray-casting e desenhadas sobre a imagem, sem ocultação pelo sólido, para que as partes IN fiquem visíveis. IN é verde, ON é amarelo e OUT é vermelho. A parte de uma aresta atrás da câmera é recortada antes da projeção.

## Consequências

- Ray-casting e exibição da SMC compartilham `Prim_M` e `Combine_M`.
- Cada primitiva implementa a interseção com a aresta e a normal no ponto.
- Testes cobrem cada primitiva, cada operação booleana e cada movimento rígido.
