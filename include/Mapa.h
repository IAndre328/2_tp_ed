#ifndef MAPA_H
#define MAPA_H

#include "lib.h"
#include <string>


struct Rota {
    int destino;
    long long custo;

    Rota() : destino(-1), custo(0) {}
    Rota(int d, long long c) : destino(d), custo(c) {}
};


class Mapa {
private:
    lib::vetor<std::string> nomes;          
    lib::vetor<lib::vetor<Rota>*> adj;      
    int numRotas;

    // buffers reaproveitados pela BFS
    lib::vetor<int> marca;      
    lib::vetor<int> filaBFS;  
    int rodadaBFS;

    // Contadores BFS
    long long ultimasVisitadas;     // regioes visitadas na ultima BFS
    long long totalVisitadas;
    long long numConsultasBFS;

    Mapa(const Mapa&);
    Mapa& operator=(const Mapa&);

public:
    Mapa();
    ~Mapa();

    // cadastra uma regiao e devolve seu id
    int adicionarRegiao(const std::string& nome);

    // cria rota de mao dupla entre u e v com o custo dado
    void adicionarRota(int u, int v, long long custo);

    // busca em largura a partir de 'origem', parando ao achar 'destino'
    bool alcancavelBFS(int origem, int destino);

    const std::string& getNome(int id) const;
    int getNumRegioes() const;
    int getNumRotas() const;
    const lib::vetor<Rota>& getRotas(int id) const;

    // Contadores
    long long getUltimasVisitadas() const;
    long long getTotalVisitadas() const;
    long long getNumConsultasBFS() const;
    long long getMemoriaBytes() const;
    void resetarEstatisticas();
};

#endif
