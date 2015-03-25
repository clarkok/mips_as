#include <iostream>
#include <fstream>
#include <vector>

#include "arguments.h"
#include "defination.h"
#include "parser.h"
#include "item.h"
#include "env.h"
#include "visitor.h"

using namespace std;
using namespace MIPS;

#define OUTPUT_STATUS(os, s)                                          \
    do {                                                              \
      os << s.msg1 << '(' << s.code << ')' << ": " << s.msg2 << endl; \
      os << '\t' << s.src << " : " << s.line_number << endl;          \
      os << "\tline number: " << s.input_line_number                  \
         << "\toffset: " << s.offset << endl;                         \
    } while (0)

int
main(int argc, char **argv)
{
  if (argc == 1) {
    cout << Config::HELP_TEXT;
    exit(0);
  }

  Arguments args;
  args.parseArgument(argc, argv);

  if (args.print_help) {
    cout << Config::HELP_TEXT;
    exit(0);
  }

  if (args.input_file.length() == 0) {
    cerr << "No input file" << endl;
    exit(-1);
  }

  Defination defs;
  auto def_status = defs.load(args.def_file);
  if (def_status) {
    OUTPUT_STATUS(cerr, def_status);
    exit(-1);
  }

  if (!args.quite)
    cout << defs.size() << " definations loaded" << endl;

  vector<Item> items;
  Env env;
  ifstream fin(args.input_file);

  if (!fin) {
    cerr << "Unable to open input file: " << args.input_file << endl;
    exit(-1);
  }

  auto parse_status = Parser().parse(fin, defs, items, env);
  if (parse_status) {
    OUTPUT_STATUS(cerr, parse_status);
    exit(-1);
  }

  if (!args.quite)
    cout << items.size() << " items parsed" << endl;

  if (args.label_file.length()) {
    ofstream fout(args.label_file);

    if (!fout) {
      cerr << "Unable to open label file " << args.label_file << endl;
      exit(-1);
    }

    for (auto &i : env.labels)
      fout << i.first << '\t' << i.second << endl;
  }

  if (args.item_file.length()) {
    ofstream fout(args.item_file);

    if (!fout) {
      cerr << "Unable to open debug file " << args.item_file << endl;
      exit(-1);
    }

    auto debug_status = DebugVisitor(fout, &env, &defs).apply(items.begin(), items.end());
    if (debug_status) {
      OUTPUT_STATUS(cerr, debug_status);
      exit(-1);
    }
  }

  ofstream fout(args.output_file);
  if (!fout) {
    cerr << "Unable to open output file: " << args.output_file << endl;
    exit(-1);
  }

  auto code_gen_status = CodeGenVisitor(fout, &env, &defs).apply(items.begin(), items.end());
  if (code_gen_status)
    OUTPUT_STATUS(cerr, code_gen_status);

  return 0;
}

