#ifndef _MIPS_H_ARGUMENTS_H_
#define _MIPS_H_ARGUMENTS_H_

#include <string>
#include <cstring>

#include "config.h"

namespace MIPS {

struct Arguments {
  std::string input_file;
  std::string output_file;
  std::string def_file;
  std::string label_file;
  std::string item_file;
  bool print_help;
  bool quite;

  Arguments()
  : input_file(""),
    output_file("a.out"),
    def_file(Config::DIFINATION_PATH),
    label_file(""),
    item_file(""),
    print_help(false),
    quite(false)
  { }

  inline void
  parseArgument(int argc, char **argv)
  {
    for (int i = 1; i < argc; ++i) {
      if (std::strncmp(argv[i], "--def", 6) == 0)
        def_file = argv[++i];
      else if (std::strncmp(argv[i], "-d", 3) == 0)
        item_file = argv[++i];
      else if (std::strncmp(argv[i], "-h", 3) == 0 ||
               std::strncmp(argv[i], "--help", 7) == 0)
        print_help = true;
      else if (std::strncmp(argv[i], "-l", 3) == 0)
        label_file = argv[++i];
      else if (std::strncmp(argv[i], "-o", 3) == 0)
        output_file = argv[++i];
      else if (std::strncmp(argv[i], "-q", 3) == 0)
        quite = true;
      else
        input_file = argv[i];
    }
  }
};

};

#endif // _MIPS_H_ARGUMENTS_H_
