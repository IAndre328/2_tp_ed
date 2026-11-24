// =====================================================================
// Programa de experimentos do TP2 (nao faz parte da entrega do VPL).
//
// Mede diretamente os TADs (sem E/S de texto), com todas as entradas
// geradas ANTES dos lacos medidos e com vetores de saida reaproveitados,
// para que o tempo reflita o custo dos algoritmos e nao de alocacoes.
//
// Uso:  ./bin/bench.out <experimento> <pasta_saida> [repeticoes]
//       experimento: 1 | 2 | 3 | 4 | 5 | todos
// Cada experimento grava um CSV em <pasta_saida>.
// =====================================================================

#include "lib.h"
#include "ArvoreAVL.h"
#include "TabelaHash.h"
#include "Indice.h"
#include "FilaPrioridade.h"
#include "ConjuntosDisjuntos.h"
#include "Mapa.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// ---------------------------------------------------------------------
// Utilidades
// ---------------------------------------------------------------------

// Gerador pseudoaleatorio deterministico (splitmix64).
struct Rng {
    unsigned long long s;
    explicit Rng(unsigned long long semente) : s(semente * 0x9E3779B97F4A7C15ull + 12345) {}
    unsigned long long prox() {
        unsigned long long z = (s += 0x9E3779B97F4A7C15ull);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
        return z ^ (z >> 31);
    }
    long long ate(long long n) { return (long long)(prox() % (unsigned long long)n); }  // [0, n)
    double real() { return (double)(prox() >> 11) * (1.0 / 9007199254740992.0); }      // [0, 1)
};

typedef std::chrono::steady_clock Relogio;

static double segundos(Relogio::time_point inicio) {
    return std::chrono::duration<double>(Relogio::now() - inicio).count();
}

// impede o compilador de descartar resultados nao usados
static volatile long long sumidouro = 0;

