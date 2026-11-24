#ifndef ENTRADAINDICE_H
#define ENTRADAINDICE_H

struct EntradaIndice {
    long long poder;
    int idCliente;

    EntradaIndice() : poder(0), idCliente(-1) {}
    EntradaIndice(long long p, int id) : poder(p), idCliente(id) {}
};

#endif
