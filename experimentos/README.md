# Experimentos — TP2

Esta pasta fica só na branch `experimental` e **não faz parte da entrega do VPL**.

```
experimentos/
├── bench.cpp          programa de medição (usa os mesmos TADs de src/, sem o main)
├── graficos.py        lê os CSVs e gera gráficos (PDF/PNG) + resumo em tabelas
├── resultados/        CSVs brutos (uma linha por repetição) e resumo.md
├── graficos/          figuras prontas para o Overleaf (use os .pdf)
└── .pylibs/           matplotlib instalado localmente (ignorado pelo git)
```

## Como reproduzir

```bash
make rodar-experimentos      # compila bin/bench.out, roda tudo (3 repetições) e gera os gráficos
```

Partes separadas:

```bash
make experimentos                                    # só compila
./bin/bench.out <1|2|3|4|5|todos> experimentos/resultados [repeticoes]
PYTHONPATH=experimentos/.pylibs python3 experimentos/graficos.py
```

Se a pasta `.pylibs` não existir: `pip install --target experimentos/.pylibs matplotlib numpy`.

Tempo total da bateria completa: cerca de 2 minutos.

## Ambiente

| Item | Valor |
|---|---|
| CPU | Intel Core i5-4690S, 3,2 GHz (turbo 3,9 GHz), 4 núcleos |
| Cache | L1d 32 KiB/núcleo, L2 256 KiB/núcleo, L3 6 MiB compartilhado |
| Memória | 15 GiB |
| SO | Linux 6.17 (Ubuntu 24.04) |
| Compilador | g++ 13.3, `-std=c++11 -O2` (as mesmas flags do VPL) |
| Relógio | `std::chrono::steady_clock` |

## Cuidados de medição

- **Mesmo código da entrega.** O programa de medição chama diretamente os TADs de `src/`, compilados com as mesmas flags. A única mudança na biblioteca é `lib::vetor::esvaziar()`, que só o programa de medição usa.
- **Sem E/S nos laços medidos.** Toda a entrada (poderes, ordens de remoção, consultas, comandos) é gerada **antes** do laço medido, em vetores alocados uma única vez.
- **Sem alocação desnecessária no laço.** Os vetores de saída de BP e BF são reaproveitados com `esvaziar()`. As alocações que restam são as dos próprios algoritmos: nós da AVL e da hash, o vetor auxiliar do merge sort no BF da hash e o vetor temporário do BP na AVL.
- **Determinismo.** Usa um gerador pseudoaleatório próprio (splitmix64) com semente fixa por repetição. A mesma semente gera a mesma carga para AVL e Hash.
- **Repetições.** Cada configuração roda 3 vezes, com sementes diferentes. Os gráficos mostram a média, e as barras de erro mostram o desvio padrão.
- **Tempo por tipo de operação.** No Exp. 2 e na decomposição por comando do Exp. 3, o tempo é medido operação a operação. Isso soma o custo do próprio relógio (dezenas de ns), igual para as duas estruturas. Por isso também é reportado o tempo total **sem** instrumentação no laço.
- **Contadores dos TADs.** Comparações, rotações, colisões, altura, maior lista, passos do union-find e regiões visitadas vêm dos contadores internos de cada TAD (`getNum...()`, `resetarEstatisticas()`). São medidas independentes da máquina.
- **Memória.** É uma estimativa calculada pelo próprio TAD (`getMemoriaBytes()`): número de nós × tamanho do nó, mais os vetores. Não inclui o overhead do alocador.

## Parâmetros de cada experimento

### Exp. 1 — AVL × Hash por operação (`exp1_operacoes.csv`)
- n ∈ {10³, 10⁴, 10⁵, 10⁶} clientes. Poder uniforme em [0, n), então há poucos clientes por poder.
- Ordem: insere os n clientes, faz 10.000 BP com poder sorteado, faz BF com largura de 1% do intervalo de poderes e remove todos em ordem aleatória.
- Quantidade de BF: 2·10⁷/n consultas, limitada a [10, 2000].
- Migração: um `Indice` com n clientes no modo A é migrado A→H e depois H→A.

### Exp. 2 — Rotatividade (`exp2_rotatividade.csv`, `exp2_linha_tempo.csv`)
- n ∈ {10⁴, 10⁵, 10⁶}.
- Preenchimento, não medido: n inserções.
- Parte medida: n operações.
  - "só entradas": n novas inserções (o tamanho vai de n a 2n).
  - "entra e sai": cada operação é inserção ou remoção de um cliente ativo sorteado, com probabilidade ½ cada (o tamanho fica em torno de n).
- A linha do tempo registra, a cada 1/50 da execução, a altura da AVL e o comprimento da maior lista da hash.

### Exp. 3 — Perfis de carga (`exp3_perfis.csv`)
- Estado inicial, não medido:
  - 20.000 clientes;
  - 20.000 regiões e 10.000 rotas aleatórias;
  - poder uniforme em [0, 10⁵).
