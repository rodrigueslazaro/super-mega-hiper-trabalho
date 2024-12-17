#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
#include <string>
using namespace std;

class serializable {
   public:
      serializable() {}
      virtual ~serializable() {}
      virtual string toString() = 0;
      virtual void fromString(string repr) = 0;
      virtual string toXML() = 0;
      virtual void fromXML(string repr) = 0;
      virtual string toCSV() = 0;
      virtual void fromCSV(string repr) = 0;
      virtual string toJSON() = 0;
      virtual void fromJSON(string repr) = 0;
      virtual unsigned long long int size() const = 0;
};

#endif // SERIALIZABLE_H
