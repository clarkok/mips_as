#ifndef _MIPS_AS_ENV_H_
#define _MIPS_AS_ENV_H_

#include <map>
#include <string>

namespace MIPS {

struct Env
{
  std::map<std::string, int> labels;
};

};

#endif // _MIPS_AS_ENV_H_
