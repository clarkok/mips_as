#ifndef _MIPS_AS_DIFINATION_H_
#define _MIPS_AS_DIFINATION_H_

#include <iostream>

#include <map>
#include <string>

#include "rapidxml/rapidxml.hpp"

#include "config.h"
#include "status.h"

namespace MIPS {

class Item;

class Defination
{
public:
  typedef rapidxml::xml_node<char> DefEntry;

private:
  char * content = nullptr;
  rapidxml::xml_document<char> doc;
  std::map<std::string, DefEntry*> defs;
  std::map<std::string, DefEntry*> decl;
  std::map<std::string, int> sizes;
  std::map<std::string, int> registers;

  Status collect();

public:
  ~Defination() {
    if (content)
      delete[] content;
  }

  Status load(const std::string &path = Config::DIFINATION_PATH);

  const DefEntry *
  getDefination(const std::string &name) const
  {
    auto iter = defs.find(name);
    if (defs.end() == iter)
      return nullptr;
    else
      return iter->second;
  }

  const DefEntry *
  getDecleration(const std::string &name) const
  {
    auto iter = decl.find(name);
    if (decl.end() == iter)
      return nullptr;
    else
      return iter->second;
  }

  int
  getRegister(const std::string &name) const
  {
    auto iter = registers.find(name);
    if (iter == registers.end())
      return -1;
    else
      return iter->second;
  }

  int
  getSize(const std::string &type) const
  {
    auto iter = sizes.find(type);
    if (iter == sizes.end())
      return 0;
    else
      return iter->second;
  }

  int 
  size() const
  {
    return defs.size();
  }

  void
  dump(std::ostream &os) const
  {
    for (auto &def : defs) {
      os << def.first << " " << def.second->first_attribute("name")->value() << std::endl;
    }
  }
};

}

#endif // _MIPS_AS_DIFINATION_H_
