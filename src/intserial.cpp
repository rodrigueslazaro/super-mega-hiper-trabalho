#include "intserial.h"

intSerial::intSerial() : serializable() {
    this->value = 0;
}

intSerial::intSerial(int i) : serializable() {
    this->value = i;
}

intSerial::intSerial(const intSerial& other) {
    this->value = other.value;
}

intSerial::~intSerial() {

}

intSerial& intSerial::operator=(const intSerial& other) {
    if(this == &other)          //evita auto atribuição -  Compara o endereço de memória do objeto atual (this) com o endereço de other.
        return *this;
    this->value = other.value;
    return *this;
}

bool intSerial::operator==(const intSerial &other) const {
    return this->value == other.value;
}

bool intSerial::operator<(const intSerial &other) const {
    return this->value < other.value;
}

bool intSerial::operator<=(const intSerial &other) const {
    return this->value <= other.value;
}

bool intSerial::operator>(const intSerial &other) const {
    return this->value > other.value;
}

bool intSerial::operator>=(const intSerial &other) const {
    return this->value >= other.value;
}

bool intSerial::operator!=(const intSerial &other) const {
    return this->value != other.value;
}

intSerial intSerial::operator+(const intSerial& other) const {
    intSerial aux(this->value + other.value);
    //intSerial aux;                            //poderia fazer assim tbm
    //aux.value = this->value + other.value;    //poderia fazer assim tbm
    return aux;
}

intSerial intSerial::operator-(const intSerial& other) const {
    intSerial aux(this->value - other.value);
    return aux;
}

intSerial intSerial::operator*(const intSerial& other) const {
    intSerial aux(this->value * other.value);
    return aux;
}

intSerial intSerial::operator/(const intSerial& other) const {
    intSerial aux(this->value/other.value);
    return aux;
}

intSerial& intSerial::operator++() {
    this->value += 1;
    return *this;
}

intSerial& intSerial::operator--() {
    this->value -= 1;
    return *this;
}

void intSerial::setValue(int v) {
    this->value = v;
}

int intSerial::getValue() const {
    return this->value;
}

string intSerial::toString() {
    string repr = "";
    repr += string(reinterpret_cast<char*>(&this->value), sizeof(value));
    //repr.replace(0, sizeof(this->value), reinterpret_cast<char*>(&this->value), sizeof(value));    //poderia ter sido feito assim
    return repr;
}

void intSerial::fromString(string repr) {
    repr.copy(reinterpret_cast<char*>(&this->value), sizeof(this->value), 0);
}

string intSerial::toXML() {
    return "";
}

void intSerial::fromXML(string repr) {
    //nao foi necessario
}

string intSerial::toCSV() {
    return "";
}

void intSerial::fromCSV(string repr) {
    //nao foi necessario
}

string intSerial::toJSON() {
    return "";
}

void intSerial::fromJSON(string repr) {
    //nao foi necessario
}

unsigned long long int intSerial::size() const {
    return sizeof(this->value);
}
