# Resumo dos resultados experimentais

Gerado automaticamente por `experimentos/graficos.py` a partir dos CSVs em `experimentos/resultados/`. Médias sobre as repetições.

### Exp. 1 — tempo médio por operação

| n | operação | AVL | Hash | razão (vence) |
|---|---|---|---|---|
| 1.000 | Inserção | 139 ns | 56 ns | 2.5× (Hash) |
| 1.000 | Remoção | 126 ns | 35 ns | 3.6× (Hash) |
| 1.000 | Busca exata | 120 ns | 16 ns | 7.5× (Hash) |
| 1.000 | Busca por faixa | 185 ns | 6.97 µs | 37.7× (AVL) |
| 10.000 | Inserção | 163 ns | 48 ns | 3.4× (Hash) |
| 10.000 | Remoção | 163 ns | 51 ns | 3.2× (Hash) |
| 10.000 | Busca exata | 160 ns | 16 ns | 10.1× (Hash) |
| 10.000 | Busca por faixa | 1.16 µs | 84.20 µs | 72.4× (AVL) |
| 100.000 | Inserção | 262 ns | 59 ns | 4.5× (Hash) |
| 100.000 | Remoção | 300 ns | 98 ns | 3.1× (Hash) |
| 100.000 | Busca exata | 262 ns | 30 ns | 8.7× (Hash) |
| 100.000 | Busca por faixa | 15.95 µs | 1.21 ms | 76.0× (AVL) |
| 1.000.000 | Inserção | 737 ns | 202 ns | 3.6× (Hash) |
| 1.000.000 | Remoção | 946 ns | 280 ns | 3.4× (Hash) |
| 1.000.000 | Busca exata | 879 ns | 60 ns | 14.7× (Hash) |
| 1.000.000 | Busca por faixa | 489.13 µs | 16.34 ms | 33.4× (AVL) |

### Exp. 1 — forma das estruturas após inserir n clientes

| n | altura AVL | limite 1,44·log₂n | rotações/inserção | rotações/remoção | colisões Hash | maior lista Hash | memória AVL | memória Hash |
|---|---|---|---|---|---|---|---|---|
| 1.000 | 12 | 14.4 | 0.713 | 0.366 | 158 | 2 | 31 KiB | 32 KiB |
| 10.000 | 16 | 19.1 | 0.702 | 0.369 | 1743 | 1 | 313 KiB | 363 KiB |
| 100.000 | 20 | 23.9 | 0.701 | 0.373 | 16414 | 2 | 3125 KiB | 3389 KiB |
| 1.000.000 | 24 | 28.7 | 0.705 | 0.373 | 165292 | 2 | 31250 KiB | 31866 KiB |

### Exp. 1 — custo da migração

| n | A→H por cliente | H→A por cliente | A→H total | H→A total |
|---|---|---|---|---|
| 1.000 | 80 ns | 172 ns | 80.06 µs | 171.98 µs |
| 10.000 | 112 ns | 248 ns | 1.12 ms | 2.48 ms |
| 100.000 | 181 ns | 387 ns | 18.07 ms | 38.67 ms |
| 1.000.000 | 452 ns | 843 ns | 451.91 ms | 842.52 ms |

### Exp. 2 — rotatividade (parte medida: n operações)

| n | estrutura | só entradas | entra e sai | inserção* | remoção* | comp./inserção | comp./remoção | rot./inserção | rot./remoção |
|---|---|---|---|---|---|---|---|---|---|
| 10.000 | AVL | 216 ns | 172 ns | 206 ns | 197 ns | 13.58 | 12.57 | 0.675 | 0.357 |
| 10.000 | Hash | 22 ns | 28 ns | 46 ns | 54 ns | 0.63 | 1.00 | 0.000 | 0.000 |
| 100.000 | AVL | 492 ns | 379 ns | 415 ns | 418 ns | 16.94 | 15.97 | 0.666 | 0.362 |
| 100.000 | Hash | 66 ns | 53 ns | 88 ns | 106 ns | 0.68 | 1.04 | 0.000 | 0.000 |
| 1.000.000 | AVL | 1.08 µs | 999 ns | 1.02 µs | 1.04 µs | 20.30 | 19.32 | 0.666 | 0.361 |
| 1.000.000 | Hash | 264 ns | 173 ns | 190 ns | 231 ns | 0.73 | 1.08 | 0.000 | 0.000 |

\* tempo medido operação a operação (inclui o custo do relógio, igual para as duas).

### Exp. 3 — Predomínio de atualizações

| comando | quantidade | modo A | modo H |
|---|---|---|---|
| C | 15059 | 271 ns | 74 ns |
| D | 12574 | 284 ns | 145 ns |
| F | 9920 | 75 ns | 79 ns |
| Z | 2553 | 293 ns | 312 ns |
| T | 2513 | 211 ns | 333 ns |
| BP | 2398 | 235 ns | 61 ns |
| BF | 2545 | 2.62 µs | 272.84 µs |
| CC | 2439 | 92 ns | 154 ns |
| TOTAL | 50000 | 328 ns | 14.18 µs |

### Exp. 3 — Predomínio de consultas

