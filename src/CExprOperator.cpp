#include <CExprI.h>
#include <cstring>

/*
 *   Operator Token        | Precedence   | Associativity
 * ------------------------+--------------+--------------
 *                         |              |
 *  ( )                    |  16          |  L -> R
 *  **                     |  15          |  R -> L
 *  !   ~   ++  --  +   -  |  14 (unary)  |  R -> L
 *  *   /   %              |  13          |  L -> R
 *  +   -                  |  12          |  L -> R
 *  <<  >>                 |  11          |  L -> R
 *  <   <=  >   >=         |  10          |  L -> R
 *  ==  !=  ~=             |   9          |  L -> R
 *  &                      |   8          |  L -> R
 *  ^                      |   7          |  L -> R
 *  |                      |   6          |  L -> R
 *  &&                     |   5          |  L -> R
 *  ||                     |   4          |  L -> R
 *  ?:                     |   3          |  R -> L
 *  =   +=  -=   *=  /= %= |   2          |  R -> L
 *  &=  ^=  |=  <<= >>=    |   2          |  R -> L
 *  ,                      |   1          |  R -> L
 *
 */

struct CExprOperatorData {
  CExprOpType  type;
  const char  *name;
};

static CExprOperatorData
operator_data[] = {
  { CExprOpType::OPEN_RBRACKET    , "("  , },
  { CExprOpType::CLOSE_RBRACKET   , ")"  , },
  { CExprOpType::LOGICAL_NOT      , "!"  , },
  { CExprOpType::BIT_NOT          , "~"  , },
  { CExprOpType::INCREMENT        , "++" , },
  { CExprOpType::DECREMENT        , "--" , },
  { CExprOpType::UNARY_PLUS       , "+"  , },
  { CExprOpType::UNARY_MINUS      , "-"  , },
  { CExprOpType::POWER            , "**" , },
  { CExprOpType::TIMES            , "*"  , },
  { CExprOpType::DIVIDE           , "/"  , },
  { CExprOpType::MODULUS          , "%"  , },
  { CExprOpType::PLUS             , "+"  , },
  { CExprOpType::MINUS            , "-"  , },
  { CExprOpType::BIT_LSHIFT       , "<<" , },
  { CExprOpType::BIT_RSHIFT       , ">>" , },
  { CExprOpType::LESS             , "<"  , },
  { CExprOpType::LESS_EQUAL       , "<=" , },
  { CExprOpType::GREATER          , ">"  , },
  { CExprOpType::GREATER_EQUAL    , ">=" , },
  { CExprOpType::EQUAL            , "==" , },
  { CExprOpType::NOT_EQUAL        , "!=" , },
  { CExprOpType::APPROX_EQUAL     , "~=" , },
  { CExprOpType::BIT_AND          , "&"  , },
  { CExprOpType::BIT_XOR          , "^"  , },
  { CExprOpType::BIT_OR           , "|"  , },
  { CExprOpType::LOGICAL_AND      , "&&" , },
  { CExprOpType::LOGICAL_OR       , "||" , },
  { CExprOpType::QUESTION         , "?"  , },
  { CExprOpType::COLON            , ":"  , },
  { CExprOpType::EQUALS           , "="  , },
  { CExprOpType::PLUS_EQUALS      , "+=" , },
  { CExprOpType::MINUS_EQUALS     , "-=" , },
  { CExprOpType::TIMES_EQUALS     , "*=" , },
  { CExprOpType::DIVIDE_EQUALS    , "/=" , },
  { CExprOpType::MODULUS_EQUALS   , "%=" , },
  { CExprOpType::BIT_AND_EQUALS   , "&=" , },
  { CExprOpType::BIT_XOR_EQUALS   , "^=" , },
  { CExprOpType::BIT_OR_EQUALS    , "|=" , },
  { CExprOpType::BIT_LSHIFT_EQUALS, "<<=", },
  { CExprOpType::BIT_RSHIFT_EQUALS, ">>=", },
  { CExprOpType::COMMA            , ","  , },
  { CExprOpType::START_BLOCK      , "{"  , },
  { CExprOpType::END_BLOCK        , "}"  , },
  { CExprOpType::UNKNOWN          , 0    , }
};

//------

CExprOperatorMgr::
CExprOperatorMgr(CExpr *expr) :
 expr_(expr)
{
  for (uint i = 0; operator_data[i].name != 0; ++i)
    operator_map_[operator_data[i].type] =
      CExprOperatorPtr(new CExprOperator(operator_data[i].type, operator_data[i].name));
}

CExprOperatorPtr
CExprOperatorMgr::
getOperator(CExprOpType type) const
{
  auto p = operator_map_.find(type);

  if (p != operator_map_.end())
    return (*p).second;

  return CExprOperatorPtr();
}

//------

CExprOperator::
CExprOperator(CExprOpType type, const std::string &name) :
 type_(type), name_(name)
{
}

bool
CExprOperator::
isOperatorChar(char c)
{
  static char operator_chars [] = "()!~*/%+-<>=!&^|?:,";

  return (strchr(operator_chars, c) != 0);
}
