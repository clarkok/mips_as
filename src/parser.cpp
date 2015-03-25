#include <cstring>
#include <limits>
#include <cstdlib>

#include "rapidxml/rapidxml.hpp"

#include "env.h"
#include "status.h"
#include "item.h"
#include "parser.h"
#include "defination.h"

using namespace MIPS;

#define CHECK_AND_RETURN(stmt)    \
  do {                            \
    Status s = (stmt);            \
    if (s) {                      \
      s.input_line_number = line_number; \
      return s;                   \
    }                             \
  } while (0)

Status
Parser::parse(std::istream &in, Defination &def, std::vector<Item> &res, Env &env)
{
  int offset = 0;

  std::string first;

  decltype(env.labels.begin()) last_label = env.labels.end();

  while (in) {
    overWhite(in);
    switch(in.peek()) {
      case '#':
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      default:
        if (in >> first) {
          if (first.back() == ':') {
            first.pop_back();
            env.labels[first] = offset;
            last_label = env.labels.find(first);
          }
          else {
            res.push_back(Item());
            if (!def.getDefination(first))
              return ERROR_IN_SOURCE(S_PARSE_FAILED, "Unknown instrument", first);
            CHECK_AND_RETURN(parseItem(in, def.getDefination(first), res.back()));
            auto size = def.getSize(def.getDefination(first)->first_attribute("type")->value());
            if (offset % size != 0)
              offset += size - (offset % size); // padding
            if (last_label != env.labels.end()) {
              last_label->second = offset;
              last_label = env.labels.end();
            }
            offset += size;
          }
        }
        else
          return OK();
    };
  }

  return OK();
}

Status
Parser::overWhite(std::istream &in)
{
  while (in) {
    switch (in.peek()) {
      case ' ': case '\r': case '\v': case '\t':
        in.get();
        break;
      case '\n':
        line_number++;
        in.get();
        break;
      default:
        return OK();
    };
  }
  return OK();
}

Status
Parser::parseRegister(std::istream &in, std::string &res)
{
  CHECK_AND_RETURN(overWhite(in));

  if (in.peek() != '$')
    return ERROR_IN_SOURCE(S_PARSE_FAILED, "Parse Error", "Expect $");

  in.get(); // '$'

  res.clear();

  while (in) {
    if (
      (in.peek() >= '0' && in.peek() <= '9') ||
      (in.peek() >= 'a' && in.peek() <= 'z') ||
      (in.peek() >= 'A' && in.peek() <= 'Z'))
      res += in.get();
    else
      if (res.length())
        return OK();
      else
        return ERROR_IN_SOURCE(S_PARSE_FAILED, "Parse error", "Expect register");
  }

return ERROR_IN_SOURCE(S_PARSE_FAILED, "Parse error", "unexpected EOF");
}

Status
Parser::parseImmediate(std::istream &in, std::string &res)
{
  CHECK_AND_RETURN(overWhite(in));

  if (in.peek() == '0') {
    res += in.get(); // '0'
    switch (in.peek()) {
      case 'x':
        res += in.get();
        while (in) {
          if ((in.peek() >= '0' && in.peek() <= '9') ||
              (in.peek() >= 'a' && in.peek() <= 'f') ||
              (in.peek() >= 'A' && in.peek() <= 'F'))
            res += in.get();
          else
            return OK();
        }
        return OK();
        break;
      case 'b':
        res += in.get();
        while (in) {
          if (in.peek() >= '0' && in.peek() <= '1')
            res += in.get();
          else
            return OK();
        }
        return OK();
        break;
      default:
        while (in) {
          if (in.peek() >= '0' && in.peek() <= '7')
            res += in.get();
          else
            return OK();
        }
        return OK();
    };
  }
  else {
    while (in) {
      if ((in.peek() >= '0' && in.peek() <= '9') ||
          (in.peek() >= 'a' && in.peek() <= 'z') ||
          (in.peek() >= 'A' && in.peek() <= 'Z') ||
          (in.peek() == '_') || (in.peek() == '-'))
        res += in.get();
      else
        break;
    }
    if (res.length())
      return OK();
    else
      return ERROR_IN_SOURCE(S_PARSE_FAILED, "Parse error", "Expect immediate");
  }
}

Status
Parser::parseOffset(std::istream &in, std::string &res)
{
  return parseImmediate(in, res);
}

Status
Parser::parseItem(std::istream &in, const Defination::DefEntry *def, Item &res)
{
  for (auto *a = def->first_attribute(); a; a = a->next_attribute()) {
    if (
      std::strncmp(a->name(), "name", a->name_size() + 1) && 
      std::strncmp(a->name(), "type", a->name_size() + 1)
    )
      res.literal[a->name()] = a->value();
  }

  res.name_ = def->first_attribute("name")->value();

  if (def->first_attribute("type"))
    res.type_ = def->first_attribute("type")->value();
  else
    res.type_ = "";

  for (auto *n = def->first_node("argument"); n; n = n->next_sibling("argument")) {
    if (std::strncmp(
        n->first_attribute("type")->value(),
        "register",
        n->first_attribute("type")->value_size()
      ) == 0) {
      CHECK_AND_RETURN(parseRegister(in, res.literal[n->first_attribute("name")->value()]));
    }
    else if (std::strncmp(
        n->first_attribute("type")->value(),
        "immediate",
        n->first_attribute("type")->value_size()
      ) == 0) {
      CHECK_AND_RETURN(parseImmediate(in, res.literal[n->first_attribute("name")->value()]));
    }
    else if (std::strncmp(
        n->first_attribute("type")->value(),
        "address",
        n->first_attribute("type")->value_size()
      ) == 0) {
      CHECK_AND_RETURN(parseImmediate(in, res.literal[n->first_attribute("name")->value()]));
    }
    else if (std::strncmp(
        n->first_attribute("type")->value(),
        "offset",
        n->first_attribute("type")->value_size()
      ) == 0) {
      CHECK_AND_RETURN(parseOffset(in, res.literal[n->first_attribute("name")->value()]));
      CHECK_AND_RETURN(overWhite(in));
      if (in.peek() == '(') {
        in.get();
        if (std::strncmp(
            n->next_sibling("argument")->first_attribute("type")->value(), 
            "register",
            n->next_sibling("argument")->first_attribute("type")->value_size()
          ) == 0) {
          n = n->next_sibling("argument");
          CHECK_AND_RETURN(parseRegister(in, res.literal[n->first_attribute("name")->value()]));
        }
        else
          return ERROR_IN_SOURCE(
            S_INVALID_DEFINATION, 
            "Invalid defination", 
            "Offset should be followed by register");
      }
      else
        return ERROR_IN_SOURCE(S_PARSE_FAILED, "Parse error", "Expect (");

      CHECK_AND_RETURN(overWhite(in));
      if (in.peek() == ')')
        in.get();
      else
        return ERROR_IN_SOURCE(S_PARSE_FAILED, "Parse error", "Expect )");
    }
    else
      return ERROR_IN_SOURCE(S_PARSE_FAILED,
          "Unknow argument type",
          n->first_attribute("type")->value());

    if (n->next_sibling("argument")) {
      CHECK_AND_RETURN(overWhite(in));
      if (in.peek() == ',')
        in.get();
      else
        return ERROR_IN_SOURCE(S_PARSE_FAILED, "Parse error", "Expect ,");
    }
  }

  return OK();
}
