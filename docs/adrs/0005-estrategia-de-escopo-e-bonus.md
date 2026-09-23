# ADR-0005 — Estratégia de escopo e bônus

- Estado: Aceito
- Data: 2026-09-22

## Contexto

O enunciado separa os requisitos de graduação, mestrado e doutorado. O professor informou que as funcionalidades dos níveis superiores concedem bônus. Queremos buscar esses bônus sem colocar a entrega obrigatória em risco.

## Decisão

Vamos tratar todo o escopo de graduação como o MVP do projeto. Não começaremos funcionalidades de bônus antes de concluir esse MVP: ele precisa compilar e executar em Linux, passar nos testes previstos e produzir evidências suficientes para a apresentação.

Depois disso, vamos avançar nesta ordem:

1. adicionar visualização da octree;
2. implementar bônus de mestrado;
3. implementar bônus de doutorado de custo controlado;
4. avaliar 4D/tempo e iluminação global somente depois da estabilidade dos níveis anteriores.

Os bônus não fazem parte da definição de concluído dos requisitos obrigatórios.

Quando houver mais de uma solução válida e o enunciado não exigir sofisticação, escolheremos a alternativa mais básica, simples e portátil. Não faremos benchmark para escolher parâmetros do modelo, a menos que apareça um requisito explícito de desempenho.

## Consequências

- Manteremos uma versão entregável antes de começar os bônus.
- Vamos identificar claramente na board o nível acadêmico e se cada card é obrigatório ou bônus.
- Não vamos deixar funcionalidades de alto risco bloquearem a entrega principal.
- Só faremos otimizações e generalizações quando houver um requisito ou problema observado que as justifique.
