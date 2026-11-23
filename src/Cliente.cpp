#include "Cliente.h"

Cliente::Cliente() : id(-1), nome(""), build(), poder(0), ativo(false) {}

Cliente::Cliente(int id, const std::string& nome, const lib::vetor<long long>& build)
    : id(id), nome(nome), build(build), poder(0), ativo(true) {
    // poder = soma dos valores de todos os atributos da build
    for (int a = 0; a < this->build.getTamanho(); a++) {
        poder += this->build.getById(a);
    }
}

int Cliente::getId() const { return id; }

const std::string& Cliente::getNome() const { return nome; }

long long Cliente::getPoder() const { return poder; }

long long Cliente::getValor(int idAtributo) const {
    // atributo cadastrado depois do cliente (ou invalido) vale zero
    if (idAtributo < 0 || idAtributo >= build.getTamanho()) return 0;
    return build.getById(idAtributo);
}

int Cliente::getNumAtributos() const { return build.getTamanho(); }

bool Cliente::isAtivo() const { return ativo; }

void Cliente::desativar() { ativo = false; }
