#ifndef INTSERIAL_H
#define INTSERIAL_H

#include "Serializable.h"

class IntSerial : Serializable {
   public:
      IntSerial();
      IntSerial(int i);
      IntSerial(const IntSerial& other);
      virtual ~IntSerial();
      IntSerial& operator=(const IntSerial& other);
      bool operator==(const IntSerial &other) const;
      bool operator<(const IntSerial &other) const;
      bool operator<=(const IntSerial &other) const;
      bool operator>(const IntSerial &other) const;
      bool operator>=(const IntSerial &other) const;
      bool operator!=(const IntSerial &other) const;
      IntSerial operator+(const IntSerial& other) const;
      IntSerial operator-(const IntSerial& other) const;
      IntSerial operator*(const IntSerial& other) const;
      IntSerial operator/(const IntSerial& other) const;
      IntSerial& operator++();
      IntSerial& operator--();
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