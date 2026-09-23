# ADR-0006 — Tema inicial Ford Escort

- Estado: Aceito
- Data: 2026-09-22

## Contexto

Escolhemos fazer um Ford Escort. Mas é extremamente difícil representar esse carro apenas com as primitivas e operações que teremos no nível de graduação.

## Decisão

Vamos primeiro concluir tudo que o trabalho de graduação exige. Depois, com as ferramentas funcionando, veremos o quanto é trabalhoso produzir um Escort reconhecível e decidiremos se vamos manter o tema ou escolher outro mais adequado.

Não vamos ampliar nem bloquear o MVP apenas para viabilizar o Escort.

## Consequências

- Vamos implementar a octree e as funcionalidades obrigatórias sem dependência do tema.
- Depois de concluir o MVP, faremos uma validação simples da viabilidade do Escort.
- Se o custo for excessivo, poderemos trocar o tema sem alterar a arquitetura ou invalidar o trabalho concluído.
