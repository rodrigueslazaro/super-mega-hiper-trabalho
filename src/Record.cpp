#include "Record.h"

template <class T, const unsigned int min_degree>
Record<T, min_degree>::Record() : Serializable()
{
    Node<T, min_degree> data;
    this->data = data;
    this->next = 0;
    this->deleted = false;
}

template <class T, const unsigned int min_degree>
Record<T, min_degree>::Record(Node<T, min_degree> d) : Serializable()
{
    this->data = d;
    this->deleted = false;
    this->next = 0;
}

template <class T, const unsigned int min_degree>
Record<T, min_degree>::Record(const Record<T, min_degree> &other)
{
    this->data = other.data;
    this->deleted = other.deleted;
    this->next = other.next;
}

template <class T, const unsigned int min_degree>
Record<T, min_degree>::~Record()
{
}

template <class T, const unsigned int min_degree>
Record<T, min_degree> &Record<T, min_degree>::operator=(const Record<T, min_degree> &other)
{
    if (this == &other)
    {
        return *this;
    }
    this->data = other.data;
    this->deleted = other.deleted;
    this->next = other.next;
    return *this;
}

template <class T, const unsigned int min_degree>
bool Record<T, min_degree>::operator==(const Record<T, min_degree> &other)
{
    return (this->data == other.data) && (this->deleted == other.deleted) && (this->next == other.next);
}

template <class T, const unsigned int min_degree>
Node<T, min_degree> Record<T, min_degree>::getData() const
{
    return this->data;
}

template <class T, const unsigned int min_degree>
void Record<T, min_degree>::setData(Node<T, min_degree> d)
{
    this->data = d;
}

template <class T, const unsigned int min_degree>
unsigned long long int Record<T, min_degree>::getNext() const
{
    return this->next;
}

template <class T, const unsigned int min_degree>
void Record<T, min_degree>::setNext(unsigned long long int n)
{
    this->next = n;
}

template <class T, const unsigned int min_degree>
bool Record<T, min_degree>::isDeleted() const
{
    return this->deleted;
}

template <class T, const unsigned int min_degree>
void Record<T, min_degree>::del()
{
    this->deleted = true;
}

template <class T, const unsigned int min_degree>
void Record<T, min_degree>::undel()
{
    this->deleted = false;
}

template <class T, const unsigned int min_degree>
string Record<T, min_degree>::toString()
{
    string aux = "";
    aux += this->data.toString();
    aux += string(reinterpret_cast<char *>(&this->deleted), sizeof(this->deleted));
    aux += string(reinterpret_cast<char *>(&this->next), sizeof(this->next));
    return aux;
}

template <class T, const unsigned int min_degree>
void Record<T, min_degree>::fromString(string repr)
{
    int pos = 0;
    string dataString = repr.substr(pos, this->data.size());
    this->data.fromString(dataString);
    pos += this->data.size();
    repr.copy(reinterpret_cast<char *>(&this->deleted), sizeof(this->deleted), pos);
    pos += sizeof(this->deleted);
    repr.copy(reinterpret_cast<char *>(&this->next), sizeof(this->next), pos);
}

template <class T, const unsigned int min_degree>
unsigned long long int Record<T, min_degree>::size() const
{
    return this->data.size() + sizeof(this->deleted) + sizeof(this->next);
}