# TP2 — A Consultoria do Blibblon: To-do list

> **Prazo: 24/11/2026, 23:59.** Prioridade: código funcionando no VPL (nota > 0 é obrigatória; vale 75%). Depois relatório.

---

## Fase 0 — Decisões de projeto (≈15 min, antes de codar)
- [x] Usar `long long` para valores, poder e prioridade, porque somas de muitos atributos podem estourar `int`.
- [x] Entrada: ler de `std::cin` (uso: `./bin/tp2.out < entrada.txt`).
- [x] **Chave do índice:** guardar o par `(poder, id)` como chave composta.
  - Na AVL, a ordem `(poder, id)` já sai pronta para o BF, e o BP vira uma faixa `[p, p]`.
  - Na Hash, a chave é o `poder` e cada entrada guarda a lista de ids daquele poder.
- [x] **Hash:** escolher e justificar no relatório.
  - Sugestão: encadeamento separado, com função de espalhamento que trate poder negativo (por exemplo, multiplicativa de Knuth sobre `unsigned long long`).
  - Rehash quando o fator de carga passar de 0,75.

## Fase 1 — TADs simples
- [x] **Atributo** (`Atributo.h/.cpp`): `id` e `nome`. Um `lib::vetor<Atributo>` global no sistema.
- [x] **Cliente** (`Cliente.h/.cpp`): `id`, `nome`, `poder` e `ativo`.
  - Build: um vetor de tamanho T inicializado com zero. Se o atributo se repetir, sobrescreve (o último vale).
  - Calcular o poder uma única vez, no construtor.
- [x] **ConjuntosDisjuntos**: `criar()`, chamado a cada `R`, mais `find` com compressão de caminho e `union` por rank.
- [x] **Mapa**: regiões (`id`, `nome`) e lista de adjacência com custo.
  - `adicionarRota(u, v, c)` nos dois sentidos.
  - `alcancavelBFS(u, v)` não entra na saída, mas é necessário para o experimento 4. Contar as regiões visitadas.
- [x] **FilaPrioridade**: heap máximo implementado à mão sobre `lib::vetor`.
  - Elemento `(prioridade, id_cliente)`.
  - Comparador: maior prioridade primeiro; no empate, menor id.
  - Operações: `inserir`, `removerMax`, `tamanho`, `vazia`.

## Fase 2 — Índice (o núcleo da nota)
- [x] **ArvoreAVL**:
  - [x] nó com `poder`, `id`, `altura`, `esq` e `dir`;
  - [x] rotações simples e duplas;
  - [x] inserção com rebalanceamento;
  - [x] **remoção com rebalanceamento** (caso de dois filhos usando o sucessor);
  - [x] busca de faixa em ordem, com poda: `faixa(min, max, callback/vetor de saída)`;
  - [x] destrutor liberando todos os nós;
  - [x] contadores de comparações, rotações e altura.
- [x] **TabelaHash**:
  - [x] inserir: acha o bucket do poder e põe o id em ordem crescente (os ids chegam crescentes, então na prática é um append);
  - [x] remover;
  - [x] busca exata;
  - [x] **faixa**: varrer todas as entradas, filtrar `[min, max]` e ordenar por `(poder, id)` com uma ordenação própria (merge sort ou quicksort, já que `std::sort` é proibido);
  - [x] rehash;
  - [x] contadores de colisões, comparações e fator de carga.
- [x] **Indice** (fachada): guarda o modo atual e um ponteiro para a AVL ou para a Hash.
  - Expõe `inserir`, `remover`, `buscaExata`, `faixa` e `migrar(modo)`.
  - `migrar(modo)`: percorre a estrutura atual, insere tudo na nova e destrói a antiga. Se já estiver no modo pedido, não faz nada.

