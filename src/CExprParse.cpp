#include <CExprI.h>
#include <sstream>
#include <cstdio>

class CExprParseImpl {
 public:
  CExprParseImpl(CExpr *expr) : expr_(expr) { }
 ~CExprParseImpl() { }

  CExprTokenStack parseFile(const std::string &filename);
  CExprTokenStack parseFile(FILE *fp);
  CExprTokenStack parseFile(CFile &file);
  CExprTokenStack parseLine(const std::string &str);

  bool skipExpression(const std::string &str, uint &i, const std::string &echars="");

 private:
  bool parseLine(CExprTokenStack &stack, const std::string &line);
  bool parseLine(CExprTokenStack &stack, const std::string &line, uint &i);

  void parseError(const std::string &msg, const std::string &line, uint i);

  bool isNumber(const std::string &str, uint i);

  CExprTokenBaseP readNumber    (const std::string &str, uint *i);
  CExprTokenBaseP readString    (const std::string &str, uint *i);
  CExprTokenBaseP readOperator  (const std::string &str, uint *i);
  CExprTokenBaseP readIdentifier(const std::string &str, uint *i);
#if 0
  CExprTokenBaseP readUnknown(const std::string &str, uint *i);
#endif

  bool        skipNumber    (const std::string &str, uint *i);
  bool        skipString    (const std::string &str, uint *i);
  CExprOpType skipOperator  (const std::string &str, uint *i);
  void        skipIdentifier(const std::string &str, uint *i);

  bool readStringChars    (const std::string &str, uint *i, bool process, std::string &str1);
  void readIdentifierChars(const std::string &str, uint *i, std::string &identifier);

#if 0
  CExprTokenBaseP createUnknownToken();
#endif
  CExprTokenBaseP createIdentifierToken(const std::string &str);
  CExprTokenBaseP createOperatorToken  (CExprOpType op);
  CExprTokenBaseP createIntegerToken   (long);
  CExprTokenBaseP createRealToken      (double);
  CExprTokenBaseP createStringToken    (const std::string &str);

  std::string replaceEscapeCodes(const std::string &str);

  bool isBooleanOp(CExprOpType op) const;

 private:
  CExpr*          expr_ { 0 };
  CExprTokenStack tokenStack_;
};

//-----------

CExprParse::
CExprParse(CExpr *expr) :
 expr_(expr)
{
  impl_ = CExprParseImplP(new CExprParseImpl(expr));
}

CExprParse::
~CExprParse()
{
}

CExprTokenStack
CExprParse::
parseFile(const std::string &filename)
{
  return impl_->parseFile(filename);
}

CExprTokenStack
CExprParse::
parseFile(FILE *fp)
{
  return impl_->parseFile(fp);
}

CExprTokenStack
CExprParse::
parseLine(const std::string &line)
{
  return impl_->parseLine(line);
}

bool
CExprParse::
skipExpression(const std::string &line, uint &i)
{
  return impl_->skipExpression(line, i);
}

//-----------

CExprTokenStack
CExprParseImpl::
parseFile(const std::string &filename)
{
  CFile file(filename);

  return parseFile(file);
}

CExprTokenStack
CExprParseImpl::
parseFile(FILE *fp)
{
  CFile file(fp);

  return parseFile(file);
}

CExprTokenStack
CExprParseImpl::
parseFile(CFile &file)
{
  std::vector<std::string> lines;

  file.toLines(lines);

  tokenStack_.clear();

  auto num_lines = lines.size();

  for (uint i = 0; i < num_lines; i++)
    if (! parseLine(tokenStack_, lines[i]))
      return CExprTokenStack();

  return tokenStack_;
}

CExprTokenStack
CExprParseImpl::
parseLine(const std::string &line)
{
  tokenStack_.clear();

  bool flag = parseLine(tokenStack_, line);

  if (! flag)
    return CExprTokenStack();

  return tokenStack_;
}

