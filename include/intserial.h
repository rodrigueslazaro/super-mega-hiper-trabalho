#ifndef INTSERIAL_H
#define INTSERIAL_H

#include "serializable.h"

class intSerial : serializable {
   public:
      intSerial();
      intSerial(int i);
      intSerial(const intSerial& other);
      virtual ~intSerial();
      intSerial& operator=(const intSerial& other);
      bool operator==(const intSerial &other) const;
      bool operator<(const intSerial &other) const;
      bool operator<=(const intSerial &other) const;
      bool operator>(const intSerial &other) const;
      bool operator>=(const intSerial &other) const;
      bool operator!=(const intSerial &other) const;
      intSerial operator+(const intSerial& other) const;
      intSerial operator-(const intSerial& other) const;
      intSerial operator*(const intSerial& other) const;
      intSerial operator/(const intSerial& other) const;
      intSerial& operator++();
      intSerial& operator--();
      void setValue(int v);
      int getValue() const;
      virtual string toString();
      virtual void fromString(string repr);
      virtual string toXML();
      virtual void fromXML(string repr);
      virtual string toCSV();
      virtual void fromCSV(string repr);
      virtual string toJSON();
      virtual void fromJSON(string repr);
      virtual unsigned long long int size() const;
   protected:
      int value;
};

#endif // INTSERIAL_H
