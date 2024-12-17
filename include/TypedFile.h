#ifndef TYPEDFILE_H
#define TYPEDFILE_H

#include <fstream>
#include <type_traits>

using namespace std;

const ios::openmode mode = ios::out | ios::in | ios::binary;

template <class T, const unsigned int min_degree>
class TypedFile : private fstream
{
    static_assert(is_base_of<serializable, T>::value, "T must be serializable");

public:
    TypedFile();
    TypedFile(const string name, const string type, const unsigned int version);
    virtual ~TypedFile();
    bool open(const string name, const string type, const unsigned int version, ios::openmode openmode = mode);
    bool isOpen();
    bool close();
    bool clear();
    bool readRecord(record<T, min_degree> &r, unsigned long long int i);
    bool writeRecord(record<T, min_degree> &r, unsigned long long int i);
    bool insertRecord(record<T, min_degree> &r);
    bool deleteRecord(unsigned long long int i);
    unsigned long long int getFirstValid();
    unsigned long long int getFirstDeleted();
    void setFirstValid(unsigned long long int valid);
    void setFirstDeleted(unsigned long long int deleted);
    unsigned long long int search(Node<T, min_degree> data);
    unsigned long long int searchLogic(Node<T, min_degree> data);

    bool writeNo(Node<T, min_degree> &n, unsigned long long int i);
    long long int insertNo(Node<T, min_degree> &n);

protected:
    header head;
    bool readHeader(header &h);
    bool writeHeader(header &h);
    unsigned long long int index2pos(unsigned long long int i);
    unsigned long long int pos2index(unsigned long long int p);
};

#endif // TYPEDFILE_H