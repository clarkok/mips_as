#include <algorithm>
#include <iostream>
#include <string>
#include <map>
#include <cstdint>

#include "status.h"
#include "visitor.h"
#include "item.h"
#include "env.h"
#include "defination.h"

using namespace MIPS;

#define CHECK_AND_RETURN(expr)    \
  do {                            \
    Status s = expr;              \
    if (s) {                      \
      s.offset = offset;          \
      return s;                   \
    }                             \
  } while(0)

int
Visitor::calcPadding(Item *it) const
{
  auto size = defs->getSize(it->type());

  if (offset % size)
    return size - (offset % size);
  else
    return 0;
}

Status
DebugVisitor::visit(Item *it)
{
  os << it->name() << ':' << it->type() << std::endl;

  for (auto i = it->literalBegin(); i != it->literalEnd(); ++i)
    os << '\t' << i->first << ":\t" << i->second << std::endl;

  os << std::endl;

  return OK();
}

Status
CodeGenVisitor::parseRegister(const std::string &literal, int &res) const
{
  if (std::all_of(literal.begin(), literal.end(), ::isdigit)) {
    res = std::stoi(literal);
  }
  else {
    res = defs->getRegister(literal);
    if (res == -1)
      return ERROR_IN_SOURCE(S_INVALID_VALUE, "Not known register", literal);
  }

  return OK();
}

Status
CodeGenVisitor::parseImmediate(const std::string &literal, int &res) const
{
  if (literal.length() == 0)
    return ERROR_IN_SOURCE(S_INVALID_VALUE, "Empty literal", "Immediate");

  if (std::all_of(literal.begin(), literal.end(), 
      [] (char ch) -> bool {
        if (::isdigit(ch))
          return true;
        return (ch == '-');
      })) {
    res = std::stoi(literal);
  }
  else {
    if (literal.length() > 2 && literal[0] == '0') {
      std::string tmp;
      switch (literal[1]) {
        case 'x':
          res = static_cast<int>(std::stoll(literal.substr(2), nullptr, 16));
          break;
        case 'b':
          tmp = literal.substr(2);
          res = static_cast<int>(std::stoll(literal.substr(2), nullptr, 2));
          break;
        default:
          res = static_cast<int>(std::stoll(literal, nullptr, 8));
          break;
      };
    }
    else {
      auto iter = env->labels.find(literal);
      if (iter != env->labels.end()) {
        res = (iter->second);
      }
      else
        return ERROR_IN_SOURCE(S_INVALID_VALUE, "Invalid immediate", literal);
    }
  }

  return OK();
}

Status
CodeGenVisitor::parseAddress(const std::string &literal, int &res) const
{
  auto ret = parseImmediate(literal, res);
  res = (res >> 2);
  return ret;
}

Status
CodeGenVisitor::parseBranch(const std::string &literal, int &res) const
{
  auto ret = parseImmediate(literal, res);
  res = (res >> 2) - ((offset >> 2) + 1);
  return ret;
}

Status
CodeGenVisitor::visit(Item *it)
{
  auto *decl = defs->getDecleration(it->type());
  std::map<std::string, int> params;

  std::string format = decl->first_attribute("format")->value();
  format.erase(std::remove(format.begin(), format.end(), '-'), format.end());

  for (auto *d = decl->first_node("param"); d; d = d->next_sibling("param")) {
    int res;
    std::string name = d->first_attribute("name")->value();
    std::string type = d->first_attribute("type")->value();
    std::string rep = d->first_attribute("rep")->value();

    if (type == "register") {
      CHECK_AND_RETURN(parseRegister(it->getLiteral(name), res));
    }
    else if (type == "immediate") {
      CHECK_AND_RETURN(parseImmediate(it->getLiteral(name), res));
    }
    else if (type == "address") {
      CHECK_AND_RETURN(parseAddress(it->getLiteral(name), res));
    }
    else if (type == "branch") {
      CHECK_AND_RETURN(parseBranch(it->getLiteral(name), res));
    }
    else
      return ERROR_IN_SOURCE(S_INVALID_VALUE, "Invalid decl type", type);

    if (params.find(rep) != params.end())
      return ERROR_IN_SOURCE(S_INVALID_VALUE, "Duplicated rep char", rep);

    if (rep.length() > 1)
      return ERROR_IN_SOURCE(S_INVALID_VALUE, "Too long for rep char", rep);

    params[rep] = res;
  }

  uint32_t data = 0;
  int size = (8 * defs->getSize(it->type()));
  if (static_cast<int>(format.length()) != size)
    return ERROR_IN_SOURCE(
        S_INVALID_DEFINATION, 
        "Format length dismatch", 
        "For type " + it->type());

  char current = format[size - 1];
  int start = size - 1;

#define CALC_MASK(size) static_cast<uint32_t>((static_cast<uint64_t>(1) << (size)) - 1)
#define SHIFT_AND_MASK(val, shl, size) ((static_cast<uint32_t>(val) & CALC_MASK(size)) << (shl))

  for (int i = size - 1; i >= 0; --i) {
    if (format[i] != current) {
      data |= SHIFT_AND_MASK(params[std::string(1, current)], size - start - 1, start - i);
      current = format[i];
      start = i;
    }
  }
  data |= SHIFT_AND_MASK(params[std::string(1, current)], size - start - 1, start + 1);

  uint32_t zero = 0;

  os.write(reinterpret_cast<char*>(&zero), calcPadding(it));
  os.write(reinterpret_cast<char*>(&data), defs->getSize(it->type()));

  offset += calcPadding(it) + defs->getSize(it->type());

  return OK();
}
