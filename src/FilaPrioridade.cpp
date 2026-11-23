#include "FilaPrioridade.h"

FilaPrioridade::FilaPrioridade() : heap(), numComparacoes(0), numTrocas(0) {}

bool FilaPrioridade::vemAntes(const ItemFila& a, const ItemFila& b) {
    numComparacoes++;
    if (a.prioridade != b.prioridade) return a.prioridade > b.prioridade;
    return a.idCliente < b.idCliente;
}

void FilaPrioridade::trocar(int i, int j) {
    ItemFila tmp = heap.getById(i);
    heap.getById(i) = heap.getById(j);
    heap.getById(j) = tmp;
    numTrocas++;
}

// sobe o item da posicao i enquanto ele vier antes do pai
void FilaPrioridade::subir(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (!vemAntes(heap.getById(i), heap.getById(p))) break;
        trocar(i, p);
        i = p;
    }
}

// desce o item da posicao i trocando com o melhor filho enquanto preciso
void FilaPrioridade::descer(int i) {
    int n = heap.getTamanho();
    while (true) {
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        int melhor = i;
        if (esq < n && vemAntes(heap.getById(esq), heap.getById(melhor))) melhor = esq;
        if (dir < n && vemAntes(heap.getById(dir), heap.getById(melhor))) melhor = dir;
        if (melhor == i) break;
        trocar(i, melhor);
        i = melhor;
    }
}

void FilaPrioridade::inserir(int idCliente, long long prioridade) {
    heap.add_no_fim(ItemFila(prioridade, idCliente));
    subir(heap.getTamanho() - 1);
}

ItemFila FilaPrioridade::removerMax() {
    assert(!vazia());
    ItemFila maior = heap.getById(0);
    int ultimo = heap.getTamanho() - 1;
    heap.getById(0) = heap.getById(ultimo);
    heap.rem_no_fim();
    if (!vazia()) descer(0);
    return maior;
}

const ItemFila& FilaPrioridade::topo() const {
    assert(!vazia());
    return heap.getById(0);
}

int FilaPrioridade::getTamanho() const { return heap.getTamanho(); }

bool FilaPrioridade::vazia() const { return heap.getTamanho() == 0; }

long long FilaPrioridade::getNumComparacoes() const { return numComparacoes; }
long long FilaPrioridade::getNumTrocas() const { return numTrocas; }

long long FilaPrioridade::getMemoriaBytes() const {
    return (long long)sizeof(*this) + (long long)heap.getTamanho() * (long long)sizeof(ItemFila);
}

void FilaPrioridade::resetarEstatisticas() {
    numComparacoes = 0;
    numTrocas = 0;
}
