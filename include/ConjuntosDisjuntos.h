#ifndef CONJUNTOSDISJUNTOS_H
#define CONJUNTOSDISJUNTOS_H

#include "lib.h"


// Mantem as regioes agrupadas por conectividade.

class ConjuntosDisjuntos {
private:
    lib::vetor<int> pai;
    lib::vetor<int> rank;

    // Contadores (analise experimental)
    long long numBuscas;    // chamadas de encontrar
    long long numPassos;    // saltos de pai percorridos nas buscas
    long long numUnioes;    // unioes que efetivamente juntaram dois conjuntos

public:
    ConjuntosDisjuntos();

    // cria um conjunto unitario {x} e devolve x (ids crescentes a partir de 0)
    int criar();

    // devolve o representante (raiz) do conjunto de x
    int encontrar(int x);

    // une os conjuntos de a e b; devolve false se ja estavam juntos
    bool unir(int a, int b);

    // true se a e b estao no mesmo conjunto (comando CC)
    bool conectados(int a, int b);

    int getTamanho() const;

    // Contadores
    long long getNumBuscas() const;
    long long getNumPassos() const;
    long long getNumUnioes() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif
