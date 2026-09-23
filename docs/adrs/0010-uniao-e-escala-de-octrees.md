# ADR-0010 — União e escala de octrees

- Estado: Aceito
- Data: 2026-09-23

## Contexto

O trabalho exige que a união e a escala produzam novas octrees a partir das árvores, sem recuperar ou transformar as primitivas que deram origem a elas.

O professor apresenta a união por percurso sincronizado, de cima para baixo, com resolução dos casos entre folhas e recursão quando os dois nós são parciais. Para a escala, o material determina o uso das matrizes usuais de transformação, mas não fornece um algoritmo específico para reconstruir a octree transformada.

## Decisão

Vamos implementar a união percorrendo as duas árvores de forma sincronizada. Um nó cheio domina a união; um nó vazio devolve uma cópia da outra região; dois nós parciais são combinados recursivamente pelos oito filhos correspondentes.

As árvores precisam representar o mesmo domínio, mas podem ter profundidades máximas diferentes. O resultado usará a maior profundidade configurada. Depois de cada combinação recursiva, vamos substituir oito filhos homogêneos por uma única folha.

Vamos implementar a escala uniforme em torno da origem com um fator finito e positivo. Cada folha cheia será tratada como a célula volumétrica efetivamente representada pela octree. Aplicaremos a transformação ao centro e ao lado dessa célula e inseriremos a região transformada em uma nova árvore com o mesmo domínio e a mesma profundidade máxima.

Vamos rejeitar a escala quando uma célula cheia transformada ultrapassar o domínio. Não vamos truncar o resultado.

## Consequências

- União e escala não alteram as árvores recebidas.
- A união aceita níveis de detalhe diferentes sem converter a representação para primitivas.
- A escala transforma a aproximação armazenada na octree, inclusive as células terminais que foram consideradas cheias durante a construção.
- Contrações e expansões são reamostradas na profundidade configurada.
- Fatores negativos não fazem parte desta operação, pois combinariam escala com reflexão ou inversão.
- Uma escala anisotrópica poderá ser adicionada depois sem alterar a escala uniforme obrigatória.
