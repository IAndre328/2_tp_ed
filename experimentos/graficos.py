"""
Gera os graficos (PDF + PNG) e um resumo em tabelas (Markdown) a partir dos
CSVs produzidos por bin/bench.out.

Uso (a partir da raiz do projeto):
    PYTHONPATH=experimentos/.pylibs python3 experimentos/graficos.py

Todas as medias sao sobre as repeticoes (coluna rep); as barras de erro,
quando presentes, sao o desvio padrao entre repeticoes.
"""
import csv
import math
import os
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import NullFormatter, FuncFormatter, LogLocator

AQUI = os.path.dirname(os.path.abspath(__file__))
RES = os.path.join(AQUI, "resultados")
GRAF = os.path.join(AQUI, "graficos")
os.makedirs(GRAF, exist_ok=True)

# --- estilo -----------------------------------------------------------------
# Paleta categorica (slots 1 e 2 da paleta padrao validada) + codificacao
# secundaria por marcador e tracejado, para leitura em preto e branco.
COR = {"AVL": "#2a78d6", "Hash": "#eb6834", "UnionFind": "#2a78d6", "BFS": "#eb6834"}
MARCA = {"AVL": "o", "Hash": "s", "UnionFind": "o", "BFS": "s"}
TRACO = {"AVL": "-", "Hash": "--", "UnionFind": "-", "BFS": "--"}
TINTA = "#2b2b2b"
TINTA2 = "#6b6b6b"
GRADE = "#e4e4e4"

plt.rcParams.update({
    "font.family": "serif",          # combina com o template AAAI (Times)
    "font.size": 8,
    "axes.titlesize": 8,
    "axes.labelsize": 8,
    "legend.fontsize": 7,
    "xtick.labelsize": 7,
    "ytick.labelsize": 7,
    "axes.edgecolor": TINTA2,
    "axes.labelcolor": TINTA,
    "xtick.color": TINTA2,
    "ytick.color": TINTA2,
    "text.color": TINTA,
    "axes.spines.top": False,
    "axes.spines.right": False,
    "axes.grid": True,
    "axes.axisbelow": True,
    "grid.color": GRADE,
    "grid.linewidth": 0.6,
    "lines.linewidth": 1.5,
    "lines.markersize": 4.5,
    "legend.frameon": False,
    "savefig.dpi": 200,
    "savefig.bbox": "tight",
})

UMA_COLUNA = 3.3    # polegadas (largura de coluna AAAI)
DUAS_COLUNAS = 7.0


def salvar(fig, nome):
    for ax in fig.axes:   # no eixo log, so as potencias de 10 levam rotulo
        if ax.get_yscale() == "log":
            ax.yaxis.set_minor_formatter(NullFormatter())
            lo, hi = ax.get_ylim()
            if math.log10(hi / lo) < 1.6:   # menos de ~2 decadas: rotula 1, 2 e 5
                ax.yaxis.set_major_locator(LogLocator(base=10, subs=(1.0, 2.0, 5.0)))
                ax.yaxis.set_major_formatter(FuncFormatter(lambda v, _: f"{v:g}"))
        if ax.get_xscale() == "log":
            ax.xaxis.set_minor_formatter(NullFormatter())
    for ext in ("pdf", "png"):
        fig.savefig(os.path.join(GRAF, f"{nome}.{ext}"))
    plt.close(fig)
    print(f"  grafico: graficos/{nome}.pdf")


def ler(nome):
    with open(os.path.join(RES, nome), newline="") as f:
        return list(csv.DictReader(f))


def num(x):
    try:
        return float(x)
    except (TypeError, ValueError):
        return math.nan


def media_dp(valores):
    v = [x for x in valores if not math.isnan(x)]
    if not v:
        return math.nan, math.nan
    m = sum(v) / len(v)
    dp = math.sqrt(sum((x - m) ** 2 for x in v) / (len(v) - 1)) if len(v) > 1 else 0.0
    return m, dp


