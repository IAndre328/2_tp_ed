#include "ConjuntosDisjuntos.h"

ConjuntosDisjuntos::ConjuntosDisjuntos()
    : pai(), rank(), numBuscas(0), numPassos(0), numUnioes(0) {}

int ConjuntosDisjuntos::criar() {
    int x = pai.getTamanho();
    pai.add_no_fim(x);   // raiz de si mesmo
    rank.add_no_fim(0);
    return x;
}

int ConjuntosDisjuntos::encontrar(int x) {
    numBuscas++;

    // 1a passada: sobe ate a raiz
    int raiz = x;
    while (pai.getById(raiz) != raiz) {
        raiz = pai.getById(raiz);
        numPassos++;
    }

    // 2a passada: compressao de caminho (todos no caminho apontam para a raiz)
    while (pai.getById(x) != raiz) {
        int proximo = pai.getById(x);
        pai.getById(x) = raiz;
        x = proximo;
    }
    return raiz;
}

bool ConjuntosDisjuntos::unir(int a, int b) {
    int ra = encontrar(a);
    int rb = encontrar(b);
    if (ra == rb) return false;

    // uniao por rank: a arvore mais baixa vira filha da mais alta
    if (rank.getById(ra) < rank.getById(rb)) {
        pai.getById(ra) = rb;
    } else if (rank.getById(ra) > rank.getById(rb)) {
        pai.getById(rb) = ra;
    } else {
        pai.getById(rb) = ra;
        rank.getById(ra)++;
    }
    numUnioes++;
    return true;
}

bool ConjuntosDisjuntos::conectados(int a, int b) {
    return encontrar(a) == encontrar(b);
}

int ConjuntosDisjuntos::getTamanho() const { return pai.getTamanho(); }

long long ConjuntosDisjuntos::getNumBuscas() const { return numBuscas; }
long long ConjuntosDisjuntos::getNumPassos() const { return numPassos; }
long long ConjuntosDisjuntos::getNumUnioes() const { return numUnioes; }

long long ConjuntosDisjuntos::getMemoriaBytes() const {
    return (long long)sizeof(*this) + 2LL * pai.getTamanho() * (long long)sizeof(int);
}

void ConjuntosDisjuntos::resetarEstatisticas() {
    numBuscas = 0;
    numPassos = 0;
    numUnioes = 0;
}