- Parte medida: 50.000 comandos, sorteados com as proporções abaixo. A largura do BF é 1% do intervalo de poderes.

| Comando | C | D | F | Z | T | BP | BF | CC |
|---|---|---|---|---|---|---|---|---|
| Predomínio de atualizações | 30% | 25% | 20% | 5% | 5% | 5% | 5% | 5% |
| Predomínio de consultas | 5% | 4% | 3% | 1% | 2% | 30% | 25% | 30% |

- A carga respeita as regras da entrada: clientes de id par nunca saem (podem entrar na fila), e clientes de id ímpar podem sair (nunca entram na fila).
- Os mesmos comandos são executados no modo A e no modo H.

### Exp. 4 — Union-Find × BFS (`exp4_conectividade.csv`)
- V ∈ {10³, 10⁴, 10⁵} regiões; grau médio g ∈ {0,5; 1; 2; 4; 16}, ou seja, E = g·V/2 rotas aleatórias, sem rota de uma região para ela mesma.
- Construção, medida por elemento (região ou rota):
  - Mapa: listas de adjacência, que é o que a BFS precisa;
  - Union-Find: `criar` mais `unir`.
- Consultas: os mesmos pares aleatórios para os dois métodos.
  - Union-Find: 100.000 consultas.
  - BFS: 2·10⁷/(V+2E) consultas, limitadas a [20, 2000].
- Grau 1 é o limiar em que surge um "componente gigante" (Erdős–Rényi): abaixo dele, quase todo par é desconexo e a BFS termina cedo.

### Exp. 5 — Parâmetros (`exp5_parametros.csv`)
- **5a:** n = 50.000 clientes; poderes possíveis D ∈ {1, 10, …, 10⁶}. Quanto menor D, mais clientes por poder. Mede inserção, BP (1.000 consultas) e remoção.
- **5b:** n = 100.000; poder em [0, 10⁶); largura do BF ∈ {0,01%; 0,1%; 1%; 10%; 100%} do intervalo (200 consultas).

## Principais resultados (para o relatório)

Os números completos estão em `resultados/resumo.md`.

1. **Hash vence nas operações pontuais.**
   - Inserção, remoção e BP ficam de 2,5 a 15 vezes mais rápidas na hash.
   - Ela faz cerca de 1 comparação por operação, contra cerca de log₂ n na AVL (de 9 a 20).
   - A diferença no BP cresce com n, porque descer a AVL é uma sequência de acessos a nós espalhados pela memória.
2. **AVL vence com folga no BF.**
   - Para faixas de 1%, a AVL é de 30 a 80 vezes mais rápida.
   - Faixas estreitas (0,01%) ampliam a diferença para cerca de 4400 vezes.
   - O BF na hash custa O(M + n) **independente da largura**: precisa varrer a tabela inteira e depois ordenar.
3. **A remoção é onde as estruturas mais divergem, e a divergência depende da distribuição dos poderes (Exp. 5a).**
   - Com poderes quase todos distintos, a remoção na hash é cerca de 3 vezes mais rápida.
   - Com muitos clientes por poder, a lista de ids da entrada fica longa e remover exige deslocar elementos, O(k). Com D = 1, a remoção na hash custa **5,2 µs**, contra **0,25 µs** na AVL: 21 vezes mais lenta.
   - A AVL não sente essa variação, porque a chave composta (poder, id) torna todas as chaves distintas.
4. **Rotações.**
   - A AVL faz cerca de 0,70 rotação por inserção e 0,37 por remoção, praticamente constante em n.
   - A altura fica bem abaixo do limite teórico de 1,44·log₂n: 24 para n = 10⁶, contra o limite de 28,7.
5. **Rotatividade.** Com tamanho comparável, intercalar entradas e saídas não piora nenhuma das duas estruturas.
   - Na AVL, a altura se mantém (remoções também rebalanceiam).
   - Na hash, a tabela não encolhe: o fator de carga cai e as listas ficam ainda mais curtas.
6. **Perfis de carga.**
   - No modo H, o BF responde por mais de 99% do tempo em **ambos** os perfis. Mesmo com apenas 5% de BF, o modo A é cerca de 44 vezes mais rápido no total.
   - A hash só compensaria numa carga sem consultas por faixa.
7. **Conectividade.**
   - O Union-Find responde em cerca de 10 ns, independente do tamanho e da densidade, porque com compressão de caminho faz no máximo cerca de 2 passos por consulta.
   - A BFS custa O(V + E) por consulta assim que o grafo passa do grau 1: até 12 ms com 10⁵ regiões.
   - Na construção, o Union-Find também é mais barato (6–33 ns por elemento) que montar as listas de adjacência (31–220 ns).
   - A BFS só é competitiva em mapas muito esparsos (grau < 1), em que quase nada é alcançável.
8. **Migração.** Construir a AVL (H→A) custa cerca do dobro de construir a hash (A→H): O(n log n) contra O(n). Com 10⁶ clientes, a migração leva 0,85 s e 0,46 s, respectivamente.
