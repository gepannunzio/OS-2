#ifndef HMC_ARCHIVOS_HPP
#define HMC_ARCHIVOS_HPP

#include <ctype.h>
#include <vector>
#include <string>

#include "HashMapConcurrente.hpp"
#include "ListaAtomica.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
);

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
);

// ---------------- Agregado por mí para testear la clase lista por separado ----------------

int cargarArchivoLista(
    ListaAtomica<std::string> &list,
    std::string filePath
);

void cargarMultiplesArchivosLista(
    ListaAtomica<std::string> &list,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
);

bool compararArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
);

bool compararMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
);

// ---------------- Agregado por mí para testear la clase lista por separado ----------------

#endif /* HMC_ARCHIVOS_HPP */
