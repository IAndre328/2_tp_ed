#ifndef INDICE_H
#define INDICE_H

#include "lib.h"
#include "ArvoreAVL.h"
#include "TabelaHash.h"
#include <string>

// TAD Indice
// Indexa os clientes ativos pelo poder, em modo A (AVL) ou H (Hash),
// permitindo migrar entre os modos sem alterar o estado logico.
//
// Decisoes de projeto (Fase 0):
// - chave composta (poder, idCliente), ambos tratados como long long/int;
//   na AVL a ordem (poder, id) ja e a ordem pedida pelo BF, e o BP vira
//   a faixa [p, p];
// - na Hash a chave e o poder; cada entrada guarda os ids daquele poder
//   em ordem crescente. Colisoes por encadeamento separado; espalhamento
//   multiplicativo (Knuth) sobre o poder convertido para unsigned long long
//   (trata poderes negativos); rehash quando fator de carga > 0,75;
// - BF na Hash: varre a tabela, filtra [min, max] e ordena por (poder, id)
//   com ordenacao propria (std::sort e proibido).

// Par (poder, id) devolvido pela busca por faixa.
struct EntradaIndice {
    long long poder;
    int idCliente;

    EntradaIndice() : poder(0), idCliente(-1) {}
    EntradaIndice(long long p, int id) : poder(p), idCliente(id) {}
};

class Indice {
private:
    std::string modo;   // "A" ou "H"; vazio antes do primeiro E
    int tamanho;        // numero de clientes ativos indexados

    // TODO (Fase 2): ArvoreAVL* arvore; TabelaHash* tabela;

    // copia proibida (o Indice sera dono das estruturas internas)
    Indice(const Indice&);
    Indice& operator=(const Indice&);

public:
    Indice();
    ~Indice();

    // troca a estrutura interna para "A" ou "H", preservando os clientes
    void migrarPara(const std::string& novoModo);

    void inserir(long long poder, int idCliente);
    void remover(long long poder, int idCliente);

    // ids com poder exatamente igual, em ordem crescente de id (comando BP)
    void buscaExata(long long poder, lib::vetor<int>& saida);

    // pares com poder em [min, max], ordenados por (poder, id) (comando BF)
    void buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida);

    const std::string& getModo() const;
    int getTamanho() const;
};

#endif // INDICE_H
