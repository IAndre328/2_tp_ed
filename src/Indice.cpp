#include "Indice.h"

// ATENCAO: implementacao provisoria (stubs) para o main compilar.
// A logica real com AVL e Tabela Hash entra na Fase 2.

Indice::Indice() : modo(""), tamanho(0) {}

Indice::~Indice() {}

void Indice::migrarPara(const std::string& novoModo) {
    modo = novoModo;   // TODO (Fase 2): migrar os dados de uma estrutura para a outra
}

void Indice::inserir(long long poder, int idCliente) {
    (void)poder; (void)idCliente;
    tamanho++;         // TODO (Fase 2)
}

void Indice::remover(long long poder, int idCliente) {
    (void)poder; (void)idCliente;
    tamanho--;         // TODO (Fase 2)
}

void Indice::buscaExata(long long poder, lib::vetor<int>& saida) {
    (void)poder; (void)saida;   // TODO (Fase 2)
}

void Indice::buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida) {
    (void)min; (void)max; (void)saida;   // TODO (Fase 2)
}

const std::string& Indice::getModo() const { return modo; }

int Indice::getTamanho() const { return tamanho; }
