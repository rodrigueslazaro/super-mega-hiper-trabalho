#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#include <vector>

using namespace std;

template <class T, const unsigned int min_degree>
class Node : public Serializable
{
    static_assert(is_base_of<Serializable, T>::value, "T must be serializable");

public:
    Node();
    virtual ~Node();

    void setN(unsigned int n) { this->n = n; }
    unsigned int getN() { return this->n; }
    void setKeys(const T &newKey, const int index) { this->keys[index] = newKey; }
    T &getKeys(int index) { return this->keys[index]; }
    void setLeaf(const bool leaf) { this->leaf = leaf; }
    bool &getLeaf() { return this->leaf; }
    void setChildren(const long long int newChildren, const int index) { this->children[index] = newChildren; }
    long long int &getChildren(const int index) { return this->children[index]; }

    bool operator==(const Node<T, min_degree> &other) const;
    Node<T, min_degree> &operator=(const Node<T, min_degree> &other);

    virtual string toString();
    virtual void fromString(string repr);
    virtual string toCSV()
    {
        return "";
    }
    virtual void fromCSV(string repr) {}
    virtual string toJSON()
    {
        return "";
    }
    virtual void fromJSON(string repr) {}
    virtual string toXML()
    {
        return "";
    }
    virtual void fromXML(string repr) {}
    virtual unsigned long long int size() const;

    static const unsigned int maxKeys = (2 * min_degree) - 1;
    static const unsigned int minKeys = min_degree - 1;
    static const unsigned int maxChildrens = maxKeys + 1;
    static const unsigned int minChildrens = minKeys + 1;

public:
    unsigned int n;                 // o n�mero de chaves atualmente armazenadas no n�
    vector<T> keys;                 // as pr�prias n chaves armazanadas de forma n�o decrescente
    bool leaf;                      // VERDADEIRO se x � uma folha e FALSO se x � um n� interno.
    vector<long long int> children; // n+1 ponteiros para os seus filhos (ind�ces  no arquivo)       (n�s de folhas n�o t�m filhos e, assim, seus atributos de children s�o indefinidos)
};

#endif // NODE_H_INCLUDED