def agrupar(linhas, chaves, campo):
    """{tupla(chaves): (media, desvio)} do campo, sobre as repeticoes."""
    g = defaultdict(list)
    for l in linhas:
        g[tuple(l[c] for c in chaves)].append(num(l[campo]))
    return {k: media_dp(v) for k, v in g.items()}


def fmt_ns(ns):
    if math.isnan(ns):
        return "-"
    if ns >= 1e6:
        return f"{ns / 1e6:.2f} ms"
    if ns >= 1e3:
        return f"{ns / 1e3:.2f} µs"
    return f"{ns:.0f} ns"


def linha_serie(ax, xs, medias, dps, serie, rotulo=None):
    ax.errorbar(xs, medias, yerr=dps, color=COR[serie], marker=MARCA[serie],
                linestyle=TRACO[serie], label=rotulo or serie, capsize=2,
                elinewidth=0.8, markeredgecolor="white", markeredgewidth=0.6)


resumo = []   # linhas do resumo em Markdown


def tabela_md(titulo, cabecalho, linhas):
    resumo.append(f"\n### {titulo}\n")
    resumo.append("| " + " | ".join(cabecalho) + " |")
    resumo.append("|" + "---|" * len(cabecalho))
    for l in linhas:
        resumo.append("| " + " | ".join(str(c) for c in l) + " |")


# ============================================================================
# Experimento 1 -- AVL x Hash por operacao
# ============================================================================
def experimento1():
    dados = ler("exp1_operacoes.csv")
    ops = [("insercao", "Inserção (C)"), ("remocao", "Remoção (D)"),
           ("busca_exata", "Busca exata (BP)"), ("busca_faixa", "Busca por faixa (BF, 1% das chaves)")]
    tempo = agrupar(dados, ["estrutura", "operacao", "n"], "tempo_ns_op")
    comps = agrupar(dados, ["estrutura", "operacao", "n"], "comparacoes_op")
    ns = sorted({int(l["n"]) for l in dados})

    fig, eixos = plt.subplots(2, 2, figsize=(DUAS_COLUNAS, 4.4))
    for ax, (op, titulo) in zip(eixos.flat, ops):
        for est in ("AVL", "Hash"):
            m = [tempo[(est, op, str(n))][0] for n in ns]
            d = [tempo[(est, op, str(n))][1] for n in ns]
            linha_serie(ax, ns, m, d, est)
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_title(titulo)
        ax.set_xlabel("número de clientes (n)")
        ax.set_ylabel("tempo por operação (ns)")
    eixos[0, 0].legend(loc="upper left")
    fig.tight_layout()
    salvar(fig, "exp1_tempo_por_operacao")

    # comparacoes por operacao (custo independente da maquina)
    fig, eixos = plt.subplots(1, 4, figsize=(DUAS_COLUNAS, 1.9))
    for ax, (op, titulo) in zip(eixos.flat, ops):
        for est in ("AVL", "Hash"):
            m = [comps[(est, op, str(n))][0] for n in ns]
            linha_serie(ax, ns, m, [0] * len(ns), est)
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_title(titulo.split(" (")[0])
        ax.set_xlabel("n")
    eixos[0].set_ylabel("comparações por operação")
    eixos[0].legend(loc="center left", bbox_to_anchor=(0.0, 0.45))
    fig.tight_layout()
    salvar(fig, "exp1_comparacoes_por_operacao")

    # migracao
    fig, ax = plt.subplots(figsize=(UMA_COLUNA, 2.1))
    mig = [("migracao_A_para_H", "A → H (constrói a Hash)", "Hash"),
           ("migracao_H_para_A", "H → A (constrói a AVL)", "AVL")]
    for op, rot, est in mig:
        m = [tempo[("Indice", op, str(n))][0] for n in ns]
        d = [tempo[("Indice", op, str(n))][1] for n in ns]
        linha_serie(ax, ns, m, d, est, rot)
    ax.set_xscale("log")
    ax.set_xlabel("número de clientes (n)")
    ax.set_ylabel("tempo por cliente migrado (ns)")
    ax.set_ylim(bottom=0)
    ax.legend(loc="upper left")
    salvar(fig, "exp1_migracao")

    # tabelas
    linhas = []
    for n in ns:
        for op, titulo in ops:
            a = tempo[("AVL", op, str(n))][0]
            h = tempo[("Hash", op, str(n))][0]
            vence = "AVL" if a < h else "Hash"
            linhas.append([f"{n:,}".replace(",", "."), titulo.split(" (")[0], fmt_ns(a), fmt_ns(h),
                           f"{max(a, h) / min(a, h):.1f}× ({vence})"])
    tabela_md("Exp. 1 — tempo médio por operação", ["n", "operação", "AVL", "Hash", "razão (vence)"], linhas)

    forma = agrupar(dados, ["estrutura", "operacao", "n"], "forma")
    rot = agrupar(dados, ["estrutura", "operacao", "n"], "rotacoes_op")
    col = agrupar(dados, ["estrutura", "operacao", "n"], "colisoes")
    mem = agrupar(dados, ["estrutura", "operacao", "n"], "memoria_bytes")
    linhas = []
    for n in ns:
        linhas.append([f"{n:,}".replace(",", "."),
                       f"{forma[('AVL', 'insercao', str(n))][0]:.0f}",
                       f"{1.44 * math.log2(n + 2):.1f}",
                       f"{rot[('AVL', 'insercao', str(n))][0]:.3f}",
                       f"{rot[('AVL', 'remocao', str(n))][0]:.3f}",
                       f"{col[('Hash', 'insercao', str(n))][0]:.0f}",
                       f"{forma[('Hash', 'insercao', str(n))][0]:.0f}",
                       f"{mem[('AVL', 'insercao', str(n))][0] / 1024:.0f} KiB",
                       f"{mem[('Hash', 'insercao', str(n))][0] / 1024:.0f} KiB"])
    tabela_md("Exp. 1 — forma das estruturas após inserir n clientes",
              ["n", "altura AVL", "limite 1,44·log₂n", "rotações/inserção", "rotações/remoção",
               "colisões Hash", "maior lista Hash", "memória AVL", "memória Hash"], linhas)

    linhas = []
    for n in ns:
        ah = tempo[("Indice", "migracao_A_para_H", str(n))][0]
        ha = tempo[("Indice", "migracao_H_para_A", str(n))][0]
        linhas.append([f"{n:,}".replace(",", "."), fmt_ns(ah), fmt_ns(ha),
                       fmt_ns(ah * n), fmt_ns(ha * n)])
    tabela_md("Exp. 1 — custo da migração", ["n", "A→H por cliente", "H→A por cliente",
                                             "A→H total", "H→A total"], linhas)