## Fase 3 — `main.cpp` (parser e saída)
Para cada comando, escrever a saída exatamente no formato pedido, com espaços simples e sem espaço no fim da linha:
- [x] `E m` → migra e imprime `E m`. A primeira linha cria o índice.
- [x] `A nome` → `A id`
- [x] `C nome k pares...` → `C id poder`, e insere o cliente no índice.
- [x] `D id` → remove do índice e imprime `D nome poder`.
- [x] `BP p` → `BP p cliente_1 id ...`, ou `BP p VAZIO`.
- [x] `BF a b` → `BF a b cliente_1 id poder ...`, ou `BF a b VAZIO`.
- [x] `R nome` → `R id`, mais `criar` no conjunto disjunto.
- [x] `T u v c` → adiciona a rota no Mapa, faz `union(u, v)` e imprime `T nomeU nomeV c`.
- [x] `CC u v` → `CC nomeU nomeV SIM` ou `NAO`, respondido via union-find.
- [x] `F id p` → `F tamanho`
- [x] `Z k` → `Z n`, e a linha `atendimento_i id prio ...` só se `n > 0`.

## Fase 4 — Validação do código
- [x] Rodar o exemplo da seção 1.7 e comparar com `diff` contra a Figura 2.
- [x] Testar casos de borda:
  - poder negativo;
  - `C` com `k = 0`;
  - atributo repetido;
  - `Z` com a fila vazia;
  - `E` para o mesmo modo;
  - vários `E` alternados;
  - BP e BF depois de `D`;
  - cliente repetido na fila.
- [x] Teste aleatório contra modelo de referência em Python (300 entradas, modos A/H, migrações) — 0 falhas.
- [x] Rodar `make sanitize` e o valgrind para checar vazamentos e acessos inválidos.
- [x] Rodar `make clean && make all` com C++11, sem warnings.
- [x] **Submeter na VPL de testes** e depois na VPL oficial. Garanta isso cedo.

## Fase 5 — Instrumentação e experimentos
- [ ] Expor os contadores nos TADs com getters e `resetarEstatisticas()`.
- [ ] Criar um binário ou flag separada para os experimentos, para não poluir a saída do VPL. Usar `clock_gettime` ou `chrono` fora dos laços de alocação.
- [ ] Escrever um gerador de entradas parametrizável: nº de clientes, % de remoções, nº de poderes distintos, nº de regiões e rotas, mistura de operações.
- [ ] Rodar os experimentos:
  1. AVL × Hash por operação (inserção, remoção, BP, BF) e custo da migração;
  2. só entradas × entradas e saídas (rotatividade);
  3. perfil com predominância de atualizações × perfil com predominância de consultas;
  4. CC com union-find × CC com BFS, em mapas esparsos e densos, separando construção de consulta;
  5. variação de parâmetros;
  6. coletar comparações, rotações, colisões, altura, regiões visitadas e memória.
- [ ] Gerar os gráficos (Python/matplotlib) e as tabelas.

## Fase 6 — Relatório (template AAAI no Overleaf)
- [ ] Resumo
- [ ] Introdução
- [ ] Trabalhos Relacionados: Cormen, Ziviani e Knuth para hash
- [ ] Métodos: os TADs, a escolha de hash justificada, a migração
- [ ] Análise de Complexidade: tempo e espaço de cada operação nos dois modos, incluindo o BF da hash, que é O(n + m log m)
- [ ] Análise Experimental, incluindo a discussão qualitativa de localidade de referência da seção 2.1
- [ ] Conclusão
- [ ] Agradecimentos e Declaração de Uso de IA
- [ ] Referências
- [ ] Exportar o PDF e submeter.

## Fase 7 — Entrega e entrevista
- [ ] Rodar `make clean` para deixar `bin/` e `obj/` vazias e conferir a estrutura `TP/{src,bin,obj,include,Makefile}`.
- [ ] Revisar o próprio código. Saber reescrever de cabeça as rotações AVL, a remoção AVL, o `heapify` e o `find`/`union`, que são os trechos com mais chance de serem removidos na questão média.

---

**Sugestão de agenda para as próximas ~30h:**
- Fases 0 a 4 hoje e de manhã, porque são elas que destravam a nota.
- Fase 5 à tarde.
- Fase 6 à noite.
- Mesmo que os experimentos fiquem menores, um código funcionando no VPL vale 75% da nota.
