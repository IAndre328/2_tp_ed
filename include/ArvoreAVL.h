#ifndef ARVOREAVL_H
#define ARVOREAVL_H

#include "lib.h"
#include "EntradaIndice.h"


struct NoAVL {
    long long poder;
    int id;
    int altura;     // altura da subarvore enraizada aqui (folha = 1)
    NoAVL* esq;
    NoAVL* dir;

    NoAVL(long long p, int i) : poder(p), id(i), altura(1), esq(nullptr), dir(nullptr) {}
};

// TAD Arvore AVL (Modo A)
class ArvoreAVL {
private:
    NoAVL* raiz;
    int tamanho;

    // Contadores
    long long numComparacoes;   // comparacoes de chave
    long long numRotacoes;      // rotacoes simples (uma dupla conta 2)

    int altura(const NoAVL* n) const;
    int fatorBalanceamento(const NoAVL* n) const;
    void atualizarAltura(NoAVL* n);

    NoAVL* rotacaoDireita(NoAVL* y);
    NoAVL* rotacaoEsquerda(NoAVL* x);
    NoAVL* balancear(NoAVL* n);

    int comparar(long long poder, int id, const NoAVL* n);

    NoAVL* inserirRec(NoAVL* n, long long poder, int id, bool& inseriu);
    NoAVL* removerRec(NoAVL* n, long long poder, int id, bool& removeu);
    NoAVL* desligarMinimo(NoAVL* n, NoAVL*& minimo);
    void faixaRec(NoAVL* n, long long min, long long max, lib::vetor<EntradaIndice>& saida);
    void destruirRec(NoAVL* n);
    int verificarRec(const NoAVL* n, bool& ok) const;

    ArvoreAVL(const ArvoreAVL&);
    ArvoreAVL& operator=(const ArvoreAVL&);

public:
    ArvoreAVL();
    ~ArvoreAVL();

    // devolvem false se a chave ja existia / nao existia
    bool inserir(long long poder, int id);
    bool remover(long long poder, int id);

    // Em ordem crescente de id
    void buscaExata(long long poder, lib::vetor<int>& saida);

    // Pares com poder em [min, max], em ordem (poder, id)
    void buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida);

    // Preparar para migração (em ordem)
    void listarTodos(lib::vetor<EntradaIndice>& saida);

    int getTamanho() const;
    int getAltura() const;

    // confere ordem e balanceamento de todos os nos (para testes)
    bool verificar() const;

    // Contadores
    long long getNumComparacoes() const;
    long long getNumRotacoes() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif 
