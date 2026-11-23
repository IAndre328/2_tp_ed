#ifndef MAPA_H
#define MAPA_H

#include "lib.h"
#include <string>

// Rota saindo de uma regiao: destino e custo de travessia.
struct Rota {
    int destino;
    long long custo;

    Rota() : destino(-1), custo(0) {}
    Rota(int d, long long c) : destino(d), custo(c) {}
};

// TAD Mapa
// Grafo nao direcionado e ponderado, representado por listas de adjacencia.
// Cada rota (u, v, c) e guardada nas duas listas (u -> v e v -> u).
//
// A conectividade do comando CC e respondida pelos Conjuntos Disjuntos.
// A busca em largura (alcancavelBFS) existe aqui para a comparacao
// experimental entre as duas formas de responder a mesma pergunta.
class Mapa {
private:
    lib::vetor<std::string> nomes;          // nomes[id] = nome da regiao
    lib::vetor<lib::vetor<Rota>*> adj;      // adj[id] = rotas saindo de id
    int numRotas;

    // buffers reaproveitados pela BFS (evita alocacao a cada consulta)
    lib::vetor<int> marca;      // marca[v] == rodadaBFS  => v ja visitado
    lib::vetor<int> filaBFS;    // fila circular nao e preciso: cada v entra 1 vez
    int rodadaBFS;

    // instrumentacao
    long long ultimasVisitadas;     // regioes visitadas na ultima BFS
    long long totalVisitadas;       // soma de todas as BFS
    long long numConsultasBFS;

    // copia proibida (o Mapa e dono dos ponteiros em adj)
    Mapa(const Mapa&);
    Mapa& operator=(const Mapa&);

public:
    Mapa();
    ~Mapa();

    // cadastra uma regiao e devolve seu id (crescente a partir de 0)
    int adicionarRegiao(const std::string& nome);

    // cria rota de mao dupla entre u e v com o custo dado
    void adicionarRota(int u, int v, long long custo);

    // busca em largura a partir de 'origem', parando ao achar 'destino'
    bool alcancavelBFS(int origem, int destino);

    const std::string& getNome(int id) const;
    int getNumRegioes() const;
    int getNumRotas() const;
    const lib::vetor<Rota>& getRotas(int id) const;

    // instrumentacao
    long long getUltimasVisitadas() const;
    long long getTotalVisitadas() const;
    long long getNumConsultasBFS() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif // MAPA_H