# ============================================================================
# Experimento 2 -- rotatividade
# ============================================================================
def experimento2():
    dados = ler("exp2_rotatividade.csv")
    tempo = agrupar(dados, ["estrutura", "carga", "n", "operacao"], "tempo_ns_op")
    rots = agrupar(dados, ["estrutura", "carga", "n", "operacao"], "rotacoes_op")
    comps = agrupar(dados, ["estrutura", "carga", "n", "operacao"], "comparacoes_op")
    ns = sorted({int(l["n"]) for l in dados})
    nmax = str(ns[-1])

    # barras: tempo por operacao no maior n
    grupos = [("so_entradas", "total", "Só entradas"),
              ("entra_e_sai", "total", "Entra e sai\n(média)"),
              ("entra_e_sai", "insercao", "Entra e sai:\ninserções"),
              ("entra_e_sai", "remocao", "Entra e sai:\nremoções")]
    fig, ax = plt.subplots(figsize=(UMA_COLUNA, 2.3))
    largura = 0.38
    for i, est in enumerate(("AVL", "Hash")):
        xs = [g + (i - 0.5) * largura for g in range(len(grupos))]
        m = [tempo[(est, c, nmax, op)][0] for c, op, _ in grupos]
        d = [tempo[(est, c, nmax, op)][1] for c, op, _ in grupos]
        ax.bar(xs, m, largura * 0.92, yerr=d, color=COR[est], label=est,
               error_kw={"elinewidth": 0.8, "capsize": 2, "ecolor": TINTA2})
    ax.set_xticks(range(len(grupos)))
    ax.set_xticklabels([g[2] for g in grupos])
    ax.set_ylabel("tempo por operação (ns)")
    ax.set_title(f"n = {int(nmax):,} clientes antes da parte medida".replace(",", "."), pad=16)
    ax.grid(axis="x", visible=False)
    ax.legend(loc="lower center", bbox_to_anchor=(0.5, 0.99), ncol=2)
    salvar(fig, "exp2_rotatividade_tempo")

    # linha do tempo: altura da AVL e maior lista da Hash ao longo da execucao
    lt = ler("exp2_linha_tempo.csv")
    fig, eixos = plt.subplots(1, 2, figsize=(DUAS_COLUNAS, 1.9))
    for ax, est, rot in ((eixos[0], "AVL", "altura da árvore"),
                         (eixos[1], "Hash", "comprimento da maior lista")):
        for carga, estilo, nome in (("so_entradas", "-", "só entradas"),
                                    ("entra_e_sai", ":", "entra e sai")):
            pts = [l for l in lt if l["estrutura"] == est and l["carga"] == carga and l["n"] == nmax]
            xs = [int(l["passo"]) / 1e6 for l in pts]
            ys = [int(l["forma"]) for l in pts]
            ax.plot(xs, ys, color=COR[est], linestyle=estilo, label=nome)
        ax.set_ylim(bottom=0, top=ax.get_ylim()[1] * 1.15)
        ax.axvline(int(nmax) / 1e6, color=TINTA2, linewidth=0.8, linestyle="-")
        ax.text(int(nmax) / 1e6, ax.get_ylim()[1] * 0.45, " início da\n parte medida",
                color=TINTA2, fontsize=6, va="center")
        ax.set_xlabel("operações executadas (milhões)")
        ax.set_ylabel(rot)
        ax.set_title(est)
        ax.legend(loc="upper left", ncol=2)
    fig.tight_layout()
    salvar(fig, "exp2_forma_ao_longo_do_tempo")

    linhas = []
    for n in ns:
        for est in ("AVL", "Hash"):
            linhas.append([f"{n:,}".replace(",", "."), est,
                           fmt_ns(tempo[(est, "so_entradas", str(n), "total")][0]),
                           fmt_ns(tempo[(est, "entra_e_sai", str(n), "total")][0]),
                           fmt_ns(tempo[(est, "entra_e_sai", str(n), "insercao")][0]),
                           fmt_ns(tempo[(est, "entra_e_sai", str(n), "remocao")][0]),
                           f"{comps[(est, 'entra_e_sai', str(n), 'insercao')][0]:.2f}",
                           f"{comps[(est, 'entra_e_sai', str(n), 'remocao')][0]:.2f}",
                           f"{rots[(est, 'entra_e_sai', str(n), 'insercao')][0]:.3f}",
                           f"{rots[(est, 'entra_e_sai', str(n), 'remocao')][0]:.3f}"])
    tabela_md("Exp. 2 — rotatividade (parte medida: n operações)",
              ["n", "estrutura", "só entradas", "entra e sai", "inserção*", "remoção*",
               "comp./inserção", "comp./remoção", "rot./inserção", "rot./remoção"], linhas)
    resumo.append("\n\\* tempo medido operação a operação (inclui o custo do relógio, igual para as duas).")


