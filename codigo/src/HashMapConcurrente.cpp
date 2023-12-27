#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"
#include "Lightswitch.hpp"

HashMapConcurrente::HashMapConcurrente() {
    _roomEmpty.resize(HashMapConcurrente::cantLetras);
    _turnstile.resize(HashMapConcurrente::cantLetras);
    _readSwitch.resize(HashMapConcurrente::cantLetras);
    
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
        _roomEmpty[i] = new std::mutex();
        _turnstile[i] = new std::mutex();
        _readSwitch[i] = new Lightswitch();
    }
}

HashMapConcurrente::~HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        delete tabla[i];
        delete _roomEmpty[i];
        delete _turnstile[i];
        delete _readSwitch[i];
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    bool found = false;
    std::unique_lock<std::mutex> turnstileLock(*_turnstile[hashIndex(clave)]);
    std::unique_lock<std::mutex> roomLock(*_roomEmpty[hashIndex(clave)]);
    for (auto &p : *tabla[hashIndex(clave)]) {
        if (p.first == clave) {
            found = true;
            p.second++;
        }
    }
    if (!found) {
        tabla[hashIndex(clave)]->insertar(std::make_pair(clave, 1));
    }
    turnstileLock.unlock();
    roomLock.unlock();

}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> res;
    for (unsigned int i = 0; i < cantLetras; ++i) {
        std::unique_lock<std::mutex> turnstileLock(*_turnstile[i]);
        turnstileLock.unlock();

        _readSwitch[i]->lock(*_roomEmpty[i]);
        for (auto &p : *tabla[i]) {
            res.emplace_back(p.first);
        }
        _readSwitch[i]->unlock(*_roomEmpty[i]);
    }
    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    unsigned int res = 0;
    bool notFoud = true;
    for (unsigned int i = 0; i < cantLetras && notFoud; ++i) {
        std::unique_lock<std::mutex> turnstileLock(*_turnstile[i]);
        turnstileLock.unlock();

        _readSwitch[i]->lock(*_roomEmpty[i]);
        for (auto &p : *tabla[i]) {
            if (p.first == clave) {
                res = p.second;
                notFoud = false;
                break;
            }
        }
        _readSwitch[i]->unlock(*_roomEmpty[i]);
    }
    return res;
}

hashMapPair HashMapConcurrente::maximo() {
    // nota: esto capaz falle con valgrind (sol de cátedra).
    // hashMapPair *max = new hashMapPair();
    // ya fue, lo cambiamos
    hashMapPair max;
    max.second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        std::unique_lock<std::mutex> turnstileLock(*_turnstile[index]);
        turnstileLock.unlock();

        _readSwitch[index]->lock(*_roomEmpty[index]);
        for (auto &p : *tabla[index]) {
            if (p.second > max.second) {
                max.first = p.first;
                max.second = p.second;
            }
        }
        _readSwitch[index]->unlock(*_roomEmpty[index]);
    }

    return max;
}

void HashMapConcurrente::maximoFila(std::vector<hashMapPair> &maxRow, std::atomic<unsigned int> &currRow) {
    hashMapPair max;
    max.second = 0;
    unsigned int index;
    while ((index = currRow.fetch_add(1)) < HashMapConcurrente::cantLetras) {
        std::unique_lock<std::mutex> turnstileLock(*_turnstile[index]);
        turnstileLock.unlock();

        _readSwitch[index]->lock(*_roomEmpty[index]);
        for (auto &p : *tabla[index]) {
            if (p.second > max.second) {
                max.first = p.first;
                max.second = p.second;
            }
        }
        _readSwitch[index]->unlock(*_roomEmpty[index]);
        maxRow[index] = max;
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    std::atomic<unsigned int> currRow(0);
    std::vector<hashMapPair> maxRow(HashMapConcurrente::cantLetras);
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < cant_threads; ++i)
        threads.emplace_back(&HashMapConcurrente::maximoFila, this, std::ref(maxRow), std::ref(currRow));
    for (unsigned int i = 0; i < cant_threads; ++i)
        threads[i].join();
    hashMapPair res = maxRow[0];
    for (auto &e : maxRow)
        res = e.second > res.second ? e : res;
    return res;
}

#endif
