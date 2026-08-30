#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>

std::mutex mtx;

struct Instrumento
{
    std::string nome;
    bool tocando = true;

    void status(){
        std::cout << nome << ": " << (tocando ? "tocando" : "pausado") << std::endl;
    }
};

void tocar(Instrumento& inst){
    while (true){
        mtx.lock();
        bool estaTocando = inst.tocando;
        std::string nome = inst.nome;
        mtx.unlock();

        if (estaTocando){
            mtx.lock();
            std::cout << "tocando: " << nome << std::endl;
            mtx.unlock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main(){
    std::vector<Instrumento> instrumentos;
    instrumentos.push_back(Instrumento{"bateria", true});
    instrumentos.push_back(Instrumento{"baixo", true});

    std::vector<std::thread> threads;
    for (int i = 0; i < instrumentos.size(); i++){
        threads.push_back(std::thread(tocar, std::ref(instrumentos[i])));
    }

    std::cout << "Comandos: pausar <nome> | tocar <nome> | sair" << std::endl;

    std::string comando;
    while (std::cin >> comando){
        if (comando == "pausar" || comando == "tocar"){
            std::string alvo;
            std::cin >> alvo;

            mtx.lock();
            bool encontrado = false;
            for (int i = 0; i < instrumentos.size(); i++){
                if (instrumentos[i].nome == alvo){
                    instrumentos[i].tocando = (comando == "tocar");
                    encontrado = true;
                    break;
                }
            }
            mtx.unlock();

            if (!encontrado){
                std::cout << "Instrumento nao encontrado: " << alvo << std::endl;
            }
        }
        else if (comando == "sair"){
            break;
        }
        else {
            std::cout << "Comando invalido." << std::endl;
        }
    }
    std::exit(0);
}
