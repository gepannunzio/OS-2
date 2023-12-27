#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <chrono>
using namespace std::chrono;

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"

int main(int argc, char **argv) {
/*
 Para correr los tests de rendimiento con el diccionatio ordenado o desordenado usar los
 archivos es data/smallDictionarySorted o data/smallDictionaryUnsorted. Siempre son la
 misma cantidad de archivos chiquitos.
 */
    HashMapConcurrente *hM = nullptr;
    std::vector<std::string> files;
    unsigned int minThreads = 1;
    unsigned int maxThreads = 26;
    unsigned int maximoReps = 2000;
    unsigned int loadReps = 10; // tarda mucho con los desordenados!!!!!
    std::vector<uint64_t> minTimeLoad;
    std::vector<uint64_t> minTimeMaximo;

    for (int i = 0; i <= 25; ++i)
        if (i < 10) files.push_back("data/smallDictionarySorted/dicc.split000" + std::to_string(i));
        else files.push_back("data/smallDictionarySorted/dicc.split00" + std::to_string(i));

    for (unsigned int i = minThreads; i <= maxThreads; ++i) {
        uint64_t currMin = UINT64_MAX;
        for (unsigned int rep = 1; rep <= loadReps; ++rep) {
            hM = new HashMapConcurrente;
            std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
            cargarMultiplesArchivos(*hM, i, files);
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            uint64_t currTime = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            if (currMin > currTime) currMin = currTime;
            delete hM;
        }
        minTimeLoad.emplace_back(currMin);
    }

    hM = new HashMapConcurrente;
    cargarMultiplesArchivos(*hM, 6, files);
    hM->incrementar("multiprogramming");

    for (unsigned int i = minThreads; i <= maxThreads; ++i) {
        std::cout << "Testeando " << i << " threads" << std::endl;
        uint64_t currMin = UINT64_MAX;
        for (unsigned int rep = 1; rep < maximoReps; ++rep) {
            std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
            hashMapPair actual = hM->maximoParalelo(i);
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            uint64_t currTime = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            if (currMin > currTime) currMin = currTime;
            assert(actual.first == "multiprogramming" && actual.second == 2);
        }
        minTimeMaximo.emplace_back(currMin);
    }
    delete hM;
/*
    Formato CSV: cant_threads, tiempo_minimo_maxima_palabra(us), tiempo_minimo_carga(us)
*/

    std::ofstream output_file;
    output_file.open("runtime_hdd_sorted.csv", std::ios::trunc);

    unsigned int currThread = minThreads;
    for (unsigned int i = 0; i < minTimeLoad.size(); ++i) {
        output_file << currThread << "," << minTimeMaximo[i] << "," << minTimeLoad[i] << "\n";
        currThread++;
    }
    output_file.close();

    return 0;
}