bool
CExprParseImpl::
parseLine(CExprTokenStack &stack, const std::string &line)
{
  uint i = 0;

  return parseLine(stack, line, i);
}

bool
CExprParseImpl::
parseLine(CExprTokenStack &stack, const std::string &line, uint &i)
{
  CExprTokenBaseP ptoken;
  CExprTokenBaseP lastPToken;

  auto lastPTokenType = CExprTokenType::UNKNOWN;

  while (true) {
    CStrUtil::skipSpace(line, &i);
    if (i >= line.size()) break;

    if      (CExprOperator::isOperatorChar(line[i])) {
      CExprOpType lastOpType =
       (lastPToken.isValid() && lastPToken->type() == CExprTokenType::OPERATOR ?
        lastPToken->getOperator() : CExprOpType::UNKNOWN);

      if (lastPTokenType == CExprTokenType::UNKNOWN || lastOpType != CExprOpType::UNKNOWN) {
        if ((line[i] == '-' || line[i] == '+') && i < line.size() - 1 && isdigit(line[i + 1]))
          ptoken = readNumber(line, &i);
        else
          ptoken = readOperator(line, &i);
      }
      else
        ptoken = readOperator(line, &i);

      if (ptoken.isValid() && ptoken->type() == CExprTokenType::OPERATOR) {
        if      (ptoken->getOperator() == CExprOpType::OPEN_RBRACKET) {
          stack.addToken(ptoken);

          if (! parseLine(stack, line, i))
            return false;

          lastPToken     = CExprTokenBaseP();
          lastPTokenType = CExprTokenType::VALUE;

          continue;
        }
        else if (ptoken->getOperator() == CExprOpType::CLOSE_RBRACKET) {
          stack.addToken(ptoken);

          return true;
        }
      }
    }
    else if (isNumber(line, i))
      ptoken = readNumber(line, &i);
    else if (line[i] == '_' || isalpha(line[i]))
      ptoken = readIdentifier(line, &i);
    else if (line[i] == '\'' || line[i] == '\"')
      ptoken = readString(line, &i);
    else {
      parseError("Invalid Character", line, i);
      return false;
    }

    if (! ptoken.isValid()) {
      parseError("Invalid Token", line, i);
      return false;
    }

    stack.addToken(ptoken);

    lastPToken     = ptoken;
    lastPTokenType = ptoken->type();
  }

  return true;
}

