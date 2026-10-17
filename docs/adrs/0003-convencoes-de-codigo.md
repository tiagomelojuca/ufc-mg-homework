# ADR-0003 — Convenções de código

- Estado: Aceito
- Data: 2026-10-17

## Contexto

Cinco pessoas escrevem o código. O professor critica a clareza da explicação do código. Replicamos os padrões do trabalho anterior para manter consistência e facilitar a leitura.

## Decisão

### Ambiente

- C++17 e CMake, com dependências obtidas por `FetchContent`.
- GLFW, OpenGL e Dear ImGui. No Linux, X11 como backend padrão.
- GoogleTest para os testes.

### Organização

- `src/Core`: núcleo geométrico, sem dependência de GLFW, OpenGL ou Dear ImGui.
- `src/Application`: casos de uso e lista de modelos.
- `src/Persistence`: leitura e escrita de arquivos.
- `src/UI`: janela, painéis e renderização.
- `test`: um arquivo `<Classe>Test.cpp` por unidade testada.
- Declarações em `.h` e implementações em `.cpp`.
- Guardas de inclusão no formato `NOME_H_`.
- Funções auxiliares internas em namespace anônimo no `.cpp`.

### Nomes

- Nomes em português.
- Tipos com prefixo `T` (`TEsfera`) e enums com prefixo `E` (`EEstadoNoOctree`).
- Valores de enum em maiúsculas (`CHEIO`, `VAZIO`).
- Tipos e métodos em PascalCase. Atributos e variáveis em camelCase.
- Métodos de consulta sem prefixo `Get` (`Raio()`, `Centro()`).
- Testes no formato `TEST(<Classe>Test, deve_<comportamento>)`.

### Formatação

- Indentação de dois espaços.
- Chave de abertura na linha seguinte para classes e funções; na mesma linha para `if`, `for` e `while`.
- Seções `public` e `private` indentadas dentro da classe.
- Assinaturas com um parâmetro por linha.
- Lista de inicialização do construtor com um membro por linha.
- Separador `//------...` entre classes e entre funções.

### Projeto

- Composição para montar objetos e dependências recebidas pelo construtor.
- Strategy somente quando houver variação real de comportamento.
- `Copia()` como Prototype para cópia de comportamento polimórfico.
- Exclusão explícita (`= delete`) de cópias e construções inválidas.
- RAII e `std::unique_ptr` para propriedade exclusiva; sem `new` e `delete` em código novo.
- Pré-condições validadas com `std::invalid_argument` e mensagem em português.
- Objetos resultantes de operações não alteram as entradas.

### Comentários

- Sem cabeçalhos de arquivo.
- Sem referência a documentação, board, cards ou ADRs.
- Comentários curtos, somente em trechos não evidentes, como convenções geométricas e fórmulas.

## Consequências

- Código novo segue o mesmo estilo do trabalho anterior.
- Revisões rejeitam divergências destas convenções.
- Mudanças nestas convenções exigem uma nova ADR.
