# ADR-0014 — Visualização sólida opcional

- Estado: Aceito
- Data: 2026-10-04

## Contexto

O aramado de T11 mostra todas as arestas das folhas cheias, inclusive as internas. Em modelos com muitas células, as linhas se sobrepõem e fica difícil perceber a forma. O aramado continua sendo o requisito de graduação e precisa ser a visualização padrão.

## Decisão

Vamos oferecer um modo sólido opcional, ativado pelo checkbox “Sólido” no cabeçalho da visualização. O checkbox começa desmarcado e a escolha não altera os modelos.

`TGeradorSuperficieOctree`, no núcleo, gera as faces das folhas cheias e omite as faces totalmente encostadas em outra região cheia. Para cada face, localizamos a folha que fica logo depois dela. Se essa folha for cheia e tiver lado maior ou igual, ela cobre a face inteira, pois as regiões da octree são alinhadas. Quando o vizinho é menor, mantemos a face, porque ele pode cobri-la apenas em parte.

`TModeloOctree` prepara as faces junto com o aramado e o volume. `TRenderizadorAramado::DesenhaSolido` preenche as faces com um tom constante por orientação e desenha as arestas em uma cor escura por cima. O deslocamento de polígonos deixa visíveis somente as arestas que não estão encobertas. As duas visualizações compartilham a mesma configuração de câmera e de estado do OpenGL.

Os tons por orientação só distinguem os lados do modelo. Eles não constituem um modelo de iluminação; a iluminação local continua no bônus T18.

A janela de estrutura da octree (ADR-0013) continua somente em aramado.

## Consequências

- O aramado continua sendo a visualização padrão.
- A forma fica legível sem esconder a divisão em células, pois as arestas visíveis continuam desenhadas.
- Omitimos a maior parte das faces internas; ainda podem restar faces encostadas em vizinhos menores, escondidas pelo teste de profundidade. A [ADR-0015](0015-bonus-de-mestrado.md) passou a gerar somente as partes expostas das faces.
- Cada modelo guarda as faces, além das arestas.
- Continuamos com a vista ortográfica fixa e a API clássica do OpenGL.
