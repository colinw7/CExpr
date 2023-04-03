#include <CExprI.h>

std::string
CExprInsertEscapeCodes(const std::string &str)
{
  uint i = 0;

  for (i = 0; i < str.size(); ++i)
    if (str[i] == '\\' || str[i] == '\"' || str[i] == '\'' || (str[i] != ' ' && ! isgraph(str[i])))
      break;

  if (i >= str.size())
    return "";

  std::string str1;

  for (i = 0; i < str.size(); ++i) {
    char c = str[i];

    if (c == ' ' || (c != '\\' && c != '\"' && c != '\'' && isgraph(c))) {
      str1 += c;

      continue;
    }

    str1 += '\\';

    switch (c) {
      case '\a'  : str1 += 'a' ; break;
      case '\b'  : str1 += 'b' ; break;
      case '\f'  : str1 += 'f' ; break;
      case '\n'  : str1 += 'n' ; break;
      case '\r'  : str1 += 'r' ; break;
      case '\t'  : str1 += 't' ; break;
      case '\v'  : str1 += 'v' ; break;
      case '\"'  : str1 += '\"'; break;
      case '\''  : str1 += '\''; break;
      case '\033': str1 += 'e' ; break;

      default: {
        auto c1 = uchar(c);

        uint digit1 = c1/64;
        uint digit2 = (c1 - digit1*64)/8;
        uint digit3 = c1 - digit1*64 - digit2*8;

        str1 += char('0' + digit1);
        str1 += char('0' + digit2);
        str1 += char('0' + digit3);

        break;
      }
    }
  }

  return str1;
}

bool
CExprStringToNumber(const std::string &str, uint *i, long &integer, double &real, bool &is_int)
{
  uint i1 = *i;

  if (str[*i] == '+' || str[*i] == '-')
    (*i)++;

  if (*i >= str.size())
    return false;

  if (*i < str.size() - 2 && str[*i] == '0' &&
      (str[*i + 1] == 'x' || str[*i + 1] == 'X') && isxdigit(str[*i + 2])) {
    (*i)++; // skip 2 chars
    (*i)++;

    uint j = *i;

    while (*i < str.size() && isxdigit(str[*i]))
      (*i)++;

    std::string str1 = str.substr(j, *i - j);

    long unsigned integer1;

    sscanf(str1.c_str(), "%lx", &integer1);

    integer = long(integer1);
    real    = double(integer1);
    is_int  = true;

    return true;
  }

  //uint j = *i;

  while (*i < str.size() && isdigit(str[*i]))
    (*i)++;

  bool point_found = (*i < str.size() && str[*i] == '.');

  if (point_found) {
    (*i)++;

    //j = *i;

    while (*i < str.size() && isdigit(str[*i]))
      (*i)++;
  }

  bool exponent_found = (*i < str.size() && (str[*i] == 'e' || str[*i] == 'E'));

  if (exponent_found) {
    uint i2 = *i;

    (*i)++;

    if (*i < str.size() && (str[*i] == '+' || str[*i] == '-'))
      (*i)++;

    if (*i < str.size() && isdigit(str[*i])) {
      //j = *i;

      while (*i < str.size() && isdigit(str[*i]))
        (*i)++;
    }
    else {
      *i = i2;

      exponent_found = false;
    }
  }

  std::string str1 = str.substr(i1, *i - i1);

  if (! point_found && ! exponent_found) {
    if (*i < str.size() && (str[*i] == 'l' || str[*i] == 'L' || str[*i] == 'u' || str[*i] == 'U'))
      (*i)++;
  }

  if (point_found || exponent_found) {
    real    = CStrUtil::toReal(str1);
    integer = long(real);
    is_int  = false;

    return true;
  }

  //------

  bool octal = false;

  if (str1[0] == '0') {
    octal = true;

    for (uint j = 1; j < str1.size(); ++j)
      if (str1[j] < '0' || str1[j] > '7') {
        octal = false;
        break;
      }
  }

  if (octal) {
    long unsigned integer1;

    sscanf(str1.c_str(), "%lo", &integer1);

    integer = long(integer1);
    real    = double(integer1);
    is_int  = true;

    return true;
  }

  long integer1;

  sscanf(str1.c_str(), "%ld", &integer1);

  integer = long(integer1);
  real    = double(integer1);
  is_int  = true;

  return true;
}
