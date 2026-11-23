#ifndef ENTRADAINDICE_H
#define ENTRADAINDICE_H

// Par (poder, id) usado pelo indice: e a chave composta da AVL e o
// formato de saida das buscas por faixa e da migracao entre modos.
struct EntradaIndice {
    long long poder;
    int idCliente;

    EntradaIndice() : poder(0), idCliente(-1) {}
    EntradaIndice(long long p, int id) : poder(p), idCliente(id) {}
};

#endif // ENTRADAINDICE_H
