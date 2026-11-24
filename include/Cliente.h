#ifndef CLIENTE_H
#define CLIENTE_H

#include "lib.h"
#include <string>

class Cliente {
private:
    int id;
    std::string nome;
    lib::vetor<long long> build;
    long long poder;
    bool ativo;

public:
    Cliente();
    Cliente(int id, const std::string& nome, const lib::vetor<long long>& build);

    int getId() const;
    const std::string& getNome() const;
    long long getPoder() const;
    long long getValor(int idAtributo) const;
    int getNumAtributos() const;

    bool isAtivo() const;
    void desativar();   // Encerrar atendimento (comando D)
};

#endif
