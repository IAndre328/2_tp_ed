#ifndef ARVOREAVL_H
#define ARVOREAVL_H

#include "lib.h"
#include "EntradaIndice.h"

// No da AVL. A chave e o par (poder, id): como o id e unico, nao ha
// chaves repetidas, e o percurso em ordem ja sai ordenado por poder e,
// em empate, por id (exatamente a ordem pedida pelo BF).
struct NoAVL {
    long long poder;
    int id;
    int altura;     // altura da subarvore enraizada aqui (folha = 1)
    NoAVL* esq;
    NoAVL* dir;

    NoAVL(long long p, int i) : poder(p), id(i), altura(1), esq(nullptr), dir(nullptr) {}
};

// TAD Arvore AVL (Modo A)
// Arvore binaria de busca balanceada por altura: em todo no, as alturas
// das subarvores esquerda e direita diferem de no maximo 1. Isso garante
// altura O(log n) e, portanto, inserir/remover/buscar em O(log n).
class ArvoreAVL {
private:
    NoAVL* raiz;
    int tamanho;

    // instrumentacao
    long long numComparacoes;   // comparacoes de chave
    long long numRotacoes;      // rotacoes simples (uma dupla conta 2)

    int altura(const NoAVL* n) const;
    int fatorBalanceamento(const NoAVL* n) const;
    void atualizarAltura(NoAVL* n);

    NoAVL* rotacaoDireita(NoAVL* y);
    NoAVL* rotacaoEsquerda(NoAVL* x);
    NoAVL* balancear(NoAVL* n);

    // <0 se (poder,id) vem antes do no, 0 se igual, >0 se vem depois
    int comparar(long long poder, int id, const NoAVL* n);

    NoAVL* inserirRec(NoAVL* n, long long poder, int id, bool& inseriu);
    NoAVL* removerRec(NoAVL* n, long long poder, int id, bool& removeu);
    NoAVL* desligarMinimo(NoAVL* n, NoAVL*& minimo);
    void faixaRec(NoAVL* n, long long min, long long max, lib::vetor<EntradaIndice>& saida);
    void destruirRec(NoAVL* n);
    int verificarRec(const NoAVL* n, bool& ok) const;

    // copia proibida (a arvore e dona dos nos)
    ArvoreAVL(const ArvoreAVL&);
    ArvoreAVL& operator=(const ArvoreAVL&);

public:
    ArvoreAVL();
    ~ArvoreAVL();

    // devolvem false se a chave ja existia / nao existia
    bool inserir(long long poder, int id);
    bool remover(long long poder, int id);

    // ids com poder == p, em ordem crescente de id
    void buscaExata(long long poder, lib::vetor<int>& saida);

    // pares com poder em [min, max], em ordem (poder, id)
    void buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida);

    // todos os pares em ordem (usado na migracao)
    void listarTodos(lib::vetor<EntradaIndice>& saida);

    int getTamanho() const;
    int getAltura() const;

    // confere ordem e balanceamento de todos os nos (para testes)
    bool verificar() const;

    // instrumentacao
    long long getNumComparacoes() const;
    long long getNumRotacoes() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif // ARVOREAVL_H
