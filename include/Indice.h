#ifndef INDICE_H
#define INDICE_H

#include "lib.h"
#include "EntradaIndice.h"
#include "ArvoreAVL.h"
#include "TabelaHash.h"
#include <string>

// TAD Indice
// Indexa os clientes ativos pelo poder, em modo A (AVL) ou H (Hash),
// permitindo migrar entre os modos sem alterar o estado logico.
//
// Em cada momento existe exatamente uma estrutura interna (a do modo
// atual); a outra fica nula. O main so conversa com o Indice, sem saber
// qual estrutura esta por baixo.
//
// - Modo A: chave (poder, id); a ordem da arvore ja e a ordem do BF,
//   e o BP e a faixa [p, p].
// - Modo H: chave = poder; cada entrada guarda os ids daquele poder.
//   Detalhes da hash em TabelaHash.h.
class Indice {
private:
    std::string modo;       // "A" ou "H"; vazio antes do primeiro E
    ArvoreAVL* arvore;      // != nullptr so no modo A
    TabelaHash* tabela;     // != nullptr so no modo H

    // copia proibida (o Indice e dono das estruturas internas)
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

    // acesso as estruturas internas para a analise experimental
    // (devolvem nullptr se a estrutura nao for a do modo atual)
    const ArvoreAVL* getArvore() const;
    const TabelaHash* getTabela() const;
};

#endif // INDICE_H
