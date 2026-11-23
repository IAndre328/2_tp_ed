#ifndef FILAPRIORIDADE_H
#define FILAPRIORIDADE_H

#include "lib.h"

// Elemento da fila: um cliente aguardando com uma prioridade.
// O mesmo cliente pode aparecer varias vezes (uma por entrada na fila).
struct ItemFila {
    long long prioridade;
    int idCliente;

    ItemFila() : prioridade(0), idCliente(-1) {}
    ItemFila(long long p, int id) : prioridade(p), idCliente(id) {}
};

// TAD Fila de Prioridade
// Heap binario de maximo armazenado em vetor (filhos de i: 2i+1 e 2i+2).
// Ordem: maior prioridade primeiro; em empate, menor id de cliente.
// inserir e removerMax custam O(log n).
class FilaPrioridade {
private:
    lib::vetor<ItemFila> heap;

    // instrumentacao
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

    // instrumentacao
    long long getNumComparacoes() const;
    long long getNumTrocas() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif // FILAPRIORIDADE_H
