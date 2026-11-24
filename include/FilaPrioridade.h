#ifndef FILAPRIORIDADE_H
#define FILAPRIORIDADE_H

#include "lib.h"

struct ItemFila {
    long long prioridade;
    int idCliente;

    ItemFila() : prioridade(0), idCliente(-1) {}
    ItemFila(long long p, int id) : prioridade(p), idCliente(id) {}
};

class FilaPrioridade {
private:
    lib::vetor<ItemFila> heap;

    // Contadores
    long long numComparacoes;
    long long numTrocas;

    // true se 'a' deve ser atendido antes de 'b'
    bool vemAntes(const ItemFila& a, const ItemFila& b);
    void trocar(int i, int j);
    void subir(int i);
    void descer(int i);

public:
    FilaPrioridade();

    void inserir(int idCliente, long long prioridade);

    // remove e devolve o item de maior prioridade (fila nao pode estar vazia)
    ItemFila removerMax();

    const ItemFila& topo() const;
    int getTamanho() const;
    bool vazia() const;

    // Contadores
    long long getNumComparacoes() const;
    long long getNumTrocas() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif
