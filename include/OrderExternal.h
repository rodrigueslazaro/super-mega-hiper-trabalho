#ifndef ORDEREXTERNAL_H
#define ORDEREXTERNAL_H
#include <fstream>
#include <random>
#include <array>

using namespace std;

template <class T, const unsigned int min_degree>
class OrderExternal : public TypedFile<T, min_degree>
{
    public:
        OrderExternal(const string name, const string type, const unsigned int ver);
        virtual ~OrderExternal(){};
        void genRandomInput(const string name);
        void orderExternal();
    private:

        array<T, min_degree> memory;
        array<T, min_degree> tapes;
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

#endif // ORDEREXTERNAL_H


