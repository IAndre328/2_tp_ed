#include "Mapa.h"

Mapa::Mapa()
    : nomes(), adj(), numRotas(0), marca(), filaBFS(), rodadaBFS(0),
      ultimasVisitadas(0), totalVisitadas(0), numConsultasBFS(0) {}

Mapa::~Mapa() {
    for (int i = 0; i < adj.getTamanho(); i++) {
        delete adj.getById(i);
    }
}

int Mapa::adicionarRegiao(const std::string& nome) {
    int id = nomes.getTamanho();
    nomes.add_no_fim(nome);
    adj.add_no_fim(new lib::vetor<Rota>());
    marca.add_no_fim(0);
    filaBFS.add_no_fim(0);   // a fila da BFS nunca passa de numRegioes itens
    return id;
}

void Mapa::adicionarRota(int u, int v, long long custo) {
    adj.getById(u)->add_no_fim(Rota(v, custo));
    adj.getById(v)->add_no_fim(Rota(u, custo));
    numRotas++;
}

bool Mapa::alcancavelBFS(int origem, int destino) {
    numConsultasBFS++;
    rodadaBFS++;              // marcas de rodadas anteriores ficam invalidas
    ultimasVisitadas = 0;

    int inicio = 0, fim = 0;
    filaBFS.getById(fim++) = origem;
    marca.getById(origem) = rodadaBFS;

    bool achou = false;
    while (inicio < fim) {
        int u = filaBFS.getById(inicio++);
        ultimasVisitadas++;
        if (u == destino) { achou = true; break; }

        const lib::vetor<Rota>& rotas = *adj.getById(u);
        for (int i = 0; i < rotas.getTamanho(); i++) {
            int v = rotas.getById(i).destino;
            if (marca.getById(v) != rodadaBFS) {
                marca.getById(v) = rodadaBFS;
                filaBFS.getById(fim++) = v;
            }
        }
    }

    totalVisitadas += ultimasVisitadas;
    return achou;
}

const std::string& Mapa::getNome(int id) const { return nomes.getById(id); }

int Mapa::getNumRegioes() const { return nomes.getTamanho(); }

int Mapa::getNumRotas() const { return numRotas; }

const lib::vetor<Rota>& Mapa::getRotas(int id) const { return *adj.getById(id); }

long long Mapa::getUltimasVisitadas() const { return ultimasVisitadas; }
long long Mapa::getTotalVisitadas() const { return totalVisitadas; }
long long Mapa::getNumConsultasBFS() const { return numConsultasBFS; }

long long Mapa::getMemoriaBytes() const {
    long long n = nomes.getTamanho();
    long long total = (long long)sizeof(*this);
    total += n * (long long)(sizeof(std::string) + sizeof(lib::vetor<Rota>*)
                             + sizeof(lib::vetor<Rota>) + 2 * sizeof(int));
    total += 2LL * numRotas * (long long)sizeof(Rota);   // cada rota aparece 2x
    return total;
}

void Mapa::resetarEstatisticas() {
    ultimasVisitadas = 0;
    totalVisitadas = 0;
    numConsultasBFS = 0;
}
