#include "ArvoreAVL.h"

ArvoreAVL::ArvoreAVL() : raiz(nullptr), tamanho(0), numComparacoes(0), numRotacoes(0) {}

ArvoreAVL::~ArvoreAVL() { destruirRec(raiz); }

void ArvoreAVL::destruirRec(NoAVL* n) {
    if (n == nullptr) return;
    destruirRec(n->esq);
    destruirRec(n->dir);
    delete n;
}

// ---------------------------------------------------------------------
// Altura e balanceamento
// ---------------------------------------------------------------------

int ArvoreAVL::altura(const NoAVL* n) const {
    return (n == nullptr) ? 0 : n->altura;
}

// > 0: pende para a esquerda; < 0: pende para a direita
int ArvoreAVL::fatorBalanceamento(const NoAVL* n) const {
    return altura(n->esq) - altura(n->dir);
}

void ArvoreAVL::atualizarAltura(NoAVL* n) {
    int he = altura(n->esq);
    int hd = altura(n->dir);
    n->altura = 1 + (he > hd ? he : hd);
}

/*        y                x
         / \              / \
        x   C    ==>     A   y
       / \                  / \
      A   B                B   C        */
NoAVL* ArvoreAVL::rotacaoDireita(NoAVL* y) {
    NoAVL* x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    atualizarAltura(y);   // y agora e filho: atualiza primeiro
    atualizarAltura(x);
    numRotacoes++;
    return x;
}

/*      x                    y
       / \                  / \
      A   y      ==>       x   C
         / \              / \
        B   C            A   B          */
NoAVL* ArvoreAVL::rotacaoEsquerda(NoAVL* x) {
    NoAVL* y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    atualizarAltura(x);
    atualizarAltura(y);
    numRotacoes++;
    return y;
}

// Recalcula a altura de n e, se ele estiver desbalanceado (|fb| = 2),
// aplica a rotacao adequada. Devolve a nova raiz da subarvore.
NoAVL* ArvoreAVL::balancear(NoAVL* n) {
    atualizarAltura(n);
    int fb = fatorBalanceamento(n);

    if (fb > 1) {                                   // pesado a esquerda
        if (fatorBalanceamento(n->esq) < 0)         // caso esquerda-direita
            n->esq = rotacaoEsquerda(n->esq);
        return rotacaoDireita(n);                   // caso esquerda-esquerda
    }
    if (fb < -1) {                                  // pesado a direita
        if (fatorBalanceamento(n->dir) > 0)         // caso direita-esquerda
            n->dir = rotacaoDireita(n->dir);
        return rotacaoEsquerda(n);                  // caso direita-direita
    }
    return n;
}

int ArvoreAVL::comparar(long long poder, int id, const NoAVL* n) {
    numComparacoes++;
    if (poder != n->poder) return (poder < n->poder) ? -1 : 1;
    if (id != n->id) return (id < n->id) ? -1 : 1;
    return 0;
}

// ---------------------------------------------------------------------
// Insercao
// ---------------------------------------------------------------------

NoAVL* ArvoreAVL::inserirRec(NoAVL* n, long long poder, int id, bool& inseriu) {
    if (n == nullptr) {
        inseriu = true;
        return new NoAVL(poder, id);
    }
    int c = comparar(poder, id, n);
    if (c < 0)      n->esq = inserirRec(n->esq, poder, id, inseriu);
    else if (c > 0) n->dir = inserirRec(n->dir, poder, id, inseriu);
    else            return n;   // chave ja existe

    return balancear(n);        // na volta da recursao, rebalanceia o caminho
}

bool ArvoreAVL::inserir(long long poder, int id) {
    bool inseriu = false;
    raiz = inserirRec(raiz, poder, id, inseriu);
    if (inseriu) tamanho++;
    return inseriu;
}

// ---------------------------------------------------------------------
// Remocao
// ---------------------------------------------------------------------

// Tira o menor no da subarvore n (sem apaga-lo), devolvendo-o em 'minimo'.
// Devolve a nova raiz da subarvore, ja rebalanceada.
NoAVL* ArvoreAVL::desligarMinimo(NoAVL* n, NoAVL*& minimo) {
    if (n->esq == nullptr) {
        minimo = n;
        return n->dir;
    }
    n->esq = desligarMinimo(n->esq, minimo);
    return balancear(n);
}

