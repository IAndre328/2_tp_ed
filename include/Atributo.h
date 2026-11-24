#ifndef ATRIBUTO_H
#define ATRIBUTO_H

#include "lib.h"
#include <string>

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

#endif
