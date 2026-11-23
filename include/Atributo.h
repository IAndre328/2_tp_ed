#ifndef ATRIBUTO_H
#define ATRIBUTO_H

#include "lib.h"
#include <string>

// TAD Atributo
// Guarda o identificador (atribuido em ordem crescente a partir de 0)
// e o nome de um atributo de build (ex.: Forca, Magia).
class Atributo {
private:
    int id;
    std::string nome;

public:
    Atributo();
    Atributo(int id, const std::string& nome);

    int getId() const;
    const std::string& getNome() const;
};

#endif // ATRIBUTO_H
