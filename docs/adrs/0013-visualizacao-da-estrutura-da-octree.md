# ADR-0013 — Visualização da estrutura da octree

- Estado: Aceito
- Data: 2026-10-04

## Contexto

O aramado de T11 mostra somente as folhas cheias. O card bônus T17 pede a visualização da própria octree: nós parciais, vazios e cheios, com a hierarquia entre eles. A ADR-0005 previa começar os bônus somente depois de T16, mas decidimos antecipar T17. Os requisitos de graduação continuam fora da definição de concluído desse card.

Não queremos acrescentar mais controles ao painel lateral nem à área de visualização principal.

## Decisão

Vamos abrir a inspeção em uma janela flutuante da Dear ImGui, pelo botão “Estrutura da octree” no cabeçalho da visualização. A janela pode ser movida, redimensionada e fechada, e acompanha o modelo selecionado na lista.

A janela tem três partes:

- uma árvore navegável com o estado de cada nó e um tooltip com nível, centro e lado da região;
- uma tabela com a quantidade de nós `B`, `W` e `(` por nível da subárvore em foco;
- uma cena aramada com as caixas de todos os nós até o nível escolhido: parciais em âmbar, cheios em ciano e vazios em cinza, que podem ser ocultados.

Ao clicar em um nó da árvore, ele passa a ser o nó em foco. A cena é enquadrada na região desse nó, e a tabela passa a contar a partir dele.

`TGeradorAramadoOctree::GeraEstrutura` e `TAnaliseOctree::ContaNosPorNivel` ficam no núcleo e recebem qualquer nó como raiz. A interface só percorre a árvore para desenhar os itens expandidos.

A cena é desenhada pelo OpenGL dentro de um callback da lista de desenho da ImGui. Assim, ela respeita a ordem de composição das janelas: tooltips e janelas sobrepostas continuam visíveis. `TRenderizadorAramado` passa a aceitar lotes de arestas com cores diferentes e um retângulo de recorte. O desenho principal de T11 continua usando o mesmo caminho, agora com um único lote.

Os painéis fixos não vêm mais para a frente ao receber foco, para que a janela flutuante não fique escondida quando o usuário clica na lista de modelos.

## Consequências

- A inspeção não ocupa espaço na interface principal enquanto estiver fechada.
- Geramos a estrutura e as contagens somente quando o modelo, o nó em foco ou a quantidade de níveis mudam.
- A cena deixa de ser desenhada quando passa de 600 mil arestas; nesse caso, a janela pede menos níveis ou um nó mais profundo.
- O callback precisa desativar o shader da ImGui antes do pipeline fixo e pedir ao backend que restaure seu estado depois.
- Continuamos usando a vista ortográfica fixa e a API clássica do OpenGL.
