#include "lib.h"
#include "Atributo.h"
#include "Cliente.h"
#include "FilaPrioridade.h"
#include "ConjuntosDisjuntos.h"
#include "Mapa.h"
#include "Indice.h"

#include <iostream>
#include <string>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string cmd;
    std::string modo;

    lib::vetor<Atributo> atributos;
    lib::vetor<Cliente*> clientes;     // ponteiros: evita copiar builds ao crescer
    Indice indice;
    Mapa mapa;
    ConjuntosDisjuntos conjuntos;
    FilaPrioridade fila;

    while (std::cin >> cmd){

        if (cmd == "E"){
            std::cin >> modo;
            std::cout << "E " << modo << "\n";

            indice.migrarPara(modo);
        }
        else if (cmd == "A"){
            std::string nome;
            std::cin >> nome;

            Atributo temp(atributos.getTamanho(), nome);
            atributos.add_no_fim(temp);

            std::cout << "A " << temp.getId() << "\n";
        }
        else if (cmd == "C"){
            std::string nome;
            int k;
            std::cin >> nome;
            std::cin >> k;

            // build com um valor por atributo; nao informado vale zero
            lib::vetor<long long> build;
            for (int i = 0; i < atributos.getTamanho(); i++) build.add_no_fim(0);

            for (int i = 0; i < k; i++){
                int id_atributo;
                long long valor;
                std::cin >> id_atributo;
                std::cin >> valor;
                build.getById(id_atributo) = valor;   // repetido: o ultimo vale
            }

            Cliente* temp = new Cliente(clientes.getTamanho(), nome, build);
            clientes.add_no_fim(temp);
            indice.inserir(temp->getPoder(), temp->getId());

            std::cout << "C " << temp->getId() << " " << temp->getPoder() << "\n";
        }
        else if (cmd == "D"){
            int id_cliente;
            std::cin >> id_cliente;

            Cliente* t_cli = clientes.getById(id_cliente);
            indice.remover(t_cli->getPoder(), t_cli->getId());
            t_cli->desativar();

            std::cout << "D " << t_cli->getNome() << " " << t_cli->getPoder() << "\n";
        }
        else if (cmd == "BP"){
            long long poder;
            std::cin >> poder;

            lib::vetor<int> saida;
            indice.buscaExata(poder, saida);

            std::cout << "BP " << poder;
            if (saida.isEmpty()) std::cout << " VAZIO";
            for (int i = 0; i < saida.getTamanho(); i++){
                std::cout << " cliente_" << i + 1
                    << " " << saida.getById(i);
            }
            std::cout << "\n";
        }
        else if (cmd == "BF"){
            long long min, max;
            std::cin >> min;
            std::cin >> max;

            lib::vetor<EntradaIndice> saida;
            indice.buscaFaixa(min, max, saida);

            std::cout << "BF " << min << " " << max;
            if (saida.isEmpty()) std::cout << " VAZIO";
            for (int i = 0; i < saida.getTamanho(); i++){
                std::cout << " cliente_" << i + 1
                    << " " << saida.getById(i).idCliente
                    << " " << saida.getById(i).poder;
            }
            std::cout << "\n";
        }
        else if (cmd == "R"){
            std::string nome;
            std::cin >> nome;

            int id = mapa.adicionarRegiao(nome);
            conjuntos.criar();   // mesmo id no mapa e nos conjuntos

            std::cout << "R " << id << "\n";
        }
        else if (cmd == "T"){
            int id_regiao_1, id_regiao_2;
            long long custo;
            std::cin >> id_regiao_1;
            std::cin >> id_regiao_2;
            std::cin >> custo;

            mapa.adicionarRota(id_regiao_1, id_regiao_2, custo);
            conjuntos.unir(id_regiao_1, id_regiao_2);

            std::cout << "T " << mapa.getNome(id_regiao_1) << " "
                << mapa.getNome(id_regiao_2) << " "
                << custo << "\n";
        }
        else if (cmd == "CC"){
            int id_regiao_1, id_regiao_2;
            std::cin >> id_regiao_1;
            std::cin >> id_regiao_2;

            std::cout << "CC " << mapa.getNome(id_regiao_1) << " "
                << mapa.getNome(id_regiao_2) << " "
                << (conjuntos.conectados(id_regiao_1, id_regiao_2) ? "SIM" : "NAO") << "\n";
        }
        else if (cmd == "F"){
            int id_cliente;
            long long prioridade;
            std::cin >> id_cliente;
            std::cin >> prioridade;

            fila.inserir(id_cliente, prioridade);
            std::cout << "F " << fila.getTamanho() << "\n";
        }
        else if (cmd == "Z"){
            int turnos;
            std::cin >> turnos;

            if (turnos > fila.getTamanho()) turnos = fila.getTamanho();
            std::cout << "Z " << turnos << "\n";

            if (turnos == 0) continue;

            for (int i = 0; i < turnos; i++){
                ItemFila atual = fila.removerMax();

                if (i > 0) std::cout << " ";
                std::cout << "atendimento_" << i + 1
                    << " " << atual.idCliente
                    << " " << atual.prioridade;
            }
            std::cout << "\n";
        }
    }

    for (int i = 0; i < clientes.getTamanho(); i++) delete clientes.getById(i);

    return 0;
}
