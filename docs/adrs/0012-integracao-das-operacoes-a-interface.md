# ADR-0012 — Integração das operações à interface

- Estado: Aceito
- Data: 2026-10-01

## Contexto

Precisamos acessar criação, profundidade, arquivos, união, escala e volume pela interface. A união recebe duas octrees; a composição do tema exige guardar primitivas e resultados intermediários.

## Decisão

Vamos manter uma lista de modelos com identificadores estáveis. Cada modelo guarda nome, octree, aramado e volume. Selecionamos automaticamente cada novo modelo. União e escala acrescentam resultados à lista e preservam as entradas. A remoção mantém os identificadores dos modelos restantes.

`TModelador`, em `src/Application`, coordena as operações existentes no núcleo e na persistência. Ele prepara o aramado e calcula o volume quando acrescenta um modelo. `TPainelModelador`, em `src/UI`, coleta parâmetros e apresenta os resultados. Continuamos usando a estratégia e os hooks da janela.

Os modelos armazenam octrees imutáveis com propriedade compartilhada. Assim, podemos copiar o estado da estratégia pelo contrato `Copia()` existente, sem duplicar árvores nem permitir alterações indiretas nas entradas.

A interface terá uma lista de modelos, abas Criar, Operar e Arquivo, uma área de visualização do modelo selecionado e uma faixa de mensagens. Vamos usar tema escuro, destaque em ciano e cartões para volume, profundidade e quantidade de células cheias.

A profundidade padrão continua 5. O controle interativo aceita de 1 a 8 e afeta somente novas criações e leituras. Essa faixa é uma escolha da aplicação para limitar o crescimento de árvores durante o uso interativo; o núcleo mantém sua configuração independente desse limite. União usa a maior profundidade das entradas, e escala mantém a profundidade do modelo original.

Vamos abrir e salvar pelo caminho informado em um campo de texto. Caminhos relativos usam o diretório de execução. A gravação de um arquivo existente exige confirmação. O arquivo continua contendo apenas a string DF do professor, sem nome, domínio ou profundidade.

## Consequências

- Os algoritmos geométricos continuam fora da interface.
- Falhas de criação, operação ou leitura preservam a lista e a seleção atuais.
- O volume mostrado corresponde às células da octree, não ao volume analítico da primitiva.
- Trocar a profundidade não reconstrói árvores existentes.
- Os modelos podem ser usados em novas operações e removidos depois, sem invalidar resultados já produzidos.
- A interface começa vazia e permite construir o modelo pelo fluxo de criação ou leitura.
- Continuamos usando a vista ortográfica fixa de T11.
- A lista de modelos não é um formato de projeto: salvamos uma octree por arquivo DF.
- Não acrescentamos dependências nem exigimos um seletor de arquivos nativo.
