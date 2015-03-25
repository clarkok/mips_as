#ifndef _MIPS_AS_INSTRUMENT_H_
#define _MIPS_AS_INSTRUMENT_H_

#include <fstream>
#include <map>
#include <string>

#include "status.h"

namespace MIPS {

struct Env;
class Parser;
class Visitor;

class Item
{
  std::string name_;
  std::string type_;
  std::map<std::string, std::string> literal;

public:
  friend class Parser;

  std::string
  name() const
  {
    return name_;
  }

  std::string
  type() const
  {
    return type_;
  }

  std::string
  getLiteral(const std::string &na) const
  {
    auto iter = literal.find(na);
    if (iter == literal.end())
      return "";
    else
      return iter->second;
  }

  auto
  literalBegin() -> decltype(literal.begin()) const
  {
    return literal.begin();
  }

  auto
  literalEnd() -> decltype(literal.end()) const
  {
    return literal.end();
  }
};

};

#endif // _MIPS_AS_INSTRUMENT_H_