static long long limitar(long long x, long long lo, long long hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

static void embaralhar(int* v, long long n, Rng& r) {
    for (long long i = n - 1; i > 0; i--) {
        long long j = r.ate(i + 1);
        int t = v[i]; v[i] = v[j]; v[j] = t;
    }
}

static FILE* abrirCsv(const std::string& pasta, const char* nome, const char* cabecalho) {
    std::string caminho = pasta + "/" + nome;
    FILE* f = std::fopen(caminho.c_str(), "w");
    if (f == nullptr) {
        std::fprintf(stderr, "Nao consegui criar %s\n", caminho.c_str());
        std::exit(1);
    }
    std::fprintf(f, "%s\n", cabecalho);
    std::fprintf(stderr, "  -> %s\n", caminho.c_str());
    return f;
}

// Metricas especificas de cada estrutura (sobrecarga por tipo).
static const char* nomeDe(const ArvoreAVL&) { return "AVL"; }
static const char* nomeDe(const TabelaHash&) { return "Hash"; }
static long long rotacoesDe(const ArvoreAVL& a) { return a.getNumRotacoes(); }
static long long rotacoesDe(const TabelaHash&) { return 0; }
static long long colisoesDe(const ArvoreAVL&) { return 0; }
static long long colisoesDe(const TabelaHash& h) { return h.getNumColisoes(); }
// "forma": altura na AVL; comprimento da maior lista de balde na Hash
static long long formaDe(const ArvoreAVL& a) { return a.getAltura(); }
static long long formaDe(const TabelaHash& h) { return h.getMaiorBalde(); }

// ---------------------------------------------------------------------
// Medicao das operacoes do indice (usada pelos experimentos 1 e 5)
// ---------------------------------------------------------------------

static const char* CAB_OPERACOES =
    "exp,estrutura,n,chaves_distintas,frac_faixa,rep,operacao,ops,"
    "tempo_ns_op,comparacoes_op,rotacoes_op,colisoes,forma,memoria_bytes,itens_op";

static void linhaOperacao(FILE* f, const char* exp, const char* estrutura, long long n,
                          long long D, double frac, int rep, const char* op, long long ops,
                          double seg, long long comps, long long rots, long long colisoes,
                          long long forma, long long memoria, long long itens) {
    if (ops <= 0) ops = 1;
    std::fprintf(f, "%s,%s,%lld,%lld,%g,%d,%s,%lld,%.2f,%.3f,%.4f,%lld,%lld,%lld,%.2f\n",
                 exp, estrutura, n, D, frac, rep, op, ops, seg * 1e9 / ops,
                 (double)comps / ops, (double)rots / ops, colisoes, forma, memoria,
                 (double)itens / ops);
}

// Insere n clientes (poder uniforme em [0, D)), faz qBP buscas exatas,
// qBF buscas por faixa (largura = frac * D) e remove todos em ordem aleatoria.
template <class E>
static void medirOperacoes(FILE* f, const char* exp, long long n, long long D, double frac,
                           int qBP, int qBF, int rep, unsigned long long semente) {
    Rng r(semente);
    long long* poder = new long long[n];
    int* ordem = new int[n];
    long long* consBP = new long long[qBP];
    long long* consBFmin = new long long[qBF];
    long long largura = (long long)(frac * D);
    if (largura < 1) largura = 1;

    for (long long i = 0; i < n; i++) { poder[i] = r.ate(D); ordem[i] = (int)i; }
    embaralhar(ordem, n, r);
    for (int q = 0; q < qBP; q++) consBP[q] = r.ate(D);
    for (int q = 0; q < qBF; q++) consBFmin[q] = (D > largura) ? r.ate(D - largura + 1) : 0;

    lib::vetor<int> saidaBP;
    lib::vetor<EntradaIndice> saidaBF;
    E* s = new E();
    const char* nome = nomeDe(*s);

    // --- insercao ---
    s->resetarEstatisticas();
    Relogio::time_point t0 = Relogio::now();
    for (long long i = 0; i < n; i++) s->inserir(poder[i], (int)i);
    double seg = segundos(t0);
    linhaOperacao(f, exp, nome, n, D, frac, rep, "insercao", n, seg, s->getNumComparacoes(),
                  rotacoesDe(*s), colisoesDe(*s), formaDe(*s), s->getMemoriaBytes(), 0);

    // --- busca exata (BP) ---
    long long itens = 0;
    s->resetarEstatisticas();
    t0 = Relogio::now();
    for (int q = 0; q < qBP; q++) {
        saidaBP.esvaziar();
        s->buscaExata(consBP[q], saidaBP);
        itens += saidaBP.getTamanho();
    }
    seg = segundos(t0);
    linhaOperacao(f, exp, nome, n, D, frac, rep, "busca_exata", qBP, seg, s->getNumComparacoes(),
                  0, 0, formaDe(*s), s->getMemoriaBytes(), itens);
    sumidouro += itens;

    // --- busca por faixa (BF) ---
    itens = 0;
    s->resetarEstatisticas();
    t0 = Relogio::now();
    for (int q = 0; q < qBF; q++) {
        saidaBF.esvaziar();
        s->buscaFaixa(consBFmin[q], consBFmin[q] + largura - 1, saidaBF);
        itens += saidaBF.getTamanho();
    }
    seg = segundos(t0);
    linhaOperacao(f, exp, nome, n, D, frac, rep, "busca_faixa", qBF, seg, s->getNumComparacoes(),
                  0, 0, formaDe(*s), s->getMemoriaBytes(), itens);
    sumidouro += itens;

    // --- remocao ---
    s->resetarEstatisticas();
    t0 = Relogio::now();
    for (long long i = 0; i < n; i++) s->remover(poder[ordem[i]], ordem[i]);
    seg = segundos(t0);
    linhaOperacao(f, exp, nome, n, D, frac, rep, "remocao", n, seg, s->getNumComparacoes(),
                  rotacoesDe(*s), 0, formaDe(*s), s->getMemoriaBytes(), 0);

    delete s;
    delete[] poder; delete[] ordem; delete[] consBP; delete[] consBFmin;
}

// Custo da migracao entre modos, via TAD Indice.
static void medirMigracao(FILE* f, long long n, long long D, int rep, unsigned long long semente) {
    Rng r(semente);
    long long* poder = new long long[n];
    for (long long i = 0; i < n; i++) poder[i] = r.ate(D);

    Indice indice;
    indice.migrarPara("A");
    for (long long i = 0; i < n; i++) indice.inserir(poder[i], (int)i);

    Relogio::time_point t0 = Relogio::now();
    indice.migrarPara("H");
    double segAH = segundos(t0);

    t0 = Relogio::now();
    indice.migrarPara("A");
    double segHA = segundos(t0);

    linhaOperacao(f, "1", "Indice", n, D, 0, rep, "migracao_A_para_H", n, segAH, 0, 0, 0, 0, 0, 0);
    linhaOperacao(f, "1", "Indice", n, D, 0, rep, "migracao_H_para_A", n, segHA, 0, 0, 0, 0, 0, 0);
    delete[] poder;
}

// ---------------------------------------------------------------------
// Experimento 1: AVL x Hash por operacao, variando n (+ migracao)
// ---------------------------------------------------------------------
static void experimento1(const std::string& pasta, int reps) {
    std::fprintf(stderr, "[1] AVL x Hash por operacao\n");
    FILE* f = abrirCsv(pasta, "exp1_operacoes.csv", CAB_OPERACOES);
    long long tamanhos[] = {1000, 10000, 100000, 1000000};
    for (int t = 0; t < 4; t++) {
        long long n = tamanhos[t];
        int qBF = (int)limitar(20000000 / n, 10, 2000);
        for (int rep = 0; rep < reps; rep++) {
            unsigned long long sem = 1000 * (rep + 1) + t;
            medirOperacoes<ArvoreAVL>(f, "1", n, n, 0.01, 10000, qBF, rep, sem);
            medirOperacoes<TabelaHash>(f, "1", n, n, 0.01, 10000, qBF, rep, sem);
            medirMigracao(f, n, n, rep, sem);
        }
        std::fprintf(stderr, "     n = %lld ok\n", n);
    }
    std::fclose(f);
}

// ---------------------------------------------------------------------
// Experimento 2: rotatividade (so entradas x entradas e saidas)
// ---------------------------------------------------------------------

struct OpIndice {
    char tipo;          // 'I' insercao, 'R' remocao
    long long poder;
    int id;
};

// Gera a carga em duas partes, para que as duas cargas operem sobre
// estruturas de tamanho comparavel:
//  - preenchimento (nao medido): n insercoes;
//  - parte medida (n operacoes):
//      "so_entradas": n novas insercoes (tamanho vai de n a 2n);
//      "entra_e_sai": insercoes e remocoes de clientes ativos sorteados,
//                     na proporcao 1:1 (tamanho oscila em torno de n).
// Devolve o total de operacoes; as n primeiras sao o preenchimento.
static long long gerarCarga(OpIndice* ops, long long n, long long D, bool comSaidas, Rng& r) {
    long long maxIds = 2 * n;
    long long* poderDe = new long long[maxIds];
    int* ativos = new int[maxIds];
    long long nAtivos = 0, inseridos = 0, k = 0;
    while (k < 2 * n) {
        bool preenchendo = k < n;
        bool insere = preenchendo || !comSaidas || nAtivos == 0 || r.real() < 0.5;
        if (insere) {
            int id = (int)inseridos++;
            poderDe[id] = r.ate(D);
            ativos[nAtivos++] = id;
            ops[k].tipo = 'I'; ops[k].poder = poderDe[id]; ops[k].id = id; k++;
        } else {
            long long j = r.ate(nAtivos);
            int id = ativos[j];
            ativos[j] = ativos[--nAtivos];
            ops[k].tipo = 'R'; ops[k].poder = poderDe[id]; ops[k].id = id; k++;
        }
    }
    delete[] poderDe; delete[] ativos;
    return k;
}

template <class E>
static void medirRotatividade(FILE* f, FILE* fLinha, long long n, bool comSaidas, int rep,
                              unsigned long long semente) {
    Rng r(semente);
    long long maxOps = 2 * n;
    OpIndice* ops = new OpIndice[maxOps];
    long long total = gerarCarga(ops, n, n, comSaidas, r);
    const char* carga = comSaidas ? "entra_e_sai" : "so_entradas";

    // Passagem 1: tempo total da parte medida, sem instrumentacao no laco
    E* s = new E();
    const char* nome = nomeDe(*s);
    for (long long k = 0; k < n; k++) s->inserir(ops[k].poder, ops[k].id);   // preenchimento
    Relogio::time_point t0 = Relogio::now();
    for (long long k = n; k < total; k++) {
        if (ops[k].tipo == 'I') s->inserir(ops[k].poder, ops[k].id);
        else s->remover(ops[k].poder, ops[k].id);
    }
    double segTotal = segundos(t0);
    long long medidas = total - n;
    delete s;

    // Passagem 2: separa custo por tipo de operacao (tempo medido op a op,
    // o que soma o custo do proprio relogio, igual para as duas estruturas)
    // e registra a "forma" da estrutura ao longo da execucao.
    s = new E();
    double segTipo[2] = {0, 0};
    long long qtd[2] = {0, 0}, comps[2] = {0, 0}, rots[2] = {0, 0};
    long long pontos = 50, passo = total / pontos;
    if (passo < 1) passo = 1;
    for (long long k = 0; k < total; k++) {
        if (k < n) {    // preenchimento: so registra a forma, nao entra nas medias
            s->inserir(ops[k].poder, ops[k].id);
            if (rep == 0 && k % passo == 0)
                std::fprintf(fLinha, "%s,%s,%lld,%lld,%d,%lld\n", nome, carga, n, k + 1,
                             s->getTamanho(), formaDe(*s));
            continue;
        }
        int t = (ops[k].tipo == 'I') ? 0 : 1;
        long long c0 = s->getNumComparacoes(), r0 = rotacoesDe(*s);
        Relogio::time_point a = Relogio::now();
        if (t == 0) s->inserir(ops[k].poder, ops[k].id);
        else s->remover(ops[k].poder, ops[k].id);
        segTipo[t] += segundos(a);
        qtd[t]++;
        comps[t] += s->getNumComparacoes() - c0;
        rots[t] += rotacoesDe(*s) - r0;
        if (rep == 0 && (k % passo == 0 || k == total - 1)) {
            std::fprintf(fLinha, "%s,%s,%lld,%lld,%d,%lld\n", nome, carga, n, k + 1,
                         s->getTamanho(), formaDe(*s));
        }
    }
    delete s;

    const char* nomesTipo[2] = {"insercao", "remocao"};
    std::fprintf(f, "%s,%s,%lld,%d,total,%lld,%.2f,,\n", nome, carga, n, rep, medidas,
                 segTotal * 1e9 / medidas);
    for (int t = 0; t < 2; t++) {
        if (qtd[t] == 0) continue;
        std::fprintf(f, "%s,%s,%lld,%d,%s,%lld,%.2f,%.3f,%.4f\n", nome, carga, n, rep,
                     nomesTipo[t], qtd[t], segTipo[t] * 1e9 / qtd[t],
                     (double)comps[t] / qtd[t], (double)rots[t] / qtd[t]);
    }
    delete[] ops;
}

static void experimento2(const std::string& pasta, int reps) {
    std::fprintf(stderr, "[2] Rotatividade\n");
    FILE* f = abrirCsv(pasta, "exp2_rotatividade.csv",
                       "estrutura,carga,n,rep,operacao,ops,tempo_ns_op,comparacoes_op,rotacoes_op");
    FILE* fl = abrirCsv(pasta, "exp2_linha_tempo.csv",
                        "estrutura,carga,n,passo,tamanho,forma");
    long long tamanhos[] = {10000, 100000, 1000000};
    for (int t = 0; t < 3; t++) {
        for (int rep = 0; rep < reps; rep++) {
            unsigned long long sem = 2000 * (rep + 1) + t;
            for (int c = 0; c < 2; c++) {
                medirRotatividade<ArvoreAVL>(f, fl, tamanhos[t], c == 1, rep, sem);
                medirRotatividade<TabelaHash>(f, fl, tamanhos[t], c == 1, rep, sem);
            }
        }
        std::fprintf(stderr, "     n = %lld ok\n", tamanhos[t]);
    }
    std::fclose(f);
    std::fclose(fl);
}

// ---------------------------------------------------------------------
// Experimento 3: perfis de carga (atualizacoes x consultas)
// ---------------------------------------------------------------------

enum TipoCmd { CMD_C, CMD_D, CMD_F, CMD_Z, CMD_T, CMD_BP, CMD_BF, CMD_CC, NUM_CMD };
static const char* NOME_CMD[NUM_CMD] = {"C", "D", "F", "Z", "T", "BP", "BF", "CC"};

struct Comando {
    int tipo;
    long long a, b;     // poder / min,max / regioes / prioridade / k
    int id;
};

// Estado do "sistema" usado pelo experimento 3 (mesmas chamadas do main).
struct Sistema {
    Indice indice;
    FilaPrioridade fila;
    ConjuntosDisjuntos conjuntos;
    Mapa mapa;
    long long* poderDe;
    lib::vetor<int> saidaBP;
    lib::vetor<EntradaIndice> saidaBF;

    explicit Sistema(long long maxClientes) : poderDe(new long long[maxClientes]) {}
    ~Sistema() { delete[] poderDe; }

    void executar(const Comando& c) {
        switch (c.tipo) {
            case CMD_C: poderDe[c.id] = c.a; indice.inserir(c.a, c.id); break;
            case CMD_D: indice.remover(poderDe[c.id], c.id); break;
            case CMD_F: fila.inserir(c.id, c.a); break;
            case CMD_Z:
                for (long long i = 0; i < c.a && !fila.vazia(); i++) sumidouro += fila.removerMax().idCliente;
                break;
            case CMD_T:
                mapa.adicionarRota((int)c.a, (int)c.b, 1);
                conjuntos.unir((int)c.a, (int)c.b);
                break;
            case CMD_BP:
                saidaBP.esvaziar();
                indice.buscaExata(c.a, saidaBP);
                sumidouro += saidaBP.getTamanho();
                break;
            case CMD_BF:
                saidaBF.esvaziar();
                indice.buscaFaixa(c.a, c.b, saidaBF);
                sumidouro += saidaBF.getTamanho();
                break;
            case CMD_CC:
                sumidouro += conjuntos.conectados((int)c.a, (int)c.b);
                break;
        }
    }
};

// Gera N0 cadastros iniciais + M comandos com a proporcao 'peso' por tipo.
// Clientes de id par nunca saem (podem entrar na fila); de id impar
// podem sair com D (e nunca entram na fila) -- respeita as regras da entrada.
static long long gerarPerfil(Comando* cmds, long long N0, long long M, long long V,
                             long long D, const double* peso, Rng& r, long long& totalClientes) {
    long long maxClientes = N0 + M;
    int* impares = new int[maxClientes];
    long long nImpares = 0, nClientes = 0, k = 0;
    long long largura = D / 100;

    double acum[NUM_CMD];
    double soma = 0;
    for (int t = 0; t < NUM_CMD; t++) { soma += peso[t]; acum[t] = soma; }

    for (long long i = 0; i < N0 + M; i++) {
        int tipo = CMD_C;
        if (i >= N0) {
            double x = r.real() * soma;
            tipo = 0;
            while (tipo < NUM_CMD - 1 && x >= acum[tipo]) tipo++;
        }
        if (tipo == CMD_D && nImpares == 0) tipo = CMD_C;
        Comando& c = cmds[k++];
        c.tipo = tipo; c.a = 0; c.b = 0; c.id = 0;
        switch (tipo) {
            case CMD_C:
                c.id = (int)nClientes++;
                c.a = r.ate(D);
                if (c.id % 2 == 1) impares[nImpares++] = c.id;
                break;
            case CMD_D: {
                long long j = r.ate(nImpares);
                c.id = impares[j];
                impares[j] = impares[--nImpares];
                break;
            }
            case CMD_F: c.id = (int)(2 * r.ate((nClientes + 1) / 2)); c.a = r.ate(100); break;
            case CMD_Z: c.a = 1 + r.ate(5); break;
            case CMD_T: c.a = r.ate(V); c.b = r.ate(V); if (c.a == c.b) c.b = (c.b + 1) % V; break;
            case CMD_BP: c.a = r.ate(D); break;
            case CMD_BF: c.a = r.ate(D - largura); c.b = c.a + largura - 1; break;
            case CMD_CC: c.a = r.ate(V); c.b = r.ate(V); break;
        }
    }
    delete[] impares;
    totalClientes = nClientes;
    return k;
}

static void experimento3(const std::string& pasta, int reps) {
    std::fprintf(stderr, "[3] Perfis de carga\n");
    FILE* f = abrirCsv(pasta, "exp3_perfis.csv",
                       "perfil,modo,rep,comando,ops,tempo_ns_op,tempo_total_s");

    //                         C     D     F     Z     T     BP    BF    CC
    const double atualiza[] = {0.30, 0.25, 0.20, 0.05, 0.05, 0.05, 0.05, 0.05};
    const double consulta[] = {0.05, 0.04, 0.03, 0.01, 0.02, 0.30, 0.25, 0.30};
    const char* nomesPerfil[2] = {"atualizacoes", "consultas"};
    const double* pesos[2] = {atualiza, consulta};

    const long long N0 = 20000, M = 50000, V = 20000, D = 100000, T0 = 10000;
    Comando* cmds = new Comando[N0 + M];

    for (int p = 0; p < 2; p++) {
        for (int rep = 0; rep < reps; rep++) {
            Rng r(3000 * (rep + 1) + p);
            long long totalClientes = 0;
            long long total = gerarPerfil(cmds, N0, M, V, D, pesos[p], r, totalClientes);
            for (int m = 0; m < 2; m++) {
                const char* modo = (m == 0) ? "A" : "H";
                // duas passagens: (1) tempo total limpo; (2) tempo por comando
                for (int passagem = 0; passagem < 2; passagem++) {
                    Sistema* s = new Sistema(totalClientes);
                    s->indice.migrarPara(modo);
                    for (long long v = 0; v < V; v++) { s->mapa.adicionarRegiao("R"); s->conjuntos.criar(); }
                    Rng re(777 + rep);
                    for (long long e = 0; e < T0; e++) {
                        int a = (int)re.ate(V), b = (int)re.ate(V);
                        if (a != b) { s->mapa.adicionarRota(a, b, 1); s->conjuntos.unir(a, b); }
                    }
                    for (long long i = 0; i < N0; i++) s->executar(cmds[i]);   // carga inicial

                    if (passagem == 0) {
                        Relogio::time_point t0 = Relogio::now();
                        for (long long i = N0; i < total; i++) s->executar(cmds[i]);
                        double seg = segundos(t0);
                        std::fprintf(f, "%s,%s,%d,TOTAL,%lld,%.2f,%.6f\n", nomesPerfil[p], modo, rep,
                                     total - N0, seg * 1e9 / (total - N0), seg);
                    } else {
                        double segTipo[NUM_CMD] = {0};
                        long long qtd[NUM_CMD] = {0};
                        for (long long i = N0; i < total; i++) {
                            Relogio::time_point a = Relogio::now();
                            s->executar(cmds[i]);
                            segTipo[cmds[i].tipo] += segundos(a);
                            qtd[cmds[i].tipo]++;
                        }
                        for (int t = 0; t < NUM_CMD; t++) {
                            if (qtd[t] == 0) continue;
                            std::fprintf(f, "%s,%s,%d,%s,%lld,%.2f,%.6f\n", nomesPerfil[p], modo, rep,
                                         NOME_CMD[t], qtd[t], segTipo[t] * 1e9 / qtd[t], segTipo[t]);
                        }
                    }
                    delete s;
                }
            }
        }
        std::fprintf(stderr, "     perfil %s ok\n", nomesPerfil[p]);
    }
    delete[] cmds;
    std::fclose(f);
}

// ---------------------------------------------------------------------
// Experimento 4: conectividade -- Union-Find x BFS
// ---------------------------------------------------------------------
static void experimento4(const std::string& pasta, int reps) {
    std::fprintf(stderr, "[4] Conectividade: Union-Find x BFS\n");
    FILE* f = abrirCsv(pasta, "exp4_conectividade.csv",
                       "regioes,grau_medio,rotas,rep,metodo,fase,ops,tempo_ns_op,"
                       "visitadas_op,passos_op,frac_sim,memoria_bytes");
    long long tamanhos[] = {1000, 10000, 100000};
    double graus[] = {0.5, 1.0, 2.0, 4.0, 16.0};

    for (int t = 0; t < 3; t++) {
        long long V = tamanhos[t];
        for (int g = 0; g < 5; g++) {
            long long E = (long long)(graus[g] * V / 2);
            for (int rep = 0; rep < reps; rep++) {
                Rng r(4000 * (rep + 1) + 10 * t + g);
                int* ru = new int[E];
                int* rv = new int[E];
                for (long long e = 0; e < E; e++) {
                    ru[e] = (int)r.ate(V);
                    rv[e] = (int)r.ate(V - 1);
                    if (rv[e] >= ru[e]) rv[e]++;     // evita rota de uma regiao para ela mesma
                }
                long long qUF = 100000;
                long long qBFS = limitar(20000000 / (V + 2 * E), 20, 2000);
                int* qa = new int[qUF];
                int* qb = new int[qUF];
                for (long long q = 0; q < qUF; q++) { qa[q] = (int)r.ate(V); qb[q] = (int)r.ate(V); }

                // --- construcao: Mapa (o que a BFS precisa) ---
                Mapa* mapa = new Mapa();
                Relogio::time_point t0 = Relogio::now();
                for (long long v = 0; v < V; v++) mapa->adicionarRegiao("R");
                for (long long e = 0; e < E; e++) mapa->adicionarRota(ru[e], rv[e], 1);
                double segMapa = segundos(t0);

                // --- construcao: Union-Find ---
                ConjuntosDisjuntos* uf = new ConjuntosDisjuntos();
                t0 = Relogio::now();
                for (long long v = 0; v < V; v++) uf->criar();
                for (long long e = 0; e < E; e++) uf->unir(ru[e], rv[e]);
                double segUF = segundos(t0);

                // --- consultas: Union-Find ---
                uf->resetarEstatisticas();
                long long simUF = 0;
                t0 = Relogio::now();
                for (long long q = 0; q < qUF; q++) simUF += uf->conectados(qa[q], qb[q]);
                double segQUF = segundos(t0);
                double passosUF = (double)uf->getNumPassos() / qUF;

                // --- consultas: BFS (mesmos pares, menos consultas) ---
                mapa->resetarEstatisticas();
                long long simBFS = 0;
                t0 = Relogio::now();
                for (long long q = 0; q < qBFS; q++) simBFS += mapa->alcancavelBFS(qa[q], qb[q]);
                double segQBFS = segundos(t0);
                double visitadas = (double)mapa->getTotalVisitadas() / qBFS;

                std::fprintf(f, "%lld,%g,%lld,%d,BFS,construcao,%lld,%.2f,,,,%lld\n",
                             V, graus[g], E, rep, V + E, segMapa * 1e9 / (V + E), mapa->getMemoriaBytes());
                std::fprintf(f, "%lld,%g,%lld,%d,UnionFind,construcao,%lld,%.2f,,,,%lld\n",
                             V, graus[g], E, rep, V + E, segUF * 1e9 / (V + E), uf->getMemoriaBytes());
                std::fprintf(f, "%lld,%g,%lld,%d,UnionFind,consulta,%lld,%.2f,,%.3f,%.4f,\n",
                             V, graus[g], E, rep, qUF, segQUF * 1e9 / qUF, passosUF, (double)simUF / qUF);
                std::fprintf(f, "%lld,%g,%lld,%d,BFS,consulta,%lld,%.2f,%.1f,,%.4f,\n",
                             V, graus[g], E, rep, qBFS, segQBFS * 1e9 / qBFS, visitadas, (double)simBFS / qBFS);

                delete mapa; delete uf;
                delete[] ru; delete[] rv; delete[] qa; delete[] qb;
            }
        }
        std::fprintf(stderr, "     regioes = %lld ok\n", V);
    }
    std::fclose(f);
}

// ---------------------------------------------------------------------
// Experimento 5: parametros que mudam o custo
//   5a: numero de poderes distintos (D), com n fixo
//   5b: largura da faixa do BF (seletividade), com n fixo
// ---------------------------------------------------------------------
static void experimento5(const std::string& pasta, int reps) {
    std::fprintf(stderr, "[5] Variacao de parametros\n");
    FILE* f = abrirCsv(pasta, "exp5_parametros.csv", CAB_OPERACOES);

    const long long n = 50000;
    long long chaves[] = {1, 10, 100, 1000, 10000, 100000, 1000000};
    for (int c = 0; c < 7; c++) {
        for (int rep = 0; rep < reps; rep++) {
            unsigned long long sem = 5000 * (rep + 1) + c;
            medirOperacoes<ArvoreAVL>(f, "5a", n, chaves[c], 0.01, 1000, 200, rep, sem);
            medirOperacoes<TabelaHash>(f, "5a", n, chaves[c], 0.01, 1000, 200, rep, sem);
        }
        std::fprintf(stderr, "     5a: chaves distintas = %lld ok\n", chaves[c]);
    }

    const long long n2 = 100000, D2 = 1000000;
    double fracs[] = {0.0001, 0.001, 0.01, 0.1, 1.0};
    for (int i = 0; i < 5; i++) {
        for (int rep = 0; rep < reps; rep++) {
            unsigned long long sem = 5500 * (rep + 1) + i;
            medirOperacoes<ArvoreAVL>(f, "5b", n2, D2, fracs[i], 100, 200, rep, sem);
            medirOperacoes<TabelaHash>(f, "5b", n2, D2, fracs[i], 100, 200, rep, sem);
        }
        std::fprintf(stderr, "     5b: largura da faixa = %g ok\n", fracs[i]);
    }
    std::fclose(f);
}

// ---------------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <1|2|3|4|5|todos> <pasta_saida> [repeticoes]\n", argv[0]);
        return 1;
    }
    std::string qual = argv[1];
    std::string pasta = argv[2];
    int reps = (argc > 3) ? std::atoi(argv[3]) : 3;

    Relogio::time_point t0 = Relogio::now();
    if (qual == "1" || qual == "todos") experimento1(pasta, reps);
    if (qual == "2" || qual == "todos") experimento2(pasta, reps);
    if (qual == "3" || qual == "todos") experimento3(pasta, reps);
    if (qual == "4" || qual == "todos") experimento4(pasta, reps);
    if (qual == "5" || qual == "todos") experimento5(pasta, reps);
    std::fprintf(stderr, "Concluido em %.1f s (sumidouro %lld)\n", segundos(t0), (long long)sumidouro);
    return 0;
}
