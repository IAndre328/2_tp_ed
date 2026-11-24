#ifndef INDICE_H
#define INDICE_H

#include "lib.h"
#include "EntradaIndice.h"
#include "ArvoreAVL.h"
#include "TabelaHash.h"
#include <string>

class Indice {
private:
    std::string modo;       // "A" ou "H"; vazio antes do primeiro E
    ArvoreAVL* arvore;     
    TabelaHash* tabela;    

    Indice(const Indice&);
    Indice& operator=(const Indice&);

public:
    Indice();
    ~Indice();

    void migrarPara(const std::string& novoModo);

    void inserir(long long poder, int idCliente);
    void remover(long long poder, int idCliente);

    // ids com poder exatamente igual, em ordem crescente de id (comando BP)
    void buscaExata(long long poder, lib::vetor<int>& saida);

    // pares com poder em [min, max], ordenados por (poder, id) (comando BF)
    void buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida);

    const std::string& getModo() const;
    int getTamanho() const;

    // acesso as estruturas internas para a analise experimental
    // (devolvem nullptr se a estrutura nao for a do modo atual)
    const ArvoreAVL* getArvore() const;
    const TabelaHash* getTabela() const;
};

#endif
