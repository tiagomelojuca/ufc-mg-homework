# ADR-0011 — Renderização aramada das folhas

- Estado: Aceito
- Data: 2026-10-01

## Contexto

O enunciado exige renderização em aramado para graduação. No slide 2 de “Subdivisão Espacial: Implementação Octree”, o gerador de imagem recebe a estrutura representada pela octree. No slide 15, o professor apresenta “simplesmente mostrar as folhas” e “mostrar só as arestas dos nós”. Ray tracing também aparece como exemplo, mas não é obrigatório no enunciado.

## Decisão

Vamos gerar o aramado por percurso recursivo: ignoramos nós vazios, emitimos as 12 arestas da região de cada folha cheia e visitamos os oito filhos dos nós parciais, na ordem armazenada.

`TGeradorAramadoOctree` produz segmentos sem depender de OpenGL. `TRenderizadorAramado` desenha esses segmentos com `GL_LINES`, usando OpenGL compatível com o contexto 3.0 já solicitado pela janela. Estamos usando uma vista ortográfica fixa, inclinada para mostrar as três dimensões.

O aramado é gerado quando o modelo é criado ou alterado. A janela reutiliza os segmentos nos quadros seguintes. Em T11, mostramos bloco e esfera lado a lado; os controles de criação e operações ficam em T12.

## Consequências

- O percurso segue os estados e as regiões guardadas na octree, inclusive após união e escala.
- Nós parciais e vazios não recebem caixas de visualização. A inspeção da estrutura completa continua no bônus T17 ([ADR-0013](0013-visualizacao-da-estrutura-da-octree.md)).
- Arestas internas e compartilhadas entre folhas são mantidas. Essa versão não extrai a superfície nem remove linhas ocultas.
- A esfera mostra a aproximação por células definida pela profundidade.
- Não precisamos acrescentar dependências gráficas.
- O desenho usa a API clássica de OpenGL e precisa de um contexto com suporte a essa API; um perfil core exigiria outro renderizador.
- Os testes geométricos continuam independentes de janela e contexto gráfico.
