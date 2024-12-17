#include "IntSerial.h"

IntSerial::IntSerial() : Serializable()
{
    this->value = 0;
}

IntSerial::IntSerial(int i) : Serializable()
{
    this->value = i;
}

IntSerial::IntSerial(const IntSerial &other)
{
    this->value = other.value;
}

IntSerial::~IntSerial()
{
}

IntSerial &IntSerial::operator=(const IntSerial &other)
{
    if (this == &other) // evita auto atribui��o -  Compara o endere�o de mem�ria do objeto atual (this) com o endere�o de other.
        return *this;
    this->value = other.value;
    return *this;
}

bool IntSerial::operator==(const IntSerial &other) const
{
    return this->value == other.value;
}

bool IntSerial::operator<(const IntSerial &other) const
{
    return this->value < other.value;
}

bool IntSerial::operator<=(const IntSerial &other) const
{
    return this->value <= other.value;
}

bool IntSerial::operator>(const IntSerial &other) const
{
    return this->value > other.value;
}

bool IntSerial::operator>=(const IntSerial &other) const
{
    return this->value >= other.value;
}

bool IntSerial::operator!=(const IntSerial &other) const
{
    return this->value != other.value;
}

IntSerial IntSerial::operator+(const IntSerial &other) const
{
    IntSerial aux(this->value + other.value);
    // intSerial aux;                            //poderia fazer assim tbm
    // aux.value = this->value + other.value;    //poderia fazer assim tbm
    return aux;
}

IntSerial IntSerial::operator-(const IntSerial &other) const
{
    IntSerial aux(this->value - other.value);
    return aux;
}

IntSerial IntSerial::operator*(const IntSerial &other) const
{
    IntSerial aux(this->value * other.value);
    return aux;
}

IntSerial IntSerial::operator/(const IntSerial &other) const
{
    IntSerial aux(this->value / other.value);
    return aux;
}

IntSerial &IntSerial::operator++()
{
    this->value += 1;
    return *this;
}

IntSerial &IntSerial::operator--()
{
    this->value -= 1;
    return *this;
}

void IntSerial::setValue(int v)
{
    this->value = v;
}

int IntSerial::getValue() const
{
    return this->value;
}

string IntSerial::toString()
{
    string repr = "";
    repr += string(reinterpret_cast<char *>(&this->value), sizeof(value));
    // repr.replace(0, sizeof(this->value), reinterpret_cast<char*>(&this->value), sizeof(value));    //poderia ter sido feito assim
    return repr;
}

void IntSerial::fromString(string repr)
{
    repr.copy(reinterpret_cast<char *>(&this->value), sizeof(this->value), 0);
}

string IntSerial::toXML()
{
    return "";
}

void IntSerial::fromXML(string repr)
{
    // nao foi necessario
}

string IntSerial::toCSV()
{
    return "";
}

void IntSerial::fromCSV(string repr)
{
    // nao foi necessario
}

string IntSerial::toJSON()
{
    return "";
}

void IntSerial::fromJSON(string repr)
{
    // nao foi necessario
}

unsigned long long int IntSerial::size() const
{
    return sizeof(this->value);
}