# ============================================================================
# Experimento 3 -- perfis de carga
# ============================================================================
def experimento3():
    dados = ler("exp3_perfis.csv")
    tempo = agrupar(dados, ["perfil", "modo", "comando"], "tempo_ns_op")
    total = agrupar(dados, ["perfil", "modo", "comando"], "tempo_total_s")
    qtd = agrupar(dados, ["perfil", "modo", "comando"], "ops")
    perfis = [("atualizacoes", "Predomínio de atualizações"), ("consultas", "Predomínio de consultas")]
    cmds = ["C", "D", "F", "Z", "T", "BP", "BF", "CC"]

    fig, ax = plt.subplots(figsize=(UMA_COLUNA, 2.2))
    largura = 0.38
    for i, (modo, est) in enumerate((("A", "AVL"), ("H", "Hash"))):
        xs = [p + (i - 0.5) * largura for p in range(len(perfis))]
        m = [tempo[(p, modo, "TOTAL")][0] for p, _ in perfis]
        d = [tempo[(p, modo, "TOTAL")][1] for p, _ in perfis]
        barras = ax.bar(xs, m, largura * 0.92, yerr=d, color=COR[est], label=f"modo {modo} ({est})",
                        error_kw={"elinewidth": 0.8, "capsize": 2, "ecolor": TINTA2})
        for b, v in zip(barras, m):
            ax.text(b.get_x() + b.get_width() / 2, v * 1.15, fmt_ns(v), ha="center",
                    va="bottom", fontsize=6, color=TINTA)
    ax.set_yscale("log")
    ax.set_xticks(range(len(perfis)))
    ax.set_xticklabels([p[1] for p in perfis])
    ax.set_ylabel("tempo médio por comando (ns, log)")
    ax.set_ylim(top=ax.get_ylim()[1] * 4)
    ax.grid(axis="x", visible=False)
    ax.legend(loc="upper left")
    salvar(fig, "exp3_perfis_total")

    # parcela do tempo total por comando (onde o tempo vai)
    fig, eixos = plt.subplots(1, 2, figsize=(DUAS_COLUNAS, 2.0), sharey=True)
    for ax, (p, titulo) in zip(eixos, perfis):
        largura = 0.38
        for i, (modo, est) in enumerate((("A", "AVL"), ("H", "Hash"))):
            soma = sum(total[(p, modo, c)][0] for c in cmds)
            frac = [100 * total[(p, modo, c)][0] / soma for c in cmds]
            xs = [k + (i - 0.5) * largura for k in range(len(cmds))]
            ax.bar(xs, frac, largura * 0.92, color=COR[est], label=f"modo {modo} ({est})")
        ax.set_xticks(range(len(cmds)))
        ax.set_xticklabels(cmds)
        ax.set_title(titulo)
        ax.set_xlabel("comando")
        ax.grid(axis="x", visible=False)
    eixos[0].set_ylabel("% do tempo total")
    eixos[0].legend(loc="upper left")
    fig.tight_layout()
    salvar(fig, "exp3_parcela_por_comando")

    for p, titulo in perfis:
        linhas = []
        for c in cmds + ["TOTAL"]:
            a = tempo[(p, "A", c)][0]
            h = tempo[(p, "H", c)][0]
            linhas.append([c, f"{qtd[(p, 'A', c)][0]:.0f}", fmt_ns(a), fmt_ns(h)])
        tabela_md(f"Exp. 3 — {titulo}", ["comando", "quantidade", "modo A", "modo H"], linhas)


