#ifndef RECORD_H
#define RECORD_H

#include <string>
#include "serializable.h"
#include "node.h"

using namespace std;

template <class T, const unsigned int min_degree>
class record : public serializable
{
    static_assert(is_base_of<serializable, T>::value, "T must be serializable");
public:
    record();
    record(node<T, min_degree> d);
    record(const record<T, min_degree> &other);
    virtual ~record();
    record<T, min_degree>& operator=(const record<T, min_degree> &other);
    bool operator==(const record<T, min_degree> &other);
    node<T, min_degree> getData() const;
    void setData(node<T, min_degree> d);
    unsigned long long int getNext() const;
    void setNext(unsigned long long int n);
    bool isDeleted() const;
    void del();
    void undel();
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
protected:
    node<T, min_degree> data;       //o registro que será inserido no arquivo, será um dado do tipo "node"
    bool deleted;
    unsigned long long int next;
};

template <class T, const unsigned int min_degree>
record<T, min_degree>::record() : serializable()
{
    node<T, min_degree> data;
    this->data = data;
    this->next = 0;
    this->deleted = false;
}

template <class T, const unsigned int min_degree>
record<T, min_degree>::record(node<T, min_degree> d) : serializable()
{
    this->data = d;
    this->deleted = false;
    this->next = 0;
}

template <class T, const unsigned int min_degree>
record<T, min_degree>::record(const record<T, min_degree> &other)
{
    this->data = other.data;
    this->deleted = other.deleted;
    this->next = other.next;
}

template <class T, const unsigned int min_degree>
record<T, min_degree>::~record()
{

}

template <class T, const unsigned int min_degree>
record<T, min_degree>& record<T, min_degree>::operator=(const record<T, min_degree> &other)
{
    if(this == &other)
    {
        return *this;
    }
    this->data = other.data;
    this->deleted = other.deleted;
    this->next = other.next;
    return *this;
}

template <class T, const unsigned int min_degree>
bool record<T, min_degree>::operator==(const record<T, min_degree> &other)
{
    return (this->data == other.data) && (this->deleted == other.deleted) && (this->next == other.next);
}

template <class T, const unsigned int min_degree>
node<T, min_degree> record<T, min_degree>::getData() const
{
    return this->data;
}

template <class T, const unsigned int min_degree>
void record<T, min_degree>::setData(node<T, min_degree> d)
{
    this->data = d;
}

template <class T, const unsigned int min_degree>
unsigned long long int record<T, min_degree>::getNext() const
{
    return this->next;
}

template <class T, const unsigned int min_degree>
void record<T, min_degree>::setNext(unsigned long long int n)
{
    this->next = n;
}

template <class T, const unsigned int min_degree>
bool record<T, min_degree>::isDeleted() const
{
    return this->deleted;
}

template <class T, const unsigned int min_degree>
void record<T, min_degree>::del()
{
    this->deleted = true;
}

template <class T, const unsigned int min_degree>
void record<T, min_degree>::undel()
{
    this->deleted = false;
}

template <class T, const unsigned int min_degree>
string record<T, min_degree>::toString()
{
    string aux = "";
    aux += this->data.toString();
    aux += string(reinterpret_cast<char*>(&this->deleted), sizeof(this->deleted));
    aux += string(reinterpret_cast<char*>(&this->next), sizeof(this->next));
    return aux;
}

template <class T, const unsigned int min_degree>
void record<T, min_degree>::fromString(string repr)
{
    int pos = 0;
    string dataString = repr.substr(pos, this->data.size());
    this->data.fromString(dataString);
    pos += this->data.size();
    repr.copy(reinterpret_cast<char*>(&this->deleted), sizeof(this->deleted), pos);
    pos += sizeof(this->deleted);
    repr.copy(reinterpret_cast<char*>(&this->next), sizeof(this->next), pos);
}

template <class T, const unsigned int min_degree>
unsigned long long int record<T, min_degree>::size() const
{
    return this->data.size() + sizeof(this->deleted) + sizeof(this->next);
}

#endif // RECORD_H
