#ifndef CLIENTE_H
#define CLIENTE_H

#include "lib.h"
#include <string>

// TAD Cliente
// Guarda identificador, nome, build e poder de um cliente.
//
// Decisoes:
// - valores e poder usam long long: a soma de muitos atributos pode
//   estourar int, e os valores podem ser negativos;
// - a build e um vetor indexado pelo id do atributo, com tamanho igual ao
//   numero de atributos cadastrados no momento do cadastro do cliente.
//   Atributos cadastrados depois valem zero (getValor trata isso);
// - o poder e calculado uma unica vez, no construtor, e nunca muda.
class Cliente {
private:
    int id;
    std::string nome;
    lib::vetor<long long> build;
    long long poder;
    bool ativo;

public:
    Cliente();
    // 'build' ja deve vir com um valor por atributo (zero se nao informado,
    // e com o ultimo valor caso o atributo tenha sido repetido na entrada).
    Cliente(int id, const std::string& nome, const lib::vetor<long long>& build);

    int getId() const;
    const std::string& getNome() const;
    long long getPoder() const;
    long long getValor(int idAtributo) const;
    int getNumAtributos() const;

    bool isAtivo() const;
    void desativar();   // usado no encerramento de atendimento (comando D)
};

#endif // CLIENTE_H