NoAVL* ArvoreAVL::removerRec(NoAVL* n, long long poder, int id, bool& removeu) {
    if (n == nullptr) return nullptr;   // chave nao existe

    int c = comparar(poder, id, n);
    if (c < 0) {
        n->esq = removerRec(n->esq, poder, id, removeu);
    } else if (c > 0) {
        n->dir = removerRec(n->dir, poder, id, removeu);
    } else {
        removeu = true;

        // 0 ou 1 filho: o filho (ou nullptr) ocupa o lugar do no
        if (n->esq == nullptr || n->dir == nullptr) {
            NoAVL* filho = (n->esq != nullptr) ? n->esq : n->dir;
            delete n;
            return filho;
        }

        // 2 filhos: o sucessor (menor da subarvore direita) ocupa o lugar
        NoAVL* sucessor = nullptr;
        NoAVL* novaDir = desligarMinimo(n->dir, sucessor);
        sucessor->esq = n->esq;
        sucessor->dir = novaDir;
        delete n;
        n = sucessor;
    }
    return balancear(n);
}

bool ArvoreAVL::remover(long long poder, int id) {
    bool removeu = false;
    raiz = removerRec(raiz, poder, id, removeu);
    if (removeu) tamanho--;
    return removeu;
}

// ---------------------------------------------------------------------
// Buscas
// ---------------------------------------------------------------------

// Percurso em ordem com poda: so desce para a esquerda se ainda pode
// haver chave >= min, e so para a direita se ainda pode haver <= max.
void ArvoreAVL::faixaRec(NoAVL* n, long long min, long long max,
                         lib::vetor<EntradaIndice>& saida) {
    if (n == nullptr) return;
    numComparacoes++;

    if (n->poder >= min) faixaRec(n->esq, min, max, saida);
    if (n->poder >= min && n->poder <= max) saida.add_no_fim(EntradaIndice(n->poder, n->id));
    if (n->poder <= max) faixaRec(n->dir, min, max, saida);
}

void ArvoreAVL::buscaFaixa(long long min, long long max, lib::vetor<EntradaIndice>& saida) {
    faixaRec(raiz, min, max, saida);
}

void ArvoreAVL::buscaExata(long long poder, lib::vetor<int>& saida) {
    // BP e a faixa [p, p]; como a chave e (poder, id), sai em ordem de id
    lib::vetor<EntradaIndice> tmp;
    faixaRec(raiz, poder, poder, tmp);
    for (int i = 0; i < tmp.getTamanho(); i++) saida.add_no_fim(tmp.getById(i).idCliente);
}

void ArvoreAVL::listarTodos(lib::vetor<EntradaIndice>& saida) {
    if (raiz == nullptr) return;
    // pega o menor e o maior poder e faz uma busca por faixa entre eles
    NoAVL* menor = raiz;
    while (menor->esq != nullptr) menor = menor->esq;
    NoAVL* maior = raiz;
    while (maior->dir != nullptr) maior = maior->dir;
    faixaRec(raiz, menor->poder, maior->poder, saida);
}

// ---------------------------------------------------------------------
// Consultas e verificacao
// ---------------------------------------------------------------------

int ArvoreAVL::getTamanho() const { return tamanho; }

int ArvoreAVL::getAltura() const { return altura(raiz); }

// devolve a altura real da subarvore e marca ok = false se algo estiver errado
int ArvoreAVL::verificarRec(const NoAVL* n, bool& ok) const {
    if (n == nullptr) return 0;
    if (n->esq != nullptr && !(n->esq->poder < n->poder ||
        (n->esq->poder == n->poder && n->esq->id < n->id))) ok = false;
    if (n->dir != nullptr && !(n->dir->poder > n->poder ||
        (n->dir->poder == n->poder && n->dir->id > n->id))) ok = false;
    int he = verificarRec(n->esq, ok);
    int hd = verificarRec(n->dir, ok);
    int h = 1 + (he > hd ? he : hd);
    if (h != n->altura) ok = false;
    if (he - hd > 1 || hd - he > 1) ok = false;
    return h;
}

bool ArvoreAVL::verificar() const {
    bool ok = true;
    verificarRec(raiz, ok);
    return ok;
}

long long ArvoreAVL::getNumComparacoes() const { return numComparacoes; }
long long ArvoreAVL::getNumRotacoes() const { return numRotacoes; }

long long ArvoreAVL::getMemoriaBytes() const {
    return (long long)sizeof(*this) + (long long)tamanho * (long long)sizeof(NoAVL);
}

void ArvoreAVL::resetarEstatisticas() {
    numComparacoes = 0;
    numRotacoes = 0;
}
