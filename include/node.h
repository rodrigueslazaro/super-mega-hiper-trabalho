#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#include <vector>

using namespace std;

template <class T, const unsigned int min_degree>
class node : public serializable
{
    static_assert(is_base_of<serializable, T>::value, "T must be serializable");
public:
    node();
    virtual ~node();

    void setN(unsigned int n){this->n = n;}
    unsigned int getN(){return this->n;}
    void setKeys(const T& newKey, const int index){this->keys[index] = newKey;}
    T& getKeys(int index) {return this->keys[index];}
    void setLeaf(const bool leaf){this->leaf = leaf;}
    bool& getLeaf(){return this->leaf;}
    void setChildren(const long long int newChildren, const int index){this->children[index] = newChildren;}
    long long int& getChildren(const int index) {return this->children[index];}

    bool operator==(const node<T, min_degree> &other)const;
    node<T, min_degree>& operator=(const node<T, min_degree> &other);

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
    static const unsigned int minChildrens = minKeys + 1 ;
public:
    unsigned int n;   //o número de chaves atualmente armazenadas no nó
    vector<T> keys;   //as próprias n chaves armazanadas de forma não decrescente
    bool leaf;        //VERDADEIRO se x é uma folha e FALSO se x é um nó interno.
    vector<long long int> children;     // n+1 ponteiros para os seus filhos (indíces  no arquivo)       (nós de folhas não têm filhos e, assim, seus atributos de children são indefinidos)
};

template <class T, const unsigned int min_degree>
node<T, min_degree>::node()      //criando um nó da árvore
{
    this->leaf = true;
    this->n = 0;
    this->keys.resize(maxKeys);           //inicializar sem chaves e irmaos
    this->children.resize(maxChildrens, -1);    //-1 para indicar que nao possui filhos
}

template <class T, const unsigned int min_degree>
node<T, min_degree>::~node()
{

}

template <class T, const unsigned int min_degree>
bool node<T, min_degree>::operator== (const node<T, min_degree> &other)const{
    return (this->n == other.n) && (this->keys == other.keys) && (this->leaf == other.leaf) && (this->children == other.children);
}

template <class T, const unsigned int min_degree>
node<T, min_degree>& node<T, min_degree>::operator=(const node<T, min_degree> &other){
    if(this == &other){
        return *this;
    }
    this->n = other.n;
    this->keys = other.keys;
    this->leaf = other.leaf;
    this->children = other.children;
    return *this;
}

template <class T, const unsigned int min_degree>
string node<T, min_degree>::toString()
{
    string repr = "";
    repr += string(reinterpret_cast<char*>(&this->leaf), sizeof(this->leaf));
    repr += string(reinterpret_cast<char*>(&this->n), sizeof(this->n));
    for(int i = 0; i < this->keys.size(); i++){
        repr += this->keys[i].toString();
    }
    for(int i = 0; i < this->children.size(); i++){
        repr += string(reinterpret_cast<char*>(&this->children[i]), sizeof(this->children[i]));
    }
    return repr;
}

template <class T, const unsigned int min_degree>
void node<T, min_degree>::fromString(string repr)
{
    int pos = 0;
    repr.copy(reinterpret_cast<char*>(&this->leaf), sizeof(this->leaf), pos);
    pos += sizeof(this->leaf);
    repr.copy(reinterpret_cast<char*>(&this->n),sizeof(this->n), pos);
    pos += sizeof(this->n);
    for(int i = 0; i < this->keys.size(); i++){
        string keyString = repr.substr(pos, this->keys[i].size());
        this->keys[i].fromString(keyString);
        pos += this->keys[i].size();
    }
    for(int i = 0; i < this->children.size(); i++){
        repr.copy(reinterpret_cast<char*>(&this->children[i]), sizeof(this->children[i]), pos);
        pos += sizeof(this->children[i]);
    }
}

template <class T, const unsigned int min_degree>
unsigned long long int node<T, min_degree>::size() const
{
    return sizeof(this->n) + sizeof(this->leaf) + (this->keys.size()*this->keys[0].size()) + (this->children.size()* sizeof(this->children[0]));
}

#endif // NODE_H_INCLUDED