bool
CExprParseImpl::
skipExpression(const std::string &line, uint &i, const std::string &echars)
{
  auto lastTokenType = CExprTokenType::UNKNOWN;
  auto lastOpType    = CExprOpType::UNKNOWN;

  uint i1   = i;
  uint len1 = uint(line.size());

  while (true) {
    CStrUtil::skipSpace(line, &i);
    if (i >= len1) break;

    auto lastTokenType1 = lastTokenType;
    auto lastOpType1    = lastOpType;

    lastTokenType = CExprTokenType::UNKNOWN;
    lastOpType    = CExprOpType::UNKNOWN;

    if      (CExprOperator::isOperatorChar(line[i])) {
      if (line[i] == ',' || echars.find(line[i]) != std::string::npos)
        break;

      uint i2 = i;

      if (lastTokenType1 == CExprTokenType::OPERATOR && lastOpType1 != CExprOpType::UNKNOWN) {
        if ((line[i] == '-' || line[i] == '+') && i < len1 - 1 && isdigit(line[i + 1])) {
          if (! skipNumber(line, &i)) {
            break;
          }

          lastTokenType = CExprTokenType::REAL;
        }
        else {
          lastOpType = skipOperator(line, &i);

          if (lastOpType != CExprOpType::UNKNOWN) {
            if (lastTokenType1 == CExprTokenType::STRING) {
              i = i2;
              break;
            }

            lastTokenType = CExprTokenType::OPERATOR;
          }
        }
      }
      else {
        lastOpType = skipOperator(line, &i);

        if (lastOpType != CExprOpType::UNKNOWN) {
          if (lastTokenType1 == CExprTokenType::STRING) {
            i = i2;
            break;
          }

          lastTokenType = CExprTokenType::OPERATOR;
        }
      }

      if (lastTokenType == CExprTokenType::OPERATOR) {
        if      (lastTokenType == CExprTokenType::OPERATOR &&
                 lastOpType == CExprOpType::OPEN_RBRACKET) {
          while (i < len1) {
            if (! skipExpression(line, i, ",)")) {
              i = i1; return false;
            }

            // check for comma separated expression list
            CStrUtil::skipSpace(line, &i);

            if (i >= len1 || line[i] != ',')
              break;

            lastTokenType = CExprTokenType::OPERATOR;
            lastOpType    = CExprOpType::COMMA;

            ++i;
          }

          CStrUtil::skipSpace(line, &i);

          if (i >= len1 || line[i] != ')') {
            i = i1; return false;
          }

          lastTokenType = CExprTokenType::VALUE;

          ++i;

          continue;
        }
        else if (lastTokenType == CExprTokenType::OPERATOR &&
                 lastOpType == CExprOpType::CLOSE_RBRACKET) {
          i = i2;

          break;
        }
        else if (lastTokenType == CExprTokenType::OPERATOR &&
                 lastOpType == CExprOpType::QUESTION) {
          if (! skipExpression(line, i, ":")) {
            i = i1; return false;
          }

          CStrUtil::skipSpace(line, &i);

          if (i >= len1 || line[i] != ':') {
            i = i1; return false;
          }

          ++i;

          CStrUtil::skipSpace(line, &i);

          if (! skipExpression(line, i)) {
            i = i1; return false;
          }

          lastTokenType = CExprTokenType::VALUE;

          continue;
        }
        else if (lastTokenType == CExprTokenType::OPERATOR && lastOpType == CExprOpType::COLON) {
          i = i2;

          break;
        }
      }
    }
    else if (isNumber(line, i)) {
      if (lastTokenType1 != CExprTokenType::UNKNOWN &&
          lastTokenType1 != CExprTokenType::OPERATOR)
        break;

      if (! skipNumber(line, &i)) {
        i = i1; return false;
      }

      lastTokenType = CExprTokenType::REAL;
    }
    else if (line[i] == '_' || isalpha(line[i])) {
      if (lastTokenType1 != CExprTokenType::UNKNOWN &&
          lastTokenType1 != CExprTokenType::OPERATOR)
        break;

      skipIdentifier(line, &i);

      lastTokenType = CExprTokenType::IDENTIFIER;
    }
    else if (line[i] == '\'' || line[i] == '\"') {
      if (lastTokenType1 != CExprTokenType::UNKNOWN &&
          lastTokenType1 != CExprTokenType::OPERATOR)
        break;

      if (! skipString(line, &i)) {
        i = i1; return false;
      }

      if (lastTokenType1 == CExprTokenType::OPERATOR && isBooleanOp(lastOpType1))
        lastTokenType = CExprTokenType::INTEGER;
      else
        lastTokenType = CExprTokenType::STRING;
    }
    else {
      break;
    }

    if (lastTokenType == CExprTokenType::UNKNOWN) {
      break;
    }
  }

  return true;
}

void
CExprParseImpl::
parseError(const std::string &msg, const std::string &line, uint i)
{
  std::stringstream ostr;

  ostr << msg << " \"" << line.substr(0, i - 1) << "#" << line[i] << "#" <<
          (i < line.size() ? line.substr(i + 1) : "") << std::endl;

  for (uint j = 0; j < i + msg.size() + 1; j++)
    ostr << " ";

  ostr << "^";

  expr_->errorMsg(ostr.str());
}

bool
CExprParseImpl::
isBooleanOp(CExprOpType op) const
{
  return (op == CExprOpType::LESS         || op == CExprOpType::LESS_EQUAL ||
          op == CExprOpType::GREATER      || op == CExprOpType::GREATER_EQUAL ||
          op == CExprOpType::EQUAL        || op == CExprOpType::NOT_EQUAL ||
          op == CExprOpType::APPROX_EQUAL);
}

