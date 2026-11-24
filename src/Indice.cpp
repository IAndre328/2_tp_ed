#include "Indice.h"

Indice::Indice() : modo(""), arvore(nullptr), tabela(nullptr) {}

Indice::~Indice() {
    delete arvore;
    delete tabela;
}

void Indice::migrarPara(const std::string& novoModo) {
    if (novoModo == modo) return;   // ja esta no modo pedido

    
    lib::vetor<EntradaIndice> todos;
    if (arvore != nullptr) arvore->listarTodos(todos);
    if (tabela != nullptr) tabela->listarTodos(todos);

    ArvoreAVL* novaArvore = nullptr;
    TabelaHash* novaTabela = nullptr;
    if (novoModo == "A") {
        novaArvore = new ArvoreAVL();
        for (int i = 0; i < todos.getTamanho(); i++)
            novaArvore->inserir(todos.getById(i).poder, todos.getById(i).idCliente);
    } else {
        novaTabela = new TabelaHash();
        for (int i = 0; i < todos.getTamanho(); i++)
            novaTabela->inserir(todos.getById(i).poder, todos.getById(i).idCliente);
    }

    delete arvore;
    delete tabela;
    arvore = novaArvore;
    tabela = novaTabela;
    modo = novoModo;
}

void Indice::inserir(long long poder, int idCliente) {
    if (arvore != nullptr) arvore->inserir(poder, idCliente);
    else if (tabela != nullptr) tabela->inserir(poder, idCliente);
}

void Indice::remover(long long poder, int idCliente) {
    if (arvore != nullptr) arvore->remover(poder, idCliente);
    else if (tabela != nullptr) tabela->remover(poder, idCliente);
}

void Indice::buscaExata(long long poder, lib::vetor<int>& saida) {
    if (arvore != nullptr) arvore->buscaExata(poder, saida);
    else if (tabela != nullptr) tabela->buscaExata(poder, saida);
}

void Indice::buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida) {
    if (arvore != nullptr) arvore->buscaFaixa(min, max, saida);
    else if (tabela != nullptr) tabela->buscaFaixa(min, max, saida);
}

const std::string& Indice::getModo() const { return modo; }

int Indice::getTamanho() const {
    if (arvore != nullptr) return arvore->getTamanho();
    if (tabela != nullptr) return tabela->getTamanho();
    return 0;
}

const ArvoreAVL* Indice::getArvore() const { return arvore; }
const TabelaHash* Indice::getTabela() const { return tabela; }
