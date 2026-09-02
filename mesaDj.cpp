#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <string>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

std::mutex mtx;
std::mutex mci_mtx;
std::mutex out_mtx;

struct Instrumento {
    std::string nome;
    std::string arquivo;
    std::string alias;
    bool tocando = true;
    bool encerrar = false;
};

void log(const std::string& mensagem) {
    std::lock_guard<std::mutex> lock(out_mtx);
    std::cout << mensagem << std::endl;
}

void checarErroMci(MCIERROR erro) {
    if (erro != 0) {
        char textoErro[256];
        mciGetErrorStringA(erro, textoErro, sizeof(textoErro));
        log(std::string("[ERRO MCI] ") + textoErro);
    }
}

MCIERROR enviarMci(const std::string& comando) {
    std::lock_guard<std::mutex> lock(mci_mtx);
    return mciSendStringA(comando.c_str(), NULL, 0, NULL);
}

void tocar(Instrumento& inst) {
    checarErroMci(enviarMci("open \"" + inst.arquivo + "\" type mpegvideo alias " + inst.alias));
    checarErroMci(enviarMci("play " + inst.alias + " repeat"));

    bool estadoAnterior = inst.tocando;

    while (true) {
        mtx.lock();
        bool estadoAtual = inst.tocando;
        bool deveEncerrar = inst.encerrar;
        mtx.unlock();

        if (deveEncerrar) break;

        if (estadoAtual != estadoAnterior) {
            if (estadoAtual) {
                enviarMci("setaudio " + inst.alias + " volume to 1000");
                log("  [" + inst.nome + "] retomado");
            } else {
                enviarMci("setaudio " + inst.alias + " volume to 0");
                log("  [" + inst.nome + "] pausado");
            }
            estadoAnterior = estadoAtual;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    enviarMci("close " + inst.alias);
}

int main() {
    std::vector<Instrumento> instrumentos = {
        {"vocal",    "Vocal.wav",    "vocSom", true, false},
        {"bateria",  "Bateria.wav",  "batSom", true, false},
        {"piano",    "Piano.wav",    "piaSom", true, false},
        {"guitarra", "Guitarra.wav", "guiSom", true, false}
    };

    std::vector<std::thread> threads_instrumentos;
    for (auto& inst : instrumentos) {
        threads_instrumentos.emplace_back(tocar, std::ref(inst));
    }

    log("========================================");
    log(" MESA DE DJ");
    log("========================================");
    log(" Instrumentos: vocal, bateria, piano, guitarra");
    log(" Comandos:");
    log("   tocar <nome>");
    log("   pausar <nome>");
    log("   capela   (deixa so o vocal tocando)");
    log("   sair");
    log("========================================");

    std::string comando;
    while (std::cin >> comando) {
        if (comando == "sair") {
            break;
        }
        else if (comando == "pausar" || comando == "tocar") {
            std::string alvo;
            std::cin >> alvo;

            bool encontrado = false;

            mtx.lock();
            for (auto& inst : instrumentos) {
                if (inst.nome == alvo) {
                    inst.tocando = (comando == "tocar");
                    encontrado = true;
                    break;
                }
            }
            mtx.unlock();

            if (!encontrado) {
                log("  Instrumento nao encontrado: " + alvo);
            }
        }
        else if (comando == "capela") {
            mtx.lock();
            for (auto& inst : instrumentos) {
                inst.tocando = (inst.nome == "vocal");
            }
            mtx.unlock();
            log("  Modo a capela ativado (so o vocal tocando)");
        }
        else {
            log("  Comando invalido.");
        }
    }

    mtx.lock();
    for (auto& inst : instrumentos) {
        inst.encerrar = true;
    }
    mtx.unlock();

    for (auto& t : threads_instrumentos) {
        if (t.joinable()) {
            t.join();
        }
    }

    log("Encerrado.");
    return 0;
}