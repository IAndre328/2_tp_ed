#ifndef TABELAHASH_H
#define TABELAHASH_H

#include "lib.h"
#include "EntradaIndice.h"


struct NoHash {
    long long poder;
    lib::vetor<int> ids;
    NoHash* prox;

    NoHash(long long p) : poder(p), ids(), prox(nullptr) {}
};

class TabelaHash {
private:
    NoHash** baldes;
    int numBaldes;      // M = 2^bitsBaldes
    int bitsBaldes;
    int numChaves;      // entradas (poderes distintos)
    int numElementos;   // clientes (pares poder, id)

    // Contadores
    long long numComparacoes;   // comparacoes de chave ao percorrer listas
    long long numColisoes;      // novas entradas em balde ja ocupado
    long long numRehash;

    int posicao(long long poder) const;
    NoHash* buscarNo(long long poder);
    void rehash();

    void mergeSort(lib::vetor<NoHash*>& v, lib::vetor<NoHash*>& aux, int ini, int fim);

    TabelaHash(const TabelaHash&);
    TabelaHash& operator=(const TabelaHash&);

public:
    TabelaHash();
    ~TabelaHash();

    // devolvem false se o par ja existia / nao existia
    bool inserir(long long poder, int id);
    bool remover(long long poder, int id);

    // ids com poder == p, em ordem crescente de id
    void buscaExata(long long poder, lib::vetor<int>& saida);

    // pares com poder em [min, max], em ordem (poder, id)
    void buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida);

    // Preparar para migrar
    void listarTodos(lib::vetor<EntradaIndice>& saida);

    int getTamanho() const;         // numero de clientes
    int getNumChaves() const;       // numero de poderes distintos
    int getNumBaldes() const;
    int getMaiorBalde() const;      // comprimento da maior lista

    // Contadores
    long long getNumComparacoes() const;
    long long getNumColisoes() const;
    long long getNumRehash() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif
