# ADR-0015 — Bônus de mestrado

- Estado: Aceito
- Data: 2026-10-04

## Contexto

O card T18 reúne os requisitos do nível de mestrado: cilindro, interseção, translação, área superficial e iluminação local. Assim como T17, decidimos antecipá-lo em relação à ordem da ADR-0005; ele continua fora da definição de concluído da graduação.

Seguimos o critério da ADR-0005: quando o enunciado não exige sofisticação, escolhemos a alternativa mais simples e portátil.

## Decisão

### Cilindro

O cilindro é alinhado a um dos eixos X, Y ou Z e é parametrizado por centro, raio, altura e eixo; o padrão é o eixo Y. A classificação separa o eixo do cilindro dos dois eixos radiais. No eixo, comparamos intervalos como no bloco. Na seção transversal, comparamos o raio com as distâncias do ponto mais próximo e do mais distante da célula, como na esfera. O contato apenas pela fronteira continua sendo considerado vazio.

Não implementamos cilindros com orientação arbitrária, pois combinariam a primitiva com uma rotação, que pertence ao nível de doutorado.

### Interseção

A interseção percorre as duas árvores em ordem sincronizada, como a união da ADR-0010, com as regras invertidas: um nó vazio domina, um nó cheio devolve uma cópia da outra região e dois nós parciais são combinados pelos oito filhos. As árvores precisam ter o mesmo domínio, o resultado usa a maior profundidade configurada e oito filhos homogêneos são compactados em uma folha.

### Translação

A translação desloca cada folha cheia por um vetor finito e insere a região deslocada em uma nova árvore, com o mesmo domínio e a mesma profundidade. É o mesmo procedimento da escala, que passa a compartilhar o código. Deslocamentos que não coincidem com a grade são reamostrados na profundidade configurada, e um resultado fora do domínio é rejeitado.

### Área superficial

A área superficial é a área do contorno das células cheias, isto é, da aproximação representada pela octree, assim como o volume da ADR-0012. Ela não é a área analítica da primitiva.

`TGeradorSuperficieOctree` passa a emitir exatamente as partes expostas das faces. Para cada face de uma folha cheia, localizamos a região vizinha de mesmo tamanho. Se ela pertencer a uma folha cheia, a face fica encoberta. Se pertencer a uma folha vazia ou estiver fora do domínio, a face fica exposta. Se for um nó parcial, descemos pelos quatro filhos voltados para a face, recursivamente. A área é a soma das faces geradas e é calculada uma vez, quando o modelo entra na lista.

Com isso, o modo sólido da ADR-0014 também deixa de desenhar faces encobertas por vizinhos menores.

### Iluminação local

Implementamos o modelo de Phong no núcleo, em `TIluminacaoPhong`, sem depender do OpenGL:

`I = ka + kd (N·L) + ks (R·V)^n`

A luz é pontual e branca. Ela fica acima, à frente e à esquerda do observador, numa posição proporcional ao domínio. Como a projeção é ortográfica, a direção do observador é constante e vem dos mesmos ângulos da câmera fixa. O renderizador calcula a cor em cada vértice das faces e o OpenGL interpola essas cores, no sombreamento de Gouraud. Os coeficientes ficam fixos em `TMaterialPhong`.

A iluminação só se aplica ao modo sólido. O checkbox “Iluminação” fica ao lado de “Sólido”, começa marcado e fica desabilitado no aramado. Desmarcado, o modo sólido volta aos tons fixos por orientação da ADR-0014.

### Interface

- A aba Criar oferece o cilindro, com raio, altura e eixo.
- A aba Operar tem os botões “Unir” e “Intersectar” para os mesmos dois modelos e uma seção de translação para o modelo selecionado.
- A visualização ganha um quarto cartão, com a área superficial.

## Consequências

- Todas as operações continuam trabalhando sobre as octrees, sem recuperar as primitivas.
- Interseção e translação preservam as entradas e acrescentam resultados à lista, como a união e a escala.
- A área depende da profundidade: em superfícies curvas, o contorno em degraus é maior que a área analítica, e essa diferença não diminui quando a profundidade aumenta.
- Uma face pode virar várias faces menores quando encosta em vizinhos menores; isso aumenta a quantidade de faces em modelos detalhados.
- Como calculamos a iluminação na CPU, os parâmetros do modelo podem ser testados sem contexto gráfico.
- Os parâmetros do material e a posição da luz não são editáveis na interface nesta versão.
