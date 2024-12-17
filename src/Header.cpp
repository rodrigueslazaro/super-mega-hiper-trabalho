#include "Header.h"

Header::Header() : Serializable()
{
   // ctor
   type = "***";
   firstDeleted = 0;
   firstValid = 0;
   version = 0;
}

Header::Header(const string t, unsigned int v) : Serializable()
{
   // ctor
   type = t.substr(0, 3);
   firstDeleted = 0;
   firstValid = 0;
   version = v;
}

Header::Header(const Header &h)
{
   // cpy ctor
   type = h.getType();
   firstDeleted = h.getFirstDeleted();
   firstValid = h.getFirstValid();
   version = h.getVersion();
}

Header::~Header()
{
   // dtor
}

Header &Header::operator=(const Header &h)
{
   if (this == &h)
      return *this;

   type = h.getType();
   firstDeleted = h.getFirstDeleted();
   firstValid = h.getFirstValid();
   version = h.getVersion();

   return *this;
}

bool Header::operator==(const Header &h)
{
   return type == h.getType() && version == h.getVersion() &&
          firstDeleted == h.getFirstDeleted() &&
          firstValid == h.getFirstValid();
}

unsigned long long int Header::getFirstDeleted() const
{
   return firstDeleted;
}

void Header::setFirstDeleted(unsigned long long int r)
{
   firstDeleted = r;
}

unsigned long long int Header::getFirstValid() const
{
   return firstValid;
}

void Header::setFirstValid(unsigned long long int r)
{
   firstValid = r;
}

string Header::getType() const
{
   return type;
}

void Header::setType(string t)
{
   type = t.substr(0, 3);
}

unsigned int Header::getVersion() const
{
   return version;
}

void Header::setVersion(unsigned int v)
{
   version = v;
}

string Header::toString()
{
   string aux = "";
   aux += type;
   aux += string(reinterpret_cast<char *>(&version), sizeof(version));
   aux += string(reinterpret_cast<char *>(&firstValid), sizeof(firstValid));
   aux += string(reinterpret_cast<char *>(&firstDeleted), sizeof(firstDeleted));
   return aux;
}

void Header::fromString(string repr)
{
   int pos = 0;
   type = repr.substr(pos, 3);
   pos += type.length();
   repr.copy(reinterpret_cast<char *>(&version), sizeof(version), pos);
   pos += sizeof(version);
   repr.copy(reinterpret_cast<char *>(&firstValid), sizeof(firstValid), pos);
   pos += sizeof(firstValid);
   repr.copy(reinterpret_cast<char *>(&firstDeleted), sizeof(firstDeleted), pos);
}

unsigned long long int Header::size() const
{
   return type.length() + sizeof(version) + sizeof(firstValid) + sizeof(firstDeleted);
}
