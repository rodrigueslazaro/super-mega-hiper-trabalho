#ifndef RECORD_H
#define RECORD_H

#include <string>
#include "Serializable.h"
#include "Node.h"

using namespace std;

template <class T, const unsigned int min_degree>
class Record : public Serializable
{
    static_assert(is_base_of<Serializable, T>::value, "T must be serializable");

public:
    Record();
    Record(Node<T, min_degree> d);
    Record(const Record<T, min_degree> &other);
    virtual ~Record();
    Record<T, min_degree> &operator=(const Record<T, min_degree> &other);
    bool operator==(const Record<T, min_degree> &other);
    Node<T, min_degree> getData() const;
    void setData(Node<T, min_degree> d);
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
    Node<T, min_degree> data; // o registro que ser� inserido no arquivo, ser� um dado do tipo "node"
    bool deleted;
    unsigned long long int next;
};

#endif // RECORD_H