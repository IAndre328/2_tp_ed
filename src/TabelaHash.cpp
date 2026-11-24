#include "TabelaHash.h"

static const int BITS_INICIAIS = 4;     // 16 baldes no inicio

TabelaHash::TabelaHash()
    : baldes(nullptr), numBaldes(1 << BITS_INICIAIS), bitsBaldes(BITS_INICIAIS),
      numChaves(0), numElementos(0), numComparacoes(0), numColisoes(0), numRehash(0) {
    baldes = new NoHash*[numBaldes];
    for (int i = 0; i < numBaldes; i++) baldes[i] = nullptr;
}

TabelaHash::~TabelaHash() {
    for (int i = 0; i < numBaldes; i++) {
        NoHash* atual = baldes[i];
        while (atual != nullptr) {
            NoHash* prox = atual->prox;
            delete atual;
            atual = prox;
        }
    }
    delete[] baldes;
}

// Funcao de espalhamento e rehash

// Hash multiplicativo (Knuth): multiplica pela constante 2^64 / phi e
// usa os 'bitsBaldes' bits mais altos do produto como indice do balde.
int TabelaHash::posicao(long long poder) const {
    unsigned long long x = (unsigned long long)poder;
    unsigned long long h = x * 11400714819323198485ull;
    return (int)(h >> (64 - bitsBaldes));
}

// Dobra o numero de baldes e religa cada no no seu novo balde.
void TabelaHash::rehash() {
    int antigoNum = numBaldes;
    NoHash** antigos = baldes;

    bitsBaldes++;
    numBaldes = 1 << bitsBaldes;
    baldes = new NoHash*[numBaldes];
    for (int i = 0; i < numBaldes; i++) baldes[i] = nullptr;

    for (int i = 0; i < antigoNum; i++) {
        NoHash* atual = antigos[i];
        while (atual != nullptr) {
            NoHash* prox = atual->prox;
            int p = posicao(atual->poder);
            atual->prox = baldes[p];    
            baldes[p] = atual;
            atual = prox;
        }
    }
    delete[] antigos;
    numRehash++;
}

NoHash* TabelaHash::buscarNo(long long poder) {
    NoHash* atual = baldes[posicao(poder)];
    while (atual != nullptr) {
        numComparacoes++;
        if (atual->poder == poder) return atual;
        atual = atual->prox;
    }
    return nullptr;
}

// Insercao e remocao

bool TabelaHash::inserir(long long poder, int id) {
    NoHash* no = buscarNo(poder);

    if (no == nullptr) {
        int p = posicao(poder);
        if (baldes[p] != nullptr) numColisoes++;
        no = new NoHash(poder);
        no->prox = baldes[p];
        baldes[p] = no;
        numChaves++;
    }

    // insere o id mantendo a ordem crescente.
    lib::vetor<int>& ids = no->ids;
    int pos = ids.getTamanho();
    while (pos > 0 && ids.getById(pos - 1) > id) pos--;
    if (pos > 0 && ids.getById(pos - 1) == id) return false;   // ja existia
    ids.inserir_na_posicao(pos, id);
    numElementos++;

    // fator de carga > 0,75  <=>  4 * chaves > 3 * baldes
    if (4LL * numChaves > 3LL * numBaldes) rehash();
    return true;
}

bool TabelaHash::remover(long long poder, int id) {
    int p = posicao(poder);
    NoHash* anterior = nullptr;
    NoHash* atual = baldes[p];
    while (atual != nullptr) {
        numComparacoes++;
        if (atual->poder == poder) break;
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == nullptr) return false;

    lib::vetor<int>& ids = atual->ids;
    int ini = 0, fim = ids.getTamanho() - 1, achou = -1;
    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        if (ids.getById(meio) == id) { achou = meio; break; }
        if (ids.getById(meio) < id) ini = meio + 1;
        else fim = meio - 1;
    }
    if (achou == -1) return false;

    ids.remover_da_posicao(achou);
    numElementos--;

    // ninguem mais com esse poder: tira a entrada da lista do balde
    if (ids.isEmpty()) {
        if (anterior == nullptr) baldes[p] = atual->prox;
        else anterior->prox = atual->prox;
        delete atual;
        numChaves--;
    }
    return true;
}