bool
CExprParseImpl::
isNumber(const std::string &str, uint i)
{
  if (i >= str.size())
    return false;

  if (isdigit(str[i]))
    return true;

  if (str[i] == '.') {
    i++;

    if (i >= str.size())
      return false;

    if (isdigit(str[i]))
      return true;

    return false;
  }

  if (str[i] == '+' || str[i] == '-') {
    i++;

    return isNumber(str, i);
  }

  return false;
}

CExprTokenBaseP
CExprParseImpl::
readNumber(const std::string &str, uint *i)
{
  long   integer = 0;
  double real    = 0.0;
  bool   is_int  = false;

  if (! CExprStringToNumber(str, i, integer, real, is_int))
    return CExprTokenBaseP();

  if (is_int)
    return createIntegerToken(integer);
  else
    return createRealToken(real);
}

bool
CExprParseImpl::
skipNumber(const std::string &str, uint *i)
{
  long   integer = 0;
  double real    = 0.0;
  bool   is_int  = false;

  if (! CExprStringToNumber(str, i, integer, real, is_int))
    return false;

  return true;
}

CExprTokenBaseP
CExprParseImpl::
readString(const std::string &str, uint *i)
{
  std::string str1;

  if (! readStringChars(str, i, true, str1))
    return CExprTokenBaseP();

  return createStringToken(str1);
}

bool
CExprParseImpl::
skipString(const std::string &str, uint *i)
{
  std::string str1;

  return readStringChars(str, i, false, str1);
}

bool
CExprParseImpl::
readStringChars(const std::string &str, uint *i, bool process, std::string &str1)
{
  if      (str[*i] == '\'') {
    (*i)++;

    uint j = *i;

    while (*i < str.size()) {
      if      (str[*i] == '\\' && *i < str.size() - 1)
        (*i)++;
      else if (str[*i] == '\'')
        break;

      (*i)++;
    }

    if (*i >= str.size())
      return false;

    str1 = str.substr(j, *i - j);

    (*i)++;
  }
  else if (str[*i] == '\"') {
    (*i)++;

    uint j = *i;

    str1 += str[*i];

    while (*i < str.size()) {
      if      (str[*i] == '\\' && *i < str.size() - 1)
        (*i)++;
      else if (str[*i] == '\"')
        break;

      (*i)++;
    }

    if (*i >= str.size())
      return false;

    str1 = str.substr(j, *i - j);

    (*i)++;

    if (process)
      str1 = replaceEscapeCodes(str1);
  }
  else
    assert(false);

  return true;
}

