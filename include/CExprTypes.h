#ifndef CExprTypes_H
#define CExprTypes_H

#include <vector>
#include <memory>

enum class CExprOpType {
  UNKNOWN           = -1,
  OPEN_RBRACKET     = 0,
  CLOSE_RBRACKET    = 1,
  LOGICAL_NOT       = 2,
  BIT_NOT           = 3,
  INCREMENT         = 4,
  DECREMENT         = 5,
  UNARY_PLUS        = 6,
  UNARY_MINUS       = 7,
  POWER             = 8,
  TIMES             = 9,
  DIVIDE            = 10,
  MODULUS           = 11,
  PLUS              = 12,
  MINUS             = 13,
  BIT_LSHIFT        = 14,
  BIT_RSHIFT        = 15,
  LESS              = 16,
  LESS_EQUAL        = 17,
  GREATER           = 18,
  GREATER_EQUAL     = 19,
  EQUAL             = 20,
  NOT_EQUAL         = 21,
  APPROX_EQUAL      = 22,
  BIT_AND           = 23,
  BIT_XOR           = 24,
  BIT_OR            = 25,
  LOGICAL_AND       = 26,
  LOGICAL_OR        = 27,
  QUESTION          = 28,
  COLON             = 29,
  EQUALS            = 30,
  PLUS_EQUALS       = 31,
  MINUS_EQUALS      = 32,
  TIMES_EQUALS      = 33,
  DIVIDE_EQUALS     = 34,
  MODULUS_EQUALS    = 35,
  BIT_AND_EQUALS    = 36,
  BIT_XOR_EQUALS    = 37,
  BIT_OR_EQUALS     = 38,
  BIT_LSHIFT_EQUALS = 39,
  BIT_RSHIFT_EQUALS = 40,
  COMMA             = 41,
  START_BLOCK       = 42,
  END_BLOCK         = 43
};

enum class CExprValueType {
  NONE    = 0,
  BOOLEAN = (1<<0),
  INTEGER = (1<<1),
  REAL    = (1<<2),
  STRING  = (1<<3),
  NUL     = (1<<4),

  ANY = (BOOLEAN | INTEGER | REAL | STRING)
};

enum class CExprTokenType {
  UNKNOWN = -1,

  NONE = 0,

  IDENTIFIER = 1,
  OPERATOR   = 2,
  INTEGER    = 4,
  REAL       = 5,
  STRING     = 6,
  COMPLEX    = 7,
  FUNCTION   = 8,
  VALUE      = 9,
  BLOCK      = 10
};

enum class CExprITokenType {
  NONE                      = 0,
  TOKEN_TYPE                = 100,
  EXPRESSION                = 101,
  ASSIGNMENT_EXPRESSION     = 102,
  CONDITIONAL_EXPRESSION    = 103,
  LOGICAL_OR_EXPRESSION     = 104,
  LOGICAL_AND_EXPRESSION    = 105,
  INCLUSIVE_OR_EXPRESSION   = 106,
  EXCLUSIVE_OR_EXPRESSION   = 107,
  AND_EXPRESSION            = 108,
  EQUALITY_EXPRESSION       = 109,
  RELATIONAL_EXPRESSION     = 110,
  SHIFT_EXPRESSION          = 111,
  ADDITIVE_EXPRESSION       = 112,
  MULTIPLICATIVE_EXPRESSION = 113,
  POWER_EXPRESSION          = 114,
  UNARY_EXPRESSION          = 115,
  POSTFIX_EXPRESSION        = 116,
  PRIMARY_EXPRESSION        = 117,
  ARGUMENT_EXPRESSION_LIST  = 118,
  DUMMY_EXPRESSION          = 119,
};

//------

class CExpr;
class CExprValue;
class CExprIToken;
class CExprVariable;
class CExprFunction;

using CExprValuePtr    = std::shared_ptr<CExprValue>;
using CExprITokenPtr   = std::shared_ptr<CExprIToken>;
using CExprVariablePtr = std::shared_ptr<CExprVariable>;
using CExprFunctionPtr = std::shared_ptr<CExprFunction>;

using CExprValueArray = std::vector<CExprValuePtr>;

using CExprFunctionProc = CExprValuePtr (*)(CExpr *expr, const CExprValueArray &values);

#endif