# ============================================================================
# Experimento 4 -- Union-Find x BFS
# ============================================================================
def experimento4():
    dados = ler("exp4_conectividade.csv")
    tempo = agrupar(dados, ["regioes", "grau_medio", "metodo", "fase"], "tempo_ns_op")
    visit = agrupar(dados, ["regioes", "grau_medio", "metodo", "fase"], "visitadas_op")
    passos = agrupar(dados, ["regioes", "grau_medio", "metodo", "fase"], "passos_op")
    fsim = agrupar(dados, ["regioes", "grau_medio", "metodo", "fase"], "frac_sim")
    mem = agrupar(dados, ["regioes", "grau_medio", "metodo", "fase"], "memoria_bytes")
    vs = sorted({l["regioes"] for l in dados}, key=int)
    gs = sorted({l["grau_medio"] for l in dados}, key=float)
    gx = [float(g) for g in gs]

    fig, eixos = plt.subplots(1, len(vs), figsize=(DUAS_COLUNAS, 2.0), sharey=True)
    for ax, v in zip(eixos, vs):
        for met, rot in (("UnionFind", "Union-Find"), ("BFS", "BFS")):
            m = [tempo[(v, g, met, "consulta")][0] for g in gs]
            d = [tempo[(v, g, met, "consulta")][1] for g in gs]
            linha_serie(ax, gx, m, d, met, rot)
        ax.axvline(1.0, color=TINTA2, linewidth=0.8, linestyle=":")
        if v == vs[0]:
            ax.text(1.05, 3e6, "grau 1: surge um\ngrande componente", color=TINTA2, fontsize=6, va="center")
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_xticks(gx)
        ax.set_xticklabels(gs)
        ax.set_title(f"{int(v):,} regiões".replace(",", "."))
        ax.set_xlabel("grau médio (2·rotas / regiões)")
    eixos[0].set_ylabel("tempo por consulta CC (ns)")
    eixos[0].legend(loc="center left", bbox_to_anchor=(0.0, 0.62))
    fig.tight_layout()
    salvar(fig, "exp4_consulta")

    fig, ax = plt.subplots(figsize=(UMA_COLUNA, 2.0))
    v = vs[-1]
    for met, rot in (("UnionFind", "Union-Find (criar + unir)"), ("BFS", "Mapa (listas de adjacência)")):
        m = [tempo[(v, g, met, "construcao")][0] for g in gs]
        d = [tempo[(v, g, met, "construcao")][1] for g in gs]
        linha_serie(ax, gx, m, d, met, rot)
    ax.set_xscale("log")
    ax.set_xticks(gx)
    ax.set_xticklabels(gs)
    ax.set_ylim(bottom=0)
    ax.set_xlabel("grau médio")
    ax.set_ylabel("tempo por região/rota (ns)")
    ax.set_title("Construção, " + f"{int(v):,} regiões".replace(",", "."))
    ax.legend(loc="upper left")
    salvar(fig, "exp4_construcao")

    linhas = []
    for v in vs:
        for g in gs:
            linhas.append([f"{int(v):,}".replace(",", "."), g,
                           fmt_ns(tempo[(v, g, "UnionFind", "construcao")][0]),
                           fmt_ns(tempo[(v, g, "BFS", "construcao")][0]),
                           fmt_ns(tempo[(v, g, "UnionFind", "consulta")][0]),
                           fmt_ns(tempo[(v, g, "BFS", "consulta")][0]),
                           f"{passos[(v, g, 'UnionFind', 'consulta')][0]:.2f}",
                           f"{visit[(v, g, 'BFS', 'consulta')][0]:.0f}",
                           f"{100 * fsim[(v, g, 'UnionFind', 'consulta')][0]:.0f}%",
                           f"{mem[(v, g, 'UnionFind', 'construcao')][0] / 1024:.0f} KiB",
                           f"{mem[(v, g, 'BFS', 'construcao')][0] / 1024:.0f} KiB"])
    tabela_md("Exp. 4 — conectividade",
              ["regiões", "grau", "constr. UF (por elem.)", "constr. Mapa (por elem.)",
               "consulta UF", "consulta BFS", "passos UF/consulta", "regiões visitadas/BFS",
               "% SIM", "memória UF", "memória Mapa"], linhas)