| comando | quantidade | modo A | modo H |
|---|---|---|---|
| C | 2499 | 284 ns | 116 ns |
| D | 2032 | 301 ns | 199 ns |
| F | 1524 | 68 ns | 103 ns |
| Z | 482 | 217 ns | 320 ns |
| T | 907 | 212 ns | 522 ns |
| BP | 15104 | 219 ns | 61 ns |
| BF | 12477 | 2.35 µs | 266.44 µs |
| CC | 14974 | 60 ns | 165 ns |
| TOTAL | 50000 | 690 ns | 66.36 µs |

### Exp. 4 — conectividade

| regiões | grau | constr. UF (por elem.) | constr. Mapa (por elem.) | consulta UF | consulta BFS | passos UF/consulta | regiões visitadas/BFS | % SIM | memória UF | memória Mapa |
|---|---|---|---|---|---|---|---|---|---|---|
| 1.000 | 0.5 | 9 ns | 59 ns | 11 ns | 25 ns | 0.50 | 2 | 0% | 8 KiB | 70 KiB |
| 1.000 | 1 | 8 ns | 46 ns | 14 ns | 204 ns | 1.00 | 17 | 2% | 8 KiB | 78 KiB |
| 1.000 | 2 | 12 ns | 35 ns | 10 ns | 8.15 µs | 1.67 | 384 | 65% | 8 KiB | 94 KiB |
| 1.000 | 4 | 14 ns | 31 ns | 7 ns | 14.21 µs | 1.96 | 493 | 96% | 8 KiB | 125 KiB |
| 1.000 | 16 | 9 ns | 39 ns | 7 ns | 26.66 µs | 2.00 | 498 | 100% | 8 KiB | 313 KiB |
| 10.000 | 0.5 | 6 ns | 58 ns | 11 ns | 51 ns | 0.50 | 2 | 0% | 78 KiB | 703 KiB |
| 10.000 | 1 | 9 ns | 49 ns | 15 ns | 370 ns | 1.01 | 17 | 0% | 78 KiB | 781 KiB |
| 10.000 | 2 | 15 ns | 45 ns | 10 ns | 110.09 µs | 1.70 | 3786 | 63% | 78 KiB | 938 KiB |
| 10.000 | 4 | 15 ns | 40 ns | 7 ns | 202.87 µs | 1.97 | 4918 | 96% | 78 KiB | 1250 KiB |
| 10.000 | 16 | 11 ns | 54 ns | 7 ns | 528.75 µs | 2.00 | 5133 | 100% | 78 KiB | 3125 KiB |
| 100.000 | 0.5 | 7 ns | 88 ns | 13 ns | 161 ns | 0.52 | 2 | 0% | 781 KiB | 7031 KiB |
| 100.000 | 1 | 10 ns | 86 ns | 20 ns | 4.48 µs | 1.16 | 84 | 0% | 781 KiB | 7813 KiB |
| 100.000 | 2 | 17 ns | 107 ns | 15 ns | 3.09 ms | 1.91 | 41603 | 63% | 781 KiB | 9375 KiB |
| 100.000 | 4 | 33 ns | 130 ns | 10 ns | 6.71 ms | 2.00 | 53298 | 96% | 781 KiB | 12500 KiB |
| 100.000 | 16 | 13 ns | 220 ns | 8 ns | 13.82 ms | 2.00 | 48897 | 100% | 781 KiB | 31250 KiB |

### Exp. 5a — poderes distintos (n = 50000)

| poderes possíveis | clientes por poder (≈) | inserção AVL | inserção Hash | remoção AVL | remoção Hash | BP AVL | BP Hash |
|---|---|---|---|---|---|---|---|
| 1 | 50.000 | 120 ns | 7 ns | 231 ns | 5.33 µs | 555.27 µs | 83.40 µs |
| 10 | 5.000 | 140 ns | 7 ns | 239 ns | 598 ns | 65.63 µs | 8.12 µs |
| 100 | 500 | 166 ns | 8 ns | 229 ns | 118 ns | 6.81 µs | 848 ns |
| 1.000 | 50 | 216 ns | 14 ns | 228 ns | 61 ns | 927 ns | 86 ns |
| 10.000 | 5 | 227 ns | 26 ns | 225 ns | 43 ns | 296 ns | 29 ns |
| 100.000 | 1 | 249 ns | 61 ns | 269 ns | 66 ns | 225 ns | 21 ns |
| 1.000.000 | 1 | 228 ns | 71 ns | 243 ns | 77 ns | 302 ns | 23 ns |

### Exp. 5b — largura da faixa no BF (n = 100000)

| largura | clientes devolvidos | AVL | Hash | Hash / AVL |
|---|---|---|---|---|
| 0.01% | 10 | 473 ns | 2.09 ms | 4422.3× |
| 0.1% | 100 | 2.28 µs | 2.08 ms | 911.5× |
| 1% | 1002 | 15.66 µs | 2.19 ms | 139.9× |
| 10% | 10005 | 155.41 µs | 3.70 ms | 23.8× |
| 100% | 100000 | 2.22 ms | 22.44 ms | 10.1× |
