#ifndef _MIPS_AS_VISITOR_H_
#define _MIPS_AS_VISITOR_H_

#include <iostream>

#include "status.h"

namespace MIPS {

class Item;
class Defination;
struct Env;

class Visitor
{
protected:
  int offset = 0;
  Env *env;
  Defination *defs;

  int calcPadding(Item *) const;

public:
  Visitor(Env *env, Defination *defs)
  : env(env), defs(defs) { }

  virtual Status visit(Item *) = 0;

  template<class T_iter>
  Status apply(T_iter begin, T_iter end)
  {
    while (begin != end) {
      Status s = visit(&(*begin++));
      if (s)
        return s;
    }
    return OK();
  }
};

class OutputVisitor : public Visitor
{
protected:
  std::ostream &os;

public:
  OutputVisitor(std::ostream &os, Env *env, Defination *defs)
  : Visitor(env, defs), os(os) { }
};

class DebugVisitor : public OutputVisitor
{
public:
  DebugVisitor(std::ostream &os, Env *env, Defination *defs)
  : OutputVisitor(os, env, defs) { }

  virtual Status visit(Item *);
};

class CodeGenVisitor : public OutputVisitor
{
public:
  CodeGenVisitor(std::ostream &os, Env *env, Defination *defs)
  : OutputVisitor(os, env, defs) { }

  virtual Status visit(Item *);

  Status parseRegister(const std::string &, int &) const;
  Status parseImmediate(const std::string &, int &) const;
  Status parseAddress(const std::string &, int &) const;
  Status parseBranch(const std::string &, int &) const;
};

}

#endif // _MIPS_AS_VISITOR_H_
