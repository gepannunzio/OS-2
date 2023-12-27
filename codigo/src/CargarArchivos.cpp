#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <thread>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void threadCargaArchivo(const std::vector<std::string> &filePaths, HashMapConcurrente &hashMap, std::atomic<unsigned int> &currFile) {
    unsigned int index;
    while ((index = currFile.fetch_add(1)) < filePaths.size())
        cargarArchivo(hashMap, filePaths[index]);
}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    std::atomic<unsigned int> currFile(0);
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < cantThreads; ++i)
        threads.emplace_back(threadCargaArchivo, std::ref(filePaths), std::ref(hashMap), std::ref(currFile));
    for (unsigned int i = 0; i < cantThreads; ++i)
        threads[i].join();
}

// ---------------- Agregado por mí para testear la clase lista por separado ----------------

int cargarArchivoLista(
    ListaAtomica<std::string> &list,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        list.insertar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void threadCargaArchivoLista(const std::vector<std::string> &filePaths, ListaAtomica<std::string> &list, std::atomic<unsigned int> &currFile) {
    unsigned int index;
    while ((index = currFile.fetch_add(1)) < filePaths.size())
        cargarArchivoLista(list, filePaths[index]);
}

void cargarMultiplesArchivosLista(
    ListaAtomica<std::string> &list,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    std::atomic<unsigned int> currFile(0);
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < cantThreads; ++i)
        threads.emplace_back(threadCargaArchivoLista, std::ref(filePaths), std::ref(list), std::ref(currFile));
    for (unsigned int i = 0; i < cantThreads; ++i)
        threads[i].join();
}


bool compararArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int res = true;
    std::string palabraActual;
    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        res = res && (hashMap.valor(palabraActual) == 1);
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return false;
    }
    file.close();
    return res;
}

void threadComparaArchivo(const std::vector<std::string> &filePaths, HashMapConcurrente &hashMap, std::atomic<unsigned int> &currFile, std::vector<bool> &res, unsigned int threadNum) {
    unsigned int index;
    while ((index = currFile.fetch_add(1)) < filePaths.size())
        res[threadNum] = res[threadNum] && compararArchivo(hashMap, filePaths[index]);
}

bool compararMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    std::atomic<unsigned int> currFile(0);
    std::vector<std::thread> threads;
    std::vector<bool> res(cantThreads, true);
    for (unsigned int i = 0; i < cantThreads; ++i)
        threads.emplace_back(threadComparaArchivo, std::ref(filePaths), std::ref(hashMap), std::ref(currFile), std::ref(res), i);
    for (unsigned int i = 0; i < cantThreads; ++i)
        threads[i].join();

    bool finalRes = true;
    for (bool e : res) finalRes = finalRes && e;
    return finalRes;
}

// ---------------- Agregado por mí para testear la clase lista por separado ----------------

#endif
