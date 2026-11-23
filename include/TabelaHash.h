#ifndef TABELAHASH_H
#define TABELAHASH_H

#include "lib.h"
#include "EntradaIndice.h"

// Entrada da tabela: um poder distinto e todos os ids de clientes ativos
// com esse poder, em ordem crescente. 'prox' encadeia as entradas que
// cairam no mesmo balde (encadeamento separado).
struct NoHash {
    long long poder;
    lib::vetor<int> ids;
    NoHash* prox;

    NoHash(long long p) : poder(p), ids(), prox(nullptr) {}
};

// TAD Tabela Hash (Modo H)
//
// - Chave: o poder. Varios clientes com o mesmo poder ficam na mesma entrada.
// - Colisoes: encadeamento separado. A remocao e simples (tira o no da
//   lista), sem as "lapides" que o enderecamento aberto exigiria.
// - Espalhamento: multiplicativo de Knuth. O poder e convertido para
//   unsigned long long (poderes negativos viram inteiros validos),
//   multiplicado por 2^64/phi e os bits mais altos dao o balde. O numero
//   de baldes e potencia de 2, entao nao ha divisao.
// - Fator de carga (entradas / baldes) limitado a 0,75: ao passar disso,
//   o numero de baldes dobra e tudo e redistribuido (rehash).
//
// Busca exata, insercao e remocao: O(1) em media.
// Busca por faixa: a tabela nao guarda ordem, entao e preciso varrer todos
// os baldes e ordenar o resultado (merge sort): O(M + n + d log d).
class TabelaHash {
private:
    NoHash** baldes;
    int numBaldes;      // M = 2^bitsBaldes
    int bitsBaldes;
    int numChaves;      // entradas (poderes distintos)
    int numElementos;   // clientes (pares poder, id)

    // instrumentacao
    long long numComparacoes;   // comparacoes de chave ao percorrer listas
    long long numColisoes;      // novas entradas em balde ja ocupado
    long long numRehash;

    int posicao(long long poder) const;
    NoHash* buscarNo(long long poder);
    void rehash();

    void mergeSort(lib::vetor<NoHash*>& v, lib::vetor<NoHash*>& aux, int ini, int fim);

    // copia proibida (a tabela e dona dos nos)
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

    // todos os pares, sem ordem definida (usado na migracao)
    void listarTodos(lib::vetor<EntradaIndice>& saida);

    int getTamanho() const;         // numero de clientes
    int getNumChaves() const;       // numero de poderes distintos
    int getNumBaldes() const;
    int getMaiorBalde() const;      // comprimento da maior lista

    // instrumentacao
    long long getNumComparacoes() const;
    long long getNumColisoes() const;
    long long getNumRehash() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif // TABELAHASH_H
