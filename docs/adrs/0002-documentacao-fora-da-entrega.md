# ADR-0002 — Documentação fora da entrega

- Estado: Aceito
- Data: 2026-10-17

## Contexto

A pasta `docs` contém board, arquitetura e ADRs. Esse material organiza a equipe, mas não interessa ao professor. Entregá-lo exigiria que ele explorasse decisões internas para avaliar o trabalho.

## Decisão

A pasta `docs` fica no repositório, mas não faz parte da entrega ao professor.

O código não menciona documentação, board, cards ou ADRs. Comentários descrevem apenas o próprio código, de forma clara e direta.

## Consequências

- A equipe compartilha a documentação pelo repositório.
- A entrega contém somente executável, bibliotecas, código-fonte e apresentação.
- O código é compreensível sem consulta à documentação interna.
- Justificativas relevantes para a avaliação são apresentadas na apresentação.
