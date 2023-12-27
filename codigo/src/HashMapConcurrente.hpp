#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"
#include "Lightswitch.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static constexpr int cantLetras = 26;

    HashMapConcurrente();
    ~HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(unsigned int cantThreads);
    void maximoFila(std::vector<hashMapPair> &res, std::atomic<unsigned int> &currRow);

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    std::vector<std::mutex*> _roomEmpty;
    std::vector<std::mutex*> _turnstile;
    std::vector<Lightswitch*> _readSwitch;
    static unsigned int hashIndex(std::string clave);
};

#endif  /* HMC_HPP */
