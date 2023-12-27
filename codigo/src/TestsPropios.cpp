#include <vector>
#include <algorithm>
#include <fstream>
#include "lib/littletest.hpp"

#include "../src/ListaAtomica.hpp"
#include "../src/HashMapConcurrente.hpp"
#include "../src/CargarArchivos.hpp"

LT_BEGIN_SUITE(TestsPropios)
HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsPropios)

LT_BEGIN_TEST(TestsPropios, CargarMultiplesArchivosOrdenadosDoceThreads)
    std::vector<std::string> files;
    HashMapConcurrente hM;
    for (int i = 0; i <= 25; ++i)
        if (i < 10) files.push_back("data/smallDictionarySorted/dicc.split000" + std::to_string(i));
        else files.push_back("data/smallDictionarySorted/dicc.split00" + std::to_string(i));
    std::cout << "Iniciando carga de archivos (en paralelo)" << std::endl;
    cargarMultiplesArchivos(hM, 12, files);
    std::cout << "Iniciando comparación de archivos (en paralelo)" << std::endl;
    LT_CHECK_EQ(compararMultiplesArchivos(hM, 12, files), true);
    hM.incrementar("multiprogramming");
    hashMapPair actual = hM.maximoParalelo(12);
    LT_CHECK_EQ(actual.first, "multiprogramming");
    LT_CHECK_EQ(actual.second, 2);
LT_END_TEST(CargarMultiplesArchivosOrdenadosDoceThreads)

LT_BEGIN_TEST(TestsPropios, CargarMultiplesArchivosDesordenadosDoceThreads)
    std::vector<std::string> files;
    HashMapConcurrente hM;
    for (int i = 0; i <= 25; ++i)
        if (i < 10) files.push_back("data/smallDictionaryUnsorted/dicc.split000" + std::to_string(i));
        else files.push_back("data/smallDictionaryUnsorted/dicc.split00" + std::to_string(i));
    std::cout << "Iniciando carga de archivos (en paralelo)" << std::endl;
    cargarMultiplesArchivos(hM, 12, files);
    std::cout << "Iniciando comparación de archivos (en paralelo)" << std::endl;
    LT_CHECK_EQ(compararMultiplesArchivos(hM, 12, files), true);
    hM.incrementar("multiprogramming");
    hashMapPair actual = hM.maximoParalelo(12);
    LT_CHECK_EQ(actual.first, "multiprogramming");
    LT_CHECK_EQ(actual.second, 2);
LT_END_TEST(CargarMultiplesArchivosDesordenadosDoceThreads)

LT_BEGIN_TEST(TestsPropios, CargarMultiplesArchivosListaThreads)
    ListaAtomica<std::string> list;
    std::vector<std::string> files;
    unsigned int wordCount = 0;
    for (int i = 0; i <= 12; ++i)
        if (i < 10) files.push_back("data/diccionarioInglesEspañol/diccEspaIng.split000" + std::to_string(i));
        else files.push_back("data/diccionarioInglesEspañol/diccEspaIng.split00" + std::to_string(i));
    std::cout << "Iniciando carga de archivos (en paralelo)" << std::endl;
    // cargo 50 veces estos archivos en la lista (es una lista gigante)
    for (int i = 1; i <= 50; i++)
        cargarMultiplesArchivosLista(list, files.size(), files);
    std::fstream file;
    file.open("data/diccionarioInglesEspañol/diccEspaIng", file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo durante el test" << std::endl;
    }
    std::cout << "Iniciando comparación de palabras" << std::endl;
    std::string palabraActual;
    while (file >> palabraActual)
        wordCount++;
    file.close();

    // chequeo la cantidad de elementos, no el contenido. Total si la cabeza se reemplazó bien
    // el conendido seguro está en cada nodo.
    unsigned int listLen = 0;
    auto it = list.begin();
    while (it != list.end()) {
        listLen++;
        ++it;
    }

    LT_CHECK_EQ(listLen, wordCount*50)

LT_END_TEST(CargarMultiplesArchivosListaThreads)

// Ejecutar tests
LT_BEGIN_AUTO_TEST_ENV()
    AUTORUN_TESTS()
LT_END_AUTO_TEST_ENV()