std::string
CExprParseImpl::
replaceEscapeCodes(const std::string &str)
{
  auto len = str.size();

  bool has_escape = false;

  for (uint i = 0; i < len - 1; ++i)
    if (str[i] == '\\') {
      has_escape = true;
      break;
    }

  if (! has_escape)
    return str;

  std::string str1;

  uint i = 0;

  while (i < len - 1) {
    if (str[i] != '\\') {
      str1 += str[i++];
      continue;
    }

    ++i;

    switch (str[i]) {
      case 'a': str1 += '\a'  ; break;
      case 'b': str1 += '\b'  ; break;
      case 'e': str1 += '\033'; break;
      case 'f': str1 += '\f'  ; break;
      case 'n': str1 += '\n'  ; break;
      case 'r': str1 += '\r'  ; break;
      case 't': str1 += '\t'  ; break;
      case 'v': str1 += '\v'  ; break;

      case 'x': {
        if (i < len - 1 && ::isxdigit(str[i + 1])) {
          int hex_value = 0;

          ++i;

          if      (::isdigit(str[i]))
            hex_value += (str[i] - '0');
          else if (::islower(str[i]))
            hex_value += (str[i] - 'a' + 10);
          else
            hex_value += (str[i] - 'A' + 10);

          if (i < len - 1 && ::isxdigit(str[i + 1])) {
            hex_value *= 16;

            ++i;

            if      (::isdigit(str[i]))
              hex_value += (str[i] - '0');
            else if (::islower(str[i]))
              hex_value += (str[i] - 'a' + 10);
            else
              hex_value += (str[i] - 'A' + 10);
          }

          str1 += char(hex_value);
        }
        else {
          str1 += '\\';
          str1 += str[i++];
        }

        break;
      }

      case '\\': str1 += '\\'; break;

      case '0': {
        if (i < len - 1 && CStrUtil::isodigit(str[i + 1])) {
          int oct_value = 0;

          ++i;

          oct_value += (str[i] - '0');

          if (i < len - 1 && CStrUtil::isodigit(str[i + 1])) {
            oct_value *= 8;

            ++i;

            oct_value += (str[i] - '0');
          }

          if (i < len - 1 && CStrUtil::isodigit(str[i + 1])) {
            oct_value *= 8;

            ++i;

            oct_value += (str[i] - '0');
          }

          str1 += char(oct_value);
        }
        else {
          str1 += '\\';
          str1 += str[i];
        }

        break;
      }

      case '\'': str1 += str[i]; break;
      case '\"': str1 += str[i]; break;

      default: str1 += '\\'; str1 += str[i]; break;
    }

    ++i;
  }

  if (i < len)
    str1 += str[i++];

  return str1;
}

CExprTokenBaseP
CExprParseImpl::
readOperator(const std::string &str, uint *i)
{
  auto id = skipOperator(str, i);

  if (id == CExprOpType::UNKNOWN)
    return CExprTokenBaseP();

  return createOperatorToken(id);
}

CExprOpType
CExprParseImpl::
skipOperator(const std::string &str, uint *i)
{
  auto id = CExprOpType::UNKNOWN;

  switch (str[*i]) {
    case '(':
      (*i)++;
      id = CExprOpType::OPEN_RBRACKET;
      break;
    case ')':
      (*i)++;
      id = CExprOpType::CLOSE_RBRACKET;
      break;
    case '!':
      (*i)++;

      if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::NOT_EQUAL;
      }
      else
        id = CExprOpType::LOGICAL_NOT;

      break;
    case '~':
      (*i)++;

      if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::APPROX_EQUAL;
      }
      else
        id = CExprOpType::BIT_NOT;

      break;
    case '*':
      (*i)++;

      if      (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::TIMES_EQUALS;
      }
      else if (*i < str.size() && str[*i] == '*') {
        (*i)++;
        id = CExprOpType::POWER;
      }
      else
        id = CExprOpType::TIMES;

      break;
    case '/':
      (*i)++;

      if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::DIVIDE_EQUALS;
      }
      else
        id = CExprOpType::DIVIDE;

      break;
    case '%':
      (*i)++;

      if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::MODULUS_EQUALS;
      }
      else
        id = CExprOpType::MODULUS;

      break;
    case '+':
      (*i)++;

      if      (*i < str.size() && str[*i] == '+') {
        (*i)++;
        id = CExprOpType::INCREMENT;
      }
      else if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::PLUS_EQUALS;
      }
      else
        id = CExprOpType::PLUS;

      break;
    case '-':
      (*i)++;

      if      (*i < str.size() && str[*i] == '-') {
        (*i)++;
        id = CExprOpType::DECREMENT;
      }
      else if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::MINUS_EQUALS;
      }
      else
        id = CExprOpType::MINUS;

      break;
    case '<':
      (*i)++;

      if      (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::LESS_EQUAL;
      }
      else if (*i < str.size() && str[*i] == '<') {
        (*i)++;

        if (*i < str.size() && str[*i] == '=') {
          (*i)++;
          id = CExprOpType::BIT_LSHIFT_EQUALS;
        }
        else
          id = CExprOpType::BIT_LSHIFT;
      }
      else
        id = CExprOpType::LESS;

      break;
    case '>':
      (*i)++;

      if      (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::GREATER_EQUAL;
      }
      else if (*i < str.size() && str[*i] == '>') {
        (*i)++;

        if (*i < str.size() && str[*i] == '=') {
          (*i)++;
          id = CExprOpType::BIT_RSHIFT_EQUALS;
        }
        else
          id = CExprOpType::BIT_RSHIFT;
      }
      else
        id = CExprOpType::GREATER;

      break;
    case '=':
      (*i)++;

      if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::EQUAL;
      }
      else
        id = CExprOpType::EQUALS;

      break;
    case '&':
      (*i)++;

      if      (*i < str.size() && str[*i] == '&') {
        (*i)++;
        id = CExprOpType::LOGICAL_AND;
      }
      else if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::BIT_XOR_EQUALS;
      }
      else
        id = CExprOpType::BIT_AND;

      break;
    case '^':
      (*i)++;

      if (*i < str.size() && str[*i + 1] == '=') {
        (*i)++;
        id = CExprOpType::BIT_XOR_EQUALS;
      }
      else
        id = CExprOpType::BIT_XOR;

      break;
    case '|':
      (*i)++;

      if      (*i < str.size() && str[*i] == '|') {
        (*i)++;
        id = CExprOpType::LOGICAL_OR;
      }
      else if (*i < str.size() && str[*i] == '=') {
        (*i)++;
        id = CExprOpType::BIT_OR_EQUALS;
      }
      else
        id = CExprOpType::BIT_OR;

      break;
    case '?':
      (*i)++;
      id = CExprOpType::QUESTION;
      break;
    case ':':
      (*i)++;
      id = CExprOpType::COLON;
      break;
    case ',':
      (*i)++;
      id = CExprOpType::COMMA;
      break;
    default:
      break;
  }

  return id;
}

