#ifndef _MIPS_AS_CONFIG_H_
#define _MIPS_AS_CONFIG_H_

namespace MIPS {

namespace Config {

static char HELP_TEXT[] =                                         \
"Usage: mas [options] file\n"                                     \
"  Options:\n"                                                    \
"    --def <file>   Defination file. Default res/defination.xml\n"\
"    -d <file>      Output debug values\n"                        \
"    -h, --help     Print this help\n"                            \
"    -l <file>      Output labels \n"                             \
"    -o <file>      Output file\n"                                \
"    -q             Quite\n"                                      \
;

static char DIFINATION_PATH[] = "res/defination.xml";
}

}

#endif // _MIPS_AS_CONFIG_H_
