#ifndef _MIPS_AS_PARSER_H_
#define _MIPS_AS_PARSER_H_

#include <fstream>
#include <vector>

#include "rapidxml/rapidxml.hpp"

#include "defination.h"
#include "status.h"

namespace MIPS {

class Defination;
class Item;
struct Env;

class Parser
{
  int line_number = 1;
public:
  Status parse(std::istream &fin, Defination &, std::vector<Item> &res, Env &env);

  Status parseItem(std::istream &, const Defination::DefEntry *, Item &);
  Status parseRegister(std::istream &, std::string &);
  Status parseImmediate(std::istream &, std::string &);
  Status parseOffset(std::istream &, std::string &);
  Status overWhite(std::istream &);
};

}

#endif // _MIPS_AS_PARSER_H_
