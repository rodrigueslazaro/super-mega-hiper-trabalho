#include "Node.h"

template <class T, const unsigned int min_degree>
Node<T, min_degree>::Node() // criando um n� da �rvore
{
    this->leaf = true;
    this->n = 0;
    this->keys.resize(maxKeys);              // inicializar sem chaves e irmaos
    this->children.resize(maxChildrens, -1); //-1 para indicar que nao possui filhos
}

template <class T, const unsigned int min_degree>
Node<T, min_degree>::~Node()
{
}

template <class T, const unsigned int min_degree>
bool Node<T, min_degree>::operator==(const Node<T, min_degree> &other) const
{
    return (this->n == other.n) && (this->keys == other.keys) && (this->leaf == other.leaf) && (this->children == other.children);
}

template <class T, const unsigned int min_degree>
Node<T, min_degree> &Node<T, min_degree>::operator=(const Node<T, min_degree> &other)
{
    if (this == &other)
    {
        return *this;
    }
    this->n = other.n;
    this->keys = other.keys;
    this->leaf = other.leaf;
    this->children = other.children;
    return *this;
}

template <class T, const unsigned int min_degree>
string Node<T, min_degree>::toString()
{
    string repr = "";
    repr += string(reinterpret_cast<char *>(&this->leaf), sizeof(this->leaf));
    repr += string(reinterpret_cast<char *>(&this->n), sizeof(this->n));
    for (int i = 0; i < this->keys.size(); i++)
    {
        repr += this->keys[i].toString();
    }
    for (int i = 0; i < this->children.size(); i++)
    {
        repr += string(reinterpret_cast<char *>(&this->children[i]), sizeof(this->children[i]));
    }
    return repr;
}

template <class T, const unsigned int min_degree>
void Node<T, min_degree>::fromString(string repr)
{
    int pos = 0;
    repr.copy(reinterpret_cast<char *>(&this->leaf), sizeof(this->leaf), pos);
    pos += sizeof(this->leaf);
    repr.copy(reinterpret_cast<char *>(&this->n), sizeof(this->n), pos);
    pos += sizeof(this->n);
    for (int i = 0; i < this->keys.size(); i++)
    {
        string keyString = repr.substr(pos, this->keys[i].size());
        this->keys[i].fromString(keyString);
        pos += this->keys[i].size();
    }
    for (int i = 0; i < this->children.size(); i++)
    {
        repr.copy(reinterpret_cast<char *>(&this->children[i]), sizeof(this->children[i]), pos);
        pos += sizeof(this->children[i]);
    }
}

template <class T, const unsigned int min_degree>
unsigned long long int Node<T, min_degree>::size() const
{
    return sizeof(this->n) + sizeof(this->leaf) + (this->keys.size() * this->keys[0].size()) + (this->children.size() * sizeof(this->children[0]));
}
