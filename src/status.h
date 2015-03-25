#ifndef _MIPS_AS_STATUS_H_
#define _MIPS_AS_STATUS_H_

#include <string>

namespace MIPS {

enum STATUS_CODE {
  S_OK = 0,
  S_IO_FAILED,
  S_MEM_FAILED,
  S_PARSE_FAILED,
  S_INVALID_DEFINATION,
  S_INVALID_VALUE
};

struct Status {
  STATUS_CODE code;
  std::string msg1;
  std::string msg2;
  std::string src;
  int line_number;
  int input_line_number = -1;
  int offset = -1;

  Status (
    STATUS_CODE code, 
    const std::string &src,
    int line_number,
    const std::string &msg1 = "",
    const std::string &msg2 = "")
    : code(code), msg1(msg1), msg2(msg2), src(src), line_number(line_number)
  { }

  operator bool () const
  {
    return !ok();
  }

  bool 
  ok() const
  {
    return !code;
  }
};

#define ERROR_IN_SOURCE(code, msg1, msg2) (Status(code, __FILE__, __LINE__, msg1, msg2))
#define ERROR(code, file, line, msg1, msg2) (Status(code, file, line, msg1, msg2))
#define OK() (Status(S_OK, __FILE__, __LINE__))

}

#endif // _MIPS_AS_STATUS_H_
