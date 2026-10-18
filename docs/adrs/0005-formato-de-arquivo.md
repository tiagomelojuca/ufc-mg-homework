# ADR-0005 — Formato de arquivo

- Estado: Aceito
- Data: 2026-10-18

## Contexto

O enunciado exige salvar e ler árvores em formato definido em sala. O arquivo precisa ser compatível entre os modeladores dos grupos. O professor apresentou a notação no quadro:

```text
No1; C; S param; T x,y,z
No2; B; S param; T x,y,z; R x,y,z
No3; U No1 No2
```

Códigos definidos:

| Categoria | Operação | Código |
| --- | --- | --- |
| Primitiva | Bloco, esfera, cilindro | `B`, `E`, `C` |
| Booleana | União, interseção, diferença | `U`, `I`, `D` |
| Geométrica | Escala, translação, rotação | `S`, `T`, `R` |

## Decisão

Adotamos a notação do quadro, sem campos próprios:

- uma linha por nó, iniciada pelo nome do nó;
- campos separados por `;`;
- linha de primitiva: nome, código, `S` com os parâmetros de dimensão e, opcionalmente, `T` e `R`;
- linha booleana: nome, código e os nomes dos dois operandos;
- um nó só referencia nós definidos em linhas anteriores.

`T` e `R` são lidos como nós de movimento rígido aplicados na ordem em que aparecem, conforme a gramática `<CSG tree> <rigid motion>`.

Decisões deduzidas pela equipe, a verificar antes da entrega:

- a raiz é o nó da última linha;
- coordenadas separadas por vírgula;
- parâmetros de `S`: bloco com três lados, esfera com raio, cilindro com raio e altura;
- `R x,y,z` são ângulos em graus, na convenção da ADR-0004;
- `T` e `R` também podem seguir uma linha booleana, aplicando-se ao resultado da operação: `No3; U No1 No2; T x,y,z`;
- para mover um nó já definido, a linha usa o nome dele no lugar do código: `No4; No3; T x,y,z`.

As duas últimas regras estendem o quadro mantendo sua forma: o segundo campo descreve uma árvore e os campos seguintes são movimentos rígidos aplicados a ela.

## Consequências

- A leitura rejeita códigos desconhecidos, referências inexistentes e quantidade errada de parâmetros.
- Arquivos que usam só a notação do quadro continuam válidos.
- Qualquer mudança no formato exige acordo com os outros grupos.