// Buscas

void TabelaHash::buscaExata(long long poder, lib::vetor<int>& saida) {
    NoHash* no = buscarNo(poder);
    if (no == nullptr) return;
    for (int i = 0; i < no->ids.getTamanho(); i++) saida.add_no_fim(no->ids.getById(i));
}

// Ordena por poder
void TabelaHash::mergeSort(lib::vetor<NoHash*>& v, lib::vetor<NoHash*>& aux, int ini, int fim) {
    if (ini >= fim) return;
    int meio = (ini + fim) / 2;
    mergeSort(v, aux, ini, meio);
    mergeSort(v, aux, meio + 1, fim);

    int i = ini, j = meio + 1, k = ini;
    while (i <= meio && j <= fim) {
        numComparacoes++;
        if (v.getById(i)->poder <= v.getById(j)->poder) aux.getById(k++) = v.getById(i++);
        else aux.getById(k++) = v.getById(j++);
    }
    while (i <= meio) aux.getById(k++) = v.getById(i++);
    while (j <= fim)  aux.getById(k++) = v.getById(j++);
    for (k = ini; k <= fim; k++) v.getById(k) = aux.getById(k);
}

void TabelaHash::buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida) {
    lib::vetor<NoHash*> selecionados;
    for (int i = 0; i < numBaldes; i++) {
        for (NoHash* atual = baldes[i]; atual != nullptr; atual = atual->prox) {
            numComparacoes++;
            if (atual->poder >= min && atual->poder <= max) selecionados.add_no_fim(atual);
        }
    }
    if (selecionados.isEmpty()) return;

    lib::vetor<NoHash*> aux = selecionados;
    mergeSort(selecionados, aux, 0, selecionados.getTamanho() - 1);

    for (int i = 0; i < selecionados.getTamanho(); i++) {
        NoHash* no = selecionados.getById(i);
        for (int j = 0; j < no->ids.getTamanho(); j++) {
            saida.add_no_fim(EntradaIndice(no->poder, no->ids.getById(j)));
        }
    }
}

void TabelaHash::listarTodos(lib::vetor<EntradaIndice>& saida) {
    for (int i = 0; i < numBaldes; i++) {
        for (NoHash* atual = baldes[i]; atual != nullptr; atual = atual->prox) {
            for (int j = 0; j < atual->ids.getTamanho(); j++) {
                saida.add_no_fim(EntradaIndice(atual->poder, atual->ids.getById(j)));
            }
        }
    }
}

// Consultas e instrumentacao

int TabelaHash::getTamanho() const { return numElementos; }
int TabelaHash::getNumChaves() const { return numChaves; }
int TabelaHash::getNumBaldes() const { return numBaldes; }

int TabelaHash::getMaiorBalde() const {
    int maior = 0;
    for (int i = 0; i < numBaldes; i++) {
        int c = 0;
        for (NoHash* atual = baldes[i]; atual != nullptr; atual = atual->prox) c++;
        if (c > maior) maior = c;
    }
    return maior;
}

long long TabelaHash::getNumComparacoes() const { return numComparacoes; }
long long TabelaHash::getNumColisoes() const { return numColisoes; }
long long TabelaHash::getNumRehash() const { return numRehash; }

long long TabelaHash::getMemoriaBytes() const {
    return (long long)sizeof(*this)
         + (long long)numBaldes * (long long)sizeof(NoHash*)
         + (long long)numChaves * (long long)sizeof(NoHash)
         + (long long)numElementos * (long long)sizeof(int);
}

void TabelaHash::resetarEstatisticas() {
    numComparacoes = 0;
    numColisoes = 0;
    numRehash = 0;
}
