#ifndef HEADER_H
#define HEADER_H

#include "Serializable.h"

class Header : public Serializable {
   public:
      Header();
      Header(const string t, unsigned int v);
      Header(const Header &h);
      virtual ~Header();
      Header& operator=(const Header &h);
      bool operator==(const Header &h);
      unsigned long long int getFirstDeleted() const;
      void setFirstDeleted(unsigned long long int r);
      unsigned long long int getFirstValid() const;
      void setFirstValid(unsigned long long int r);
      string getType() const;
      void setType(string t);
      unsigned int getVersion() const;
      void setVersion(unsigned int v);
      virtual string toString();
      virtual void fromString(string repr);
      virtual string toXML() { return ""; };
      virtual void fromXML(string repr) {};
      virtual string toCSV() { return ""; };
      virtual void fromCSV(string repr){};
      virtual string toJSON() { return ""; };
      virtual void fromJSON(string repr) {};
      virtual unsigned long long int size() const;
   protected:
      string type;
      unsigned int version;
      unsigned long long int firstValid;
      unsigned long long int firstDeleted;
};

#endif // HEADER_H