# ============================================================================
# Experimento 5 -- parametros
# ============================================================================
def experimento5():
    dados = ler("exp5_parametros.csv")
    a = [l for l in dados if l["exp"] == "5a"]
    b = [l for l in dados if l["exp"] == "5b"]

    tempo = agrupar(a, ["estrutura", "operacao", "chaves_distintas"], "tempo_ns_op")
    ds = sorted({int(l["chaves_distintas"]) for l in a})
    n = int(a[0]["n"])
    ops = [("insercao", "Inserção"), ("remocao", "Remoção"), ("busca_exata", "Busca exata (BP)")]
    fig, eixos = plt.subplots(1, 3, figsize=(DUAS_COLUNAS, 2.0))
    for ax, (op, titulo) in zip(eixos, ops):
        for est in ("AVL", "Hash"):
            m = [tempo[(est, op, str(d))][0] for d in ds]
            e = [tempo[(est, op, str(d))][1] for d in ds]
            linha_serie(ax, ds, m, e, est)
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_title(titulo)
        ax.set_xlabel("poderes distintos possíveis")
    eixos[0].set_ylabel("tempo por operação (ns)")
    eixos[1].legend(loc="upper right")
    fig.suptitle(f"n = {n:,} clientes".replace(",", "."), fontsize=8, y=1.02)
    fig.tight_layout()
    salvar(fig, "exp5a_poderes_distintos")

    linhas = []
    for d in ds:
        linhas.append([f"{d:,}".replace(",", "."), f"{n / min(d, n):,.0f}".replace(",", "."),
                       fmt_ns(tempo[("AVL", "insercao", str(d))][0]),
                       fmt_ns(tempo[("Hash", "insercao", str(d))][0]),
                       fmt_ns(tempo[("AVL", "remocao", str(d))][0]),
                       fmt_ns(tempo[("Hash", "remocao", str(d))][0]),
                       fmt_ns(tempo[("AVL", "busca_exata", str(d))][0]),
                       fmt_ns(tempo[("Hash", "busca_exata", str(d))][0])])
    tabela_md(f"Exp. 5a — poderes distintos (n = {n})",
              ["poderes possíveis", "clientes por poder (≈)", "inserção AVL", "inserção Hash",
               "remoção AVL", "remoção Hash", "BP AVL", "BP Hash"], linhas)

    tempo = agrupar(b, ["estrutura", "frac_faixa"], "tempo_ns_op")
    itens = agrupar(b, ["estrutura", "frac_faixa"], "itens_op")
    b_bf = [l for l in b if l["operacao"] == "busca_faixa"]
    tempo = agrupar(b_bf, ["estrutura", "frac_faixa"], "tempo_ns_op")
    itens = agrupar(b_bf, ["estrutura", "frac_faixa"], "itens_op")
    fs = sorted({l["frac_faixa"] for l in b_bf}, key=float)
    fx = [100 * float(f) for f in fs]
    fig, ax = plt.subplots(figsize=(UMA_COLUNA, 2.1))
    for est in ("AVL", "Hash"):
        m = [tempo[(est, f)][0] for f in fs]
        e = [tempo[(est, f)][1] for f in fs]
        linha_serie(ax, fx, m, e, est)
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.xaxis.set_major_formatter(FuncFormatter(lambda v, _: f"{v:g}%".replace(".", ",")))
    ax.set_xlabel("largura da faixa (% do intervalo de poderes)")
    ax.set_ylabel("tempo por BF (ns)")
    nb = int(b_bf[0]["n"])
    ax.set_title(f"BF com n = {nb:,} clientes".replace(",", "."))
    ax.legend(loc="lower right")
    salvar(fig, "exp5b_largura_faixa")

    linhas = []
    for f in fs:
        linhas.append([f"{100 * float(f):g}%", f"{itens[('AVL', f)][0]:.0f}",
                       fmt_ns(tempo[("AVL", f)][0]), fmt_ns(tempo[("Hash", f)][0]),
                       f"{tempo[('Hash', f)][0] / tempo[('AVL', f)][0]:.1f}×"])
    tabela_md(f"Exp. 5b — largura da faixa no BF (n = {nb})",
              ["largura", "clientes devolvidos", "AVL", "Hash", "Hash / AVL"], linhas)


if __name__ == "__main__":
    resumo.append("# Resumo dos resultados experimentais\n")
    resumo.append("Gerado automaticamente por `experimentos/graficos.py` a partir dos CSVs em "
                  "`experimentos/resultados/`. Médias sobre as repetições.")
    for nome, funcao in (("exp1_operacoes.csv", experimento1), ("exp2_rotatividade.csv", experimento2),
                         ("exp3_perfis.csv", experimento3), ("exp4_conectividade.csv", experimento4),
                         ("exp5_parametros.csv", experimento5)):
        if os.path.exists(os.path.join(RES, nome)):
            print(f"[{nome}]")
            funcao()
    with open(os.path.join(RES, "resumo.md"), "w") as f:
        f.write("\n".join(resumo) + "\n")
    print("  resumo: resultados/resumo.md")
