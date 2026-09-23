# ADR-0009 — Domínio, profundidade e ordem dos octantes

- Estado: Aceito
- Data: 2026-09-22

## Contexto

A raiz da octree representa o universo do modelo. Para que duas árvores possam participar das mesmas operações e para que a string DF tenha a mesma interpretação em outros modeladores, precisamos usar um domínio e uma ordem de filhos bem definidos.

A profundidade também afeta diretamente a aproximação do sólido. Queremos um padrão simples, mas sem espalhar esse valor pelo código ou dificultar uma mudança futura.

## Decisão

Vamos usar como domínio padrão o cubo fechado `[-1, 1] × [-1, 1] × [-1, 1]`. Vamos representar esse domínio em uma configuração do núcleo, usada pela construção, pelas operações, pela persistência e pela renderização. Não vamos repetir os limites como literais em vários componentes.

Vamos rejeitar, com uma mensagem clara, primitivas ou transformações cujo resultado ultrapasse o domínio. Assim, não vamos truncar partes do modelo sem avisar.

Usaremos profundidade máxima padrão `5`. Esse valor ficará na mesma configuração e poderá ser alterado pela interface antes da construção da árvore.

Vamos seguir a convenção de octantes apresentada na aula:

| Filho | X | Y | Z | Posição |
| --- | --- | --- | --- | --- |
| 0 | inferior | inferior | inferior | esquerda, frente, baixo |
| 1 | superior | inferior | inferior | direita, frente, baixo |
| 2 | inferior | inferior | superior | esquerda, frente, alto |
| 3 | superior | inferior | superior | direita, frente, alto |
| 4 | inferior | superior | inferior | esquerda, fundo, baixo |
| 5 | superior | superior | inferior | direita, fundo, baixo |
| 6 | inferior | superior | superior | esquerda, fundo, alto |
| 7 | superior | superior | superior | direita, fundo, alto |

No código, o índice corresponde a `x + 2z + 4y`, considerando `0` para a metade inferior e `1` para a metade superior de cada eixo.

## Consequências

- Conseguiremos trocar domínio e profundidade alterando uma configuração, sem reescrever os algoritmos.
- A interface começará com profundidade `5`, mas permitirá outro valor.
- Só combinaremos árvores que tenham o mesmo domínio.
- A ordem dos oito filhos será compartilhada pela construção, pela codificação DF, pelas operações e pela renderização.
- O arquivo de intercâmbio continuará contendo somente a string DF; domínio e profundidade padrão fazem parte da convenção do modelador, não de um cabeçalho particular.

