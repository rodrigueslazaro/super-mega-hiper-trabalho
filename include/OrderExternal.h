#ifndef ORDEREXTERNAL_H
#define ORDEREXTERNAL_H
#include <fstream>
#include <random>
#include <array>
#include "TypedFile.h"

using namespace std;

template <class T, const unsigned int min_degree>
class OrderExternal : public TypedFile<T, min_degree>
{
    public:
        OrderExternal(const string name, const string type, const unsigned int ver);
        virtual ~OrderExternal(){};
        void genRandomInput(const string name);
        void orderExternal(const string filename);
    private:
        bool readIntoMemory(ifstream &file);
        void memoryToTapes();

        array<T, min_degree> memory;
        const unsigned int MEMORY_SIZE = min_degree;
};

template <class T, const unsigned int min_degree>
OrderExternal<T, min_degree>::OrderExternal(const string name, const string type, const unsigned int ver) : TypedFile<T,min_degree>(name, type, ver)
{

}

template <class T, const unsigned int min_degree>
void OrderExternal<T, min_degree>::genRandomInput(const string name)
{
    ofstream outFile(name);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 3000);

    for (int i = 0; i < 3000; ++i) {
        outFile << dis(gen) << "\n";
    }
}

template <class T, const unsigned int min_degree>
void OrderExternal<T, min_degree>::orderExternal(const string filename)
{
    ifstream file(filename);
    if (!file) {
        cerr << "Unable to open file.\n";
        return;
    }

    bool finished = false;

    while(!finished) {
        memoryToTapes();
        finished = readIntoMemory(file);
    }

    file.close();
}

template <class T, const unsigned int min_degree>
void OrderExternal<T, min_degree>::memoryToTapes()
{
    // processa os numeros lidos
    for (int i = 0; i < MEMORY_SIZE; ++i)
        if (memory[i] != NULL)
            cout << memory[i].getValue() << " ";
    cout << "\n";
}

template <class T, const unsigned int min_degree>
bool OrderExternal<T, min_degree>::readIntoMemory(ifstream &file)
{
    // numero de itens lidos para a memoria
    int count = 0;
    int value = 0;
    for (int i = 0; i < MEMORY_SIZE && file >> value; ++i) {
        memory[i].setValue(value);
        ++count;
    }
    // se nao leu nada ou leu menos do que cabia na memoria, termine
    if (count == 0 || count < MEMORY_SIZE)
        return true;
    return false;
}

#endif // ORDEREXTERNAL_H
