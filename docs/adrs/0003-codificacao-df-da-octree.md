# ADR-0003 — Codificação DF da octree

- Estado: Aceito
- Data: 2026-09-22

## Contexto

A aula define uma representação DF, em profundidade primeiro, usando `B` para blocos cheios, `W` para blocos vazios e `(` para blocos parciais. O exemplo apresentado é `(BWWBBW(BWWBBBWWB`.

O professor também informou em sala que os arquivos precisam ser compatíveis entre os modeladores dos diferentes grupos. Por isso, não podemos acrescentar informações próprias ao arquivo só porque seriam úteis para o nosso programa.

## Decisão

Vamos usar a codificação DF do professor como formato textual de intercâmbio da árvore. O arquivo terá somente essa string, sem cabeçalho próprio, versão, domínio, profundidade ou outros metadados. Só acrescentaremos alguma dessas informações se o professor confirmar que ela faz parte do formato comum.

Vamos interpretar cada `B` ou `W` como o fim de um ramo. Cada `(` abre um nó interno e precisa ser seguido pela codificação de exatamente oito filhos na ordem de octantes adotada na aula.

Nosso decodificador vai ler a entrada inteira e rejeitar caracteres desconhecidos, filhos ausentes, filhos excedentes ou dados depois do término da raiz.

## Consequências

- Conseguiremos reconstruir a árvore sem separadores ou marcadores de fechamento.
- Precisamos usar a mesma ordem dos oito filhos e a mesma interpretação espacial da raiz que os outros modeladores, embora essas informações não apareçam na string.
- Não vamos misturar metadados internos com o arquivo de intercâmbio.
- Se precisarmos guardar informações adicionais no futuro, usaremos um formato separado ou confirmaremos antes a mudança com o professor e os outros grupos.