CExprTokenBaseP
CExprParseImpl::
readIdentifier(const std::string &str, uint *i)
{
  std::string identifier;

  readIdentifierChars(str, i, identifier);

  return createIdentifierToken(identifier);
}

void
CExprParseImpl::
skipIdentifier(const std::string &str, uint *i)
{
  std::string identifier;

  readIdentifierChars(str, i, identifier);
}

void
CExprParseImpl::
readIdentifierChars(const std::string &str, uint *i, std::string &identifier)
{
  uint j = *i;

  while (*i < str.size() && (str[*i] == '_' || isalnum(str[*i])))
    (*i)++;

  identifier = str.substr(j, *i - j);
}

#if 0
CExprTokenBaseP
CExprParseImpl::
readUnknown(const std::string &str, uint *i)
{
  while (*i < str.size() && ! isspace(str[*i]))
    (*i)++;

  return createUnknownToken();
}

CExprTokenBaseP
CExprParseImpl::
createUnknownToken()
{
  return CExprTokenBaseP(CExprTokenMgrInst->createUnknownToken());
}
#endif

CExprTokenBaseP
CExprParseImpl::
createIdentifierToken(const std::string &identifier)
{
  return CExprTokenBaseP(CExprTokenMgrInst->createIdentifierToken(identifier));
}

CExprTokenBaseP
CExprParseImpl::
createOperatorToken(CExprOpType id)
{
  return CExprTokenBaseP(CExprTokenMgrInst->createOperatorToken(id));
}

CExprTokenBaseP
CExprParseImpl::
createIntegerToken(long integer)
{
  return CExprTokenBaseP(CExprTokenMgrInst->createIntegerToken(integer));
}

CExprTokenBaseP
CExprParseImpl::
createRealToken(double real)
{
  return CExprTokenBaseP(CExprTokenMgrInst->createRealToken(real));
}

CExprTokenBaseP
CExprParseImpl::
createStringToken(const std::string &str)
{
  return CExprTokenBaseP(CExprTokenMgrInst->createStringToken(str));
}
