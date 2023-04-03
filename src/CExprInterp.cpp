#include <CExprI.h>

#ifdef CEXPR_DEBUG
#  define DEBUG_ENTER(a) Trace(a)
#  define DEBUG_PRINT(a) CExprPrintTypeIToken(a)
#else
#  define DEBUG_ENTER(a)
#  define DEBUG_PRINT(a)
#endif

//------

class Trace {
 public:
  Trace(const std::string &name) : name_(name) {
    std::cerr << ">" << name_ << "\n";
  }

 ~Trace() {
    std::cerr << "<" << name_ << "\n";
  }

 private:
  std::string name_;
};

//------

class CExprInterpImpl {
 public:
  CExprInterpImpl(CExpr *expr) : expr_(expr) { }

  CExprITokenPtr interpStack(const CExprTokenStack &stack);

 private:
  CExprITokenPtr readExpression();
  CExprITokenPtr readAssignmentExpression();
  CExprITokenPtr readConditionalExpression();
  CExprITokenPtr readLogicalOrExpression();
  CExprITokenPtr readLogicalAndExpression();
  CExprITokenPtr readInclusiveOrExpression();
  CExprITokenPtr readExclusiveOrExpression();
  CExprITokenPtr readAndExpression();
  CExprITokenPtr readEqualityExpression();
  CExprITokenPtr readRelationalExpression();
  CExprITokenPtr readShiftExpression();
  CExprITokenPtr readAdditiveExpression();
  CExprITokenPtr readMultiplicativeExpression();
  CExprITokenPtr readPowerExpression();
  CExprITokenPtr readUnaryExpression();
  CExprITokenPtr readPostfixExpression();
  CExprITokenPtr readPrimaryExpression();
  CExprITokenPtr readArgumentExpressionList();
  CExprITokenPtr readITokenOfType(CExprITokenType);

  bool           isLastToken();
  void           addITokenToType(CExprITokenPtr, CExprITokenPtr);
  CExprITokenPtr collapseITokenToType(CExprITokenPtr, CExprITokenType);
  void           deleteCollapsedIToken(CExprITokenPtr, CExprITokenPtr);

  bool isOperatorIToken  (const CExprITokenPtr &itoken, CExprOpType type);
  bool isIdentifierIToken(const CExprITokenPtr &itoken);
  bool isIntegerIToken   (const CExprITokenPtr &itoken);
  bool isRealIToken      (const CExprITokenPtr &itoken);
  bool isStringIToken    (const CExprITokenPtr &itoken);

  void           stackIToken(const CExprITokenPtr &itoken);
  CExprITokenPtr unstackIToken();

  void printTrackBack(std::ostream &os);

#ifdef CEXPR_DEBUG
  void printTypeIToken(std::ostream &os, CExprITokenPtr);
  void printTypeIToken1(std::ostream &os, CExprITokenPtr);

  std::string tokenToString(CExprITokenPtr itoken);
  void        concatITokenString(std::string &, CExprITokenPtr);
#endif

 private:
  CExpr*           expr_ { 0 };
  CExprTokenStack  ptokenStack_;
  CExprITokenStack itokenStack_;
  CExprErrorData   errorData_;
};

//-----------

namespace CExprInterpUtil {
  const char *getTypeName(CExprITokenType itype, CExprTokenType type) {
    switch (itype) {
      case CExprITokenType::EXPRESSION               : return "expression";
      case CExprITokenType::ASSIGNMENT_EXPRESSION    : return "assignment_expression";
      case CExprITokenType::CONDITIONAL_EXPRESSION   : return "conditional_expression";
      case CExprITokenType::LOGICAL_OR_EXPRESSION    : return "logical_or_expression";
      case CExprITokenType::LOGICAL_AND_EXPRESSION   : return "logical_and_expression";
      case CExprITokenType::INCLUSIVE_OR_EXPRESSION  : return "inclusive_or_expression";
      case CExprITokenType::EXCLUSIVE_OR_EXPRESSION  : return "exclusive_or_expression";
      case CExprITokenType::AND_EXPRESSION           : return "and_expression";
      case CExprITokenType::EQUALITY_EXPRESSION      : return "equality_expression";
      case CExprITokenType::RELATIONAL_EXPRESSION    : return "relational_expression";
      case CExprITokenType::SHIFT_EXPRESSION         : return "shift_expression";
      case CExprITokenType::ADDITIVE_EXPRESSION      : return "additive_expression";
      case CExprITokenType::MULTIPLICATIVE_EXPRESSION: return "multiplicative_expression";
      case CExprITokenType::POWER_EXPRESSION         : return "power_expression";
      case CExprITokenType::UNARY_EXPRESSION         : return "unary_expression";
      case CExprITokenType::POSTFIX_EXPRESSION       : return "postfix_expression";
      case CExprITokenType::PRIMARY_EXPRESSION       : return "primary_expression";
      case CExprITokenType::ARGUMENT_EXPRESSION_LIST : return "argument_expression_list";
      case CExprITokenType::DUMMY_EXPRESSION         : return "dummy_expression";
      case CExprITokenType::TOKEN_TYPE: {
        switch (type) {
          case CExprTokenType::IDENTIFIER : return "identifier";
          case CExprTokenType::OPERATOR   : return "operator";
          case CExprTokenType::INTEGER    : return "integer";
          case CExprTokenType::REAL       : return "real";
          case CExprTokenType::STRING     : return "string";
          default                         : return "<-?->";
        }
      }
      default                             : return "<-?->";
    }
  }
}

//-----------

CExprInterp::
CExprInterp(CExpr *expr) :
 expr_(expr)
{
  impl_ = std::make_unique<CExprInterpImpl>(expr);
}

CExprInterp::
~CExprInterp()
{
}

CExprITokenPtr
CExprInterp::
interpPTokenStack(const CExprTokenStack &stack)
{
  return impl_->interpStack(stack);
}

//-----------

CExprITokenPtr
CExprInterpImpl::
interpStack(const CExprTokenStack &stack)
{
  errorData_.setLastError("");

  if (stack.getNumTokens() == 0)
    return CExprITokenPtr();

  ptokenStack_ = stack;

  itokenStack_.clear();

  auto itoken = readExpression();

  if (! isLastToken()) {
    stackIToken(itoken);

    printTrackBack(std::cerr);

    itoken = CExprITokenPtr();
  }

  itokenStack_.clear();

  return itoken;
}

/*
 * <expression>:= <assignment_expression>
 * <expression>:= <expression> , <assignment_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readExpression()
{
  DEBUG_ENTER("readExpression");

  auto itoken = readITokenOfType(CExprITokenType::EXPRESSION);

  if (! itoken) {
    auto itoken1 = readAssignmentExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::COMMA)) {
      auto itoken2 = readAssignmentExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else {
        errorData_.setLastError("Missing assignment expression after comma");
        break;
      }

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);

    if (errorData_.isError()) {
      stackIToken(itoken);

      itoken = CExprITokenPtr();
    }
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <assignment_expression>:= <conditional_expression>
 * <assignment_expression>:= <unary_expression>   = <assignment_expression>
 * <assignment_expression>:= <unary_expression>  *= <assignment_expression>
 * <assignment_expression>:= <unary_expression>  /= <assignment_expression>
 * <assignment_expression>:= <unary_expression>  %= <assignment_expression>
 * <assignment_expression>:= <unary_expression>  += <assignment_expression>
 * <assignment_expression>:= <unary_expression>  -= <assignment_expression>
 * <assignment_expression>:= <unary_expression> <<= <assignment_expression>
 * <assignment_expression>:= <unary_expression> >>= <assignment_expression>
 * <assignment_expression>:= <unary_expression>  &= <assignment_expression>
 * <assignment_expression>:= <unary_expression>  ^= <assignment_expression>
 * <assignment_expression>:= <unary_expression>  |= <assignment_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readAssignmentExpression()
{
  DEBUG_ENTER("readAssignmentExpression");

  auto itoken = readITokenOfType(CExprITokenType::ASSIGNMENT_EXPRESSION);

  if (itoken)
    return itoken;

  auto itoken1 = readConditionalExpression();

  if (itoken1) {
    auto itoken2 = collapseITokenToType(itoken1, CExprITokenType::UNARY_EXPRESSION);

    if (itoken2) {
      auto itoken3 = unstackIToken();

      if (isOperatorIToken(itoken3, CExprOpType::EQUALS           ) ||
          isOperatorIToken(itoken3, CExprOpType::TIMES_EQUALS     ) ||
          isOperatorIToken(itoken3, CExprOpType::DIVIDE_EQUALS    ) ||
          isOperatorIToken(itoken3, CExprOpType::MODULUS_EQUALS   ) ||
          isOperatorIToken(itoken3, CExprOpType::PLUS_EQUALS      ) ||
          isOperatorIToken(itoken3, CExprOpType::MINUS_EQUALS     ) ||
          isOperatorIToken(itoken3, CExprOpType::BIT_LSHIFT_EQUALS) ||
          isOperatorIToken(itoken3, CExprOpType::BIT_RSHIFT_EQUALS) ||
          isOperatorIToken(itoken3, CExprOpType::BIT_AND_EQUALS   ) ||
          isOperatorIToken(itoken3, CExprOpType::BIT_XOR_EQUALS   ) ||
          isOperatorIToken(itoken3, CExprOpType::BIT_OR_EQUALS    )) {
        auto itoken4 = readAssignmentExpression();

        if (itoken4) {
          deleteCollapsedIToken(itoken1, itoken2);

          itoken = CExprIToken::createIToken(CExprITokenType::ASSIGNMENT_EXPRESSION);

          addITokenToType(itoken2, itoken);
          addITokenToType(itoken3, itoken);
          addITokenToType(itoken4, itoken);
        }
        else
          stackIToken(itoken3);
      }
      else
        stackIToken(itoken3);
    }

    if (! itoken) {
      itoken = CExprIToken::createIToken(CExprITokenType::ASSIGNMENT_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }

    return itoken;
  }

  itoken1 = readUnaryExpression();

  if (! itoken1)
    return itoken;

  auto itoken2 = unstackIToken();

  if (! isOperatorIToken(itoken2, CExprOpType::EQUALS           ) &&
      ! isOperatorIToken(itoken2, CExprOpType::TIMES_EQUALS     ) &&
      ! isOperatorIToken(itoken2, CExprOpType::DIVIDE_EQUALS    ) &&
      ! isOperatorIToken(itoken2, CExprOpType::MODULUS_EQUALS   ) &&
      ! isOperatorIToken(itoken2, CExprOpType::PLUS_EQUALS      ) &&
      ! isOperatorIToken(itoken2, CExprOpType::MINUS_EQUALS     ) &&
      ! isOperatorIToken(itoken2, CExprOpType::BIT_LSHIFT_EQUALS) &&
      ! isOperatorIToken(itoken2, CExprOpType::BIT_RSHIFT_EQUALS) &&
      ! isOperatorIToken(itoken2, CExprOpType::BIT_AND_EQUALS   ) &&
      ! isOperatorIToken(itoken2, CExprOpType::BIT_XOR_EQUALS   ) &&
      ! isOperatorIToken(itoken2, CExprOpType::BIT_OR_EQUALS    )) {
    stackIToken(itoken2);
    stackIToken(itoken1);

    return itoken;
  }

  auto itoken3 = readAssignmentExpression();

  if (! itoken3) {
    stackIToken(itoken2);
    stackIToken(itoken1);

    return itoken;
  }

  itoken = CExprIToken::createIToken(CExprITokenType::ASSIGNMENT_EXPRESSION);

  addITokenToType(itoken1, itoken);
  addITokenToType(itoken2, itoken);
  addITokenToType(itoken3, itoken);

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <conditional_expression>:= <logical_or_expression>
 * <conditional_expression>:= <logical_or_expression> ? <expression> : <conditional_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readConditionalExpression()
{
  DEBUG_ENTER("readConditionalExpression");

  auto itoken = readITokenOfType(CExprITokenType::CONDITIONAL_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readLogicalOrExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::CONDITIONAL_EXPRESSION);

      addITokenToType(itoken1, itoken);

      auto itoken2 = unstackIToken();

      if (isOperatorIToken(itoken2, CExprOpType::QUESTION)) {
        auto itoken3 = readExpression();

        if (itoken3) {
          auto itoken4 = unstackIToken();

          if (isOperatorIToken(itoken4, CExprOpType::COLON)) {
            auto itoken5 = readConditionalExpression();

            if (itoken5) {
              addITokenToType(itoken2, itoken);
              addITokenToType(itoken3, itoken);
              addITokenToType(itoken4, itoken);
              addITokenToType(itoken5, itoken);
            }
            else {
              stackIToken(itoken4);
              stackIToken(itoken3);
              stackIToken(itoken2);
              stackIToken(itoken1);
            }
          }
          else {
            errorData_.setLastError("Missing colon for '?:'");

            stackIToken(itoken4);
            stackIToken(itoken3);
            stackIToken(itoken2);
            stackIToken(itoken1);
          }
        }
        else {
          stackIToken(itoken2);
          stackIToken(itoken1);
        }
      }
      else
        stackIToken(itoken2);
    }
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <logical_or_expression>:= <logical_and_expression>
 * <logical_or_expression>:= <logical_or_expression> || <logical_and_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readLogicalOrExpression()
{
  DEBUG_ENTER("readLogicalOrExpression");

  auto itoken = readITokenOfType(CExprITokenType::LOGICAL_OR_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readLogicalAndExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::LOGICAL_OR_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::LOGICAL_OR)) {
      auto itoken2 = readLogicalAndExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::LOGICAL_OR_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <logical_and_expression>:= <inclusive_or_expression>
 * <logical_and_expression>:= <logical_and_expression> && <inclusive_or_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readLogicalAndExpression()
{
  DEBUG_ENTER("readLogicalAndExpression");

  auto itoken = readITokenOfType(CExprITokenType::LOGICAL_AND_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readInclusiveOrExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::LOGICAL_AND_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::LOGICAL_AND)) {
      auto itoken2 = readInclusiveOrExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::LOGICAL_AND_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <inclusive_or_expression>:= <exclusive_or_expression>
 * <inclusive_or_expression>:= <inclusive_or_expression> | <exclusive_or_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readInclusiveOrExpression()
{
  DEBUG_ENTER("readInclusiveOrExpression");

  auto itoken = readITokenOfType(CExprITokenType::INCLUSIVE_OR_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readExclusiveOrExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::INCLUSIVE_OR_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::BIT_OR)) {
      auto itoken2 = readExclusiveOrExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::INCLUSIVE_OR_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <exclusive_or_expression>:= <and_expression>
 * <exclusive_or_expression>:= <exclusive_or_expression> ^ <and_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readExclusiveOrExpression()
{
  DEBUG_ENTER("readExclusiveOrExpression");

  auto itoken = readITokenOfType(CExprITokenType::EXCLUSIVE_OR_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readAndExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::EXCLUSIVE_OR_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::BIT_XOR)) {
      auto itoken2 = readAndExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::EXCLUSIVE_OR_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <and_expression>:= <equality_expression>
 * <and_expression>:= <and_expression> & <equality_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readAndExpression()
{
  DEBUG_ENTER("readAndExpression");

  auto itoken = readITokenOfType(CExprITokenType::AND_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readEqualityExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::AND_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::BIT_AND)) {
      auto itoken2 = readEqualityExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::AND_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <equality_expression>:= <relational_expression>
 * <equality_expression>:= <equality_expression> == <relational_expression>
 * <equality_expression>:= <equality_expression> != <relational_expression>
 * <equality_expression>:= <equality_expression> ~= <relational_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readEqualityExpression()
{
  DEBUG_ENTER("readEqualityExpression");

  auto itoken = readITokenOfType(CExprITokenType::EQUALITY_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readRelationalExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::EQUALITY_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::EQUAL       ) ||
           isOperatorIToken(itoken1, CExprOpType::NOT_EQUAL   ) ||
           isOperatorIToken(itoken1, CExprOpType::APPROX_EQUAL)) {
      auto itoken2 = readRelationalExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::EQUALITY_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <relational_expression>:= <shift_expression>
 * <relational_expression>:= <relational_expression> <  <shift_expression>
 * <relational_expression>:= <relational_expression> >  <shift_expression>
 * <relational_expression>:= <relational_expression> <= <shift_expression>
 * <relational_expression>:= <relational_expression> >= <shift_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readRelationalExpression()
{
  DEBUG_ENTER("readRelationalExpression");

  auto itoken = readITokenOfType(CExprITokenType::RELATIONAL_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readShiftExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::RELATIONAL_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::LESS         ) ||
           isOperatorIToken(itoken1, CExprOpType::GREATER      ) ||
           isOperatorIToken(itoken1, CExprOpType::LESS_EQUAL   ) ||
           isOperatorIToken(itoken1, CExprOpType::GREATER_EQUAL)) {
      auto itoken2 = readShiftExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::RELATIONAL_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <shift_expression>:= <additive_expression>
 * <shift_expression>:= <shift_expression> << <additive_expression>
 * <shift_expression>:= <shift_expression> >> <additive_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readShiftExpression()
{
  DEBUG_ENTER("readShiftExpression");

  auto itoken = readITokenOfType(CExprITokenType::SHIFT_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readAdditiveExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::SHIFT_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::BIT_LSHIFT) ||
           isOperatorIToken(itoken1, CExprOpType::BIT_RSHIFT)) {
      auto itoken2 = readAdditiveExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::SHIFT_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <additive_expression>:= <multiplicative_expression>
 * <additive_expression>:= <additive_expression> + <multiplicative_expression>
 * <additive_expression>:= <additive_expression> - <multiplicative_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readAdditiveExpression()
{
  DEBUG_ENTER("readAdditiveExpression");

  auto itoken = readITokenOfType(CExprITokenType::ADDITIVE_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readMultiplicativeExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::ADDITIVE_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::PLUS ) ||
           isOperatorIToken(itoken1, CExprOpType::MINUS)) {
      auto itoken2 = readMultiplicativeExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::ADDITIVE_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else {
        if      (isOperatorIToken(itoken1, CExprOpType::PLUS))
          errorData_.setLastError("Missing right expression for '+'");
        else if (isOperatorIToken(itoken1, CExprOpType::MINUS))
          errorData_.setLastError("Missing right expression for '-'");

        stackIToken(itoken1);
        stackIToken(itoken);

        return CExprITokenPtr();
      }

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <multiplicative_expression>:= <unary_expression>
 * <multiplicative_expression>:= <multiplicative_expression> * <unary_expression>
 * <multiplicative_expression>:= <multiplicative_expression> / <unary_expression>
 * <multiplicative_expression>:= <multiplicative_expression> % <unary_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readMultiplicativeExpression()
{
  DEBUG_ENTER("readMultiplicativeExpression");

  auto itoken = readITokenOfType(CExprITokenType::MULTIPLICATIVE_EXPRESSION);

  if (! itoken) {
    auto itoken1 = readUnaryExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::MULTIPLICATIVE_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::TIMES  ) ||
           isOperatorIToken(itoken1, CExprOpType::DIVIDE ) ||
           isOperatorIToken(itoken1, CExprOpType::MODULUS)) {
      auto itoken2 = readUnaryExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::MULTIPLICATIVE_EXPRESSION);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <unary_expression>:= <power_expression>
 * <unary_expression>:= ++ <unary_expression>
 * <unary_expression>:= -- <unary_expression>
 * <unary_expression>:= +  <unary_expression>
 * <unary_expression>:= -  <unary_expression>
 * <unary_expression>:= ~  <unary_expression>
 * <unary_expression>:= !  <unary_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readUnaryExpression()
{
  DEBUG_ENTER("readUnaryExpression");

  auto itoken = readITokenOfType(CExprITokenType::UNARY_EXPRESSION);

  if (! itoken) {
    auto itoken1 = unstackIToken();

    if (isOperatorIToken(itoken1, CExprOpType::INCREMENT  ) ||
        isOperatorIToken(itoken1, CExprOpType::DECREMENT  ) ||
        isOperatorIToken(itoken1, CExprOpType::PLUS       ) ||
        isOperatorIToken(itoken1, CExprOpType::MINUS      ) ||
        isOperatorIToken(itoken1, CExprOpType::BIT_NOT    ) ||
        isOperatorIToken(itoken1, CExprOpType::LOGICAL_NOT)) {
      auto itoken2 = readUnaryExpression();

      if (itoken2) {
        itoken = CExprIToken::createIToken(CExprITokenType::UNARY_EXPRESSION);

        addITokenToType(itoken1, itoken);
        addITokenToType(itoken2, itoken);

        return itoken;
      }
    }

    stackIToken(itoken1);

    itoken1 = readPowerExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::UNARY_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <power_expression>:= <postfix_expression>
 * <power_expression>:= <postfix_expression> ** <power_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readPowerExpression()
{
  DEBUG_ENTER("readPowerExpression");

  auto itoken = readITokenOfType(CExprITokenType::POWER_EXPRESSION);

  if (itoken)
    return itoken;

  auto itoken1 = readPostfixExpression();

  if (! itoken1)
    return itoken;

  auto itoken2 = unstackIToken();

  if (isOperatorIToken(itoken2, CExprOpType::POWER)) {
    auto itoken3 = readPowerExpression();

    if (itoken3) {
      itoken = CExprIToken::createIToken(CExprITokenType::POWER_EXPRESSION);

      addITokenToType(itoken1, itoken);
      addITokenToType(itoken2, itoken);
      addITokenToType(itoken3, itoken);
    }
    else {
      stackIToken(itoken2);
      stackIToken(itoken1);
    }
  }
  else {
    stackIToken(itoken2);

    itoken = CExprIToken::createIToken(CExprITokenType::POWER_EXPRESSION);

    addITokenToType(itoken1, itoken);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <postfix_expression>:= <primary_expression>
 * <postfix_expression>:= <identifier> ( <argument_expression_list>(opt) )
 * <postfix_expression>:= <postfix_expression> ++
 * <postfix_expression>:= <postfix_expression> --
 */

CExprITokenPtr
CExprInterpImpl::
readPostfixExpression()
{
  DEBUG_ENTER("readPostfixExpression");

  auto itoken = readITokenOfType(CExprITokenType::POSTFIX_EXPRESSION);

  if (! itoken) {
    auto itoken1 = unstackIToken();

    if (isIdentifierIToken(itoken1)) {
      auto itoken2 = unstackIToken();

      if (isOperatorIToken(itoken2, CExprOpType::OPEN_RBRACKET)) {
        auto itoken3 = readArgumentExpressionList();

        if (itoken3 || ! errorData_.isError()) {
          auto itoken4 = unstackIToken();

          if (isOperatorIToken(itoken4, CExprOpType::CLOSE_RBRACKET)) {
            itoken = CExprIToken::createIToken(CExprITokenType::POSTFIX_EXPRESSION);

            addITokenToType(itoken1, itoken);
            addITokenToType(itoken2, itoken);
            addITokenToType(itoken3, itoken);
            addITokenToType(itoken4, itoken);
          }
          else {
            errorData_.setLastError("Missing close round bracket");

            stackIToken(itoken4);
            stackIToken(itoken3);
            stackIToken(itoken2);
            stackIToken(itoken1);
          }
        }
      }
      else {
        stackIToken(itoken2);
        stackIToken(itoken1);
      }
    }
    else
      stackIToken(itoken1);
  }

  if (! itoken) {
    auto itoken1 = readPrimaryExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::POSTFIX_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::INCREMENT) ||
           isOperatorIToken(itoken1, CExprOpType::DECREMENT)) {
      auto itoken2 = CExprIToken::createIToken(CExprITokenType::POSTFIX_EXPRESSION);

      addITokenToType(itoken , itoken2);
      addITokenToType(itoken1, itoken2);

      itoken = itoken2;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <primary_expression>:= <integer>
 * <primary_expression>:= <real>
 * <primary_expression>:= <string>
 * <primary_expression>:= <identifier>
 * <primary_expression>:= ( <expression> )
 */

CExprITokenPtr
CExprInterpImpl::
readPrimaryExpression()
{
  DEBUG_ENTER("readPrimaryExpression");

  auto itoken = readITokenOfType(CExprITokenType::PRIMARY_EXPRESSION);

  if (! itoken) {
    auto itoken1 = unstackIToken();

    if      (isIntegerIToken(itoken1) || isStringIToken(itoken1) || isRealIToken(itoken1)) {
      itoken = CExprIToken::createIToken(CExprITokenType::PRIMARY_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
    else if (isIdentifierIToken(itoken1)) {
      itoken = CExprIToken::createIToken(CExprITokenType::PRIMARY_EXPRESSION);

      addITokenToType(itoken1, itoken);
    }
    else if (isOperatorIToken(itoken1, CExprOpType::OPEN_RBRACKET)) {
      auto itoken2 = readExpression();

      if (itoken2) {
        auto itoken3 = unstackIToken();

        if (isOperatorIToken(itoken3, CExprOpType::CLOSE_RBRACKET)) {
          itoken = CExprIToken::createIToken(CExprITokenType::PRIMARY_EXPRESSION);

          addITokenToType(itoken1, itoken);
          addITokenToType(itoken2, itoken);
          addITokenToType(itoken3, itoken);
        }
        else {
          errorData_.setLastError("Missing close round bracket");

          stackIToken(itoken3);
          stackIToken(itoken2);
          stackIToken(itoken1);
        }
      }
      else {
        errorData_.setLastError("Missing expression after open round bracket");
        stackIToken(itoken1);
      }
    }
    else
      stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

/*
 * <argument_expression_list>:= <assignment_expression>
 * <argument_expression_list>:= <argument_expression_list> , <assignment_expression>
 */

CExprITokenPtr
CExprInterpImpl::
readArgumentExpressionList()
{
  DEBUG_ENTER("readArgumentExpressionList");

  auto itoken = readITokenOfType(CExprITokenType::ARGUMENT_EXPRESSION_LIST);

  if (! itoken) {
    auto itoken1 = readAssignmentExpression();

    if (itoken1) {
      itoken = CExprIToken::createIToken(CExprITokenType::ARGUMENT_EXPRESSION_LIST);

      addITokenToType(itoken1, itoken);
    }
  }

  if (itoken) {
    auto itoken1 = unstackIToken();

    while (isOperatorIToken(itoken1, CExprOpType::COMMA)) {
      auto itoken2 = readAssignmentExpression();

      if (itoken2) {
        auto itoken3 = CExprIToken::createIToken(CExprITokenType::ARGUMENT_EXPRESSION_LIST);

        addITokenToType(itoken , itoken3);
        addITokenToType(itoken1, itoken3);
        addITokenToType(itoken2, itoken3);

        itoken = itoken3;
      }
      else
        break;

      itoken1 = unstackIToken();
    }

    stackIToken(itoken1);
  }

  DEBUG_PRINT(itoken);

  return itoken;
}

CExprITokenPtr
CExprInterpImpl::
readITokenOfType(CExprITokenType type)
{
  auto itoken = unstackIToken();

  if (itoken) {
    if (itoken->getIType() == type)
      return itoken;

    stackIToken(itoken);
  }

  return CExprITokenPtr();
}

bool
CExprInterpImpl::
isLastToken()
{
  auto itoken = unstackIToken();

  if (! itoken)
    return true;

  stackIToken(itoken);

  return false;
}

void
CExprInterpImpl::
addITokenToType(CExprITokenPtr child, CExprITokenPtr itoken)
{
  if (! itoken || ! child)
    return;

  itoken->addChild(child);
}

CExprITokenPtr
CExprInterpImpl::
collapseITokenToType(CExprITokenPtr itoken, CExprITokenType type)
{
  auto itoken1 = itoken;

  while (itoken1) {
    if (itoken1->getIType() == type)
      break;

    if (itoken1->getNumChildren() == 1)
      itoken1 = itoken1->getChild(0);
    else
      itoken1 = CExprITokenPtr();
  }

  return itoken1;
}

void
CExprInterpImpl::
deleteCollapsedIToken(CExprITokenPtr itoken, CExprITokenPtr itoken1)
{
  auto itoken2 = itoken;

  while (itoken2 != itoken1) {
    auto itoken3 = itoken2->getChild(0);

    itoken2->removeAllChildren();

    itoken2 = itoken3;
  }
}

bool
CExprInterpImpl::
isOperatorIToken(const CExprITokenPtr &itoken, CExprOpType id)
{
  if (! itoken)
    return false;

  if (itoken->getType() != CExprTokenType::OPERATOR)
    return false;

  return (itoken->getOperator() == id);
}

bool
CExprInterpImpl::
isIdentifierIToken(const CExprITokenPtr &itoken)
{
  if (! itoken)
    return false;

  return (itoken->getType() == CExprTokenType::IDENTIFIER);
}

bool
CExprInterpImpl::
isIntegerIToken(const CExprITokenPtr &itoken)
{
  if (! itoken)
    return false;

  return (itoken->getType() == CExprTokenType::INTEGER);
}

bool
CExprInterpImpl::
isRealIToken(const CExprITokenPtr &itoken)
{
  if (! itoken)
    return false;

  return (itoken->getType() == CExprTokenType::REAL);
}

bool
CExprInterpImpl::
isStringIToken(const CExprITokenPtr &itoken)
{
  if (! itoken)
    return false;

  return (itoken->getType() == CExprTokenType::STRING);
}

void
CExprInterpImpl::
stackIToken(const CExprITokenPtr &itoken)
{
  if (! itoken)
    return;

  itokenStack_.addToken(itoken);
}

CExprITokenPtr
CExprInterpImpl::
unstackIToken()
{
  auto itoken = itokenStack_.pop_back();

  if (itoken)
    return itoken;

  if (ptokenStack_.getNumTokens() == 0)
    return CExprITokenPtr();

  auto ptoken = ptokenStack_.pop_front();

  return CExprIToken::createIToken(ptoken);
}

void
CExprInterpImpl::
printTrackBack(std::ostream &os)
{
  if (expr_->getQuiet())
    return;

  std::string error_message = errorData_.getLastError();

  if (error_message != "")
    expr_->errorMsg(error_message);
  else
    expr_->errorMsg("Syntax Error");

  uint size = itokenStack_.getNumTokens();

  for (uint i = size - 1; i >= 1; --i) {
    auto itoken = itokenStack_.getToken(i);

    if (i < size)
      os << " ";

    os << *itoken;
  }

  auto itoken = itokenStack_.getToken(0);

  if (itoken) {
    if (size > 1)
      os << " ";

    os << ">>" << *itoken << "<<";
  }

  size = ptokenStack_.getNumTokens();

  for (uint i = 0; i < size; ++i) {
    auto ptoken = ptokenStack_.getToken(i);

    os << " " << ptoken;
  }

  os << "\n";
}

#ifdef CEXPR_DEBUG
void
CExprInterpImpl::
printTypeIToken(std::ostream &os, CExprITokenPtr itoken)
{
  if (! itoken)
    return;

  if (! expr_->getDebug())
    return;

  printTypeIToken1(os, itoken);

  os << "\n";
}

void
CExprInterpImpl::
printTypeIToken1(std::ostream &os, CExprITokenPtr itoken)
{
  os << "<" << CExprInterpUtil::getTypeName(itoken->getIType(), itoken->getType()) << ">";

  itoken->print(os, false);

  os << " ";

  uint num_children = itoken->getNumChildren();

  for (uint i = 0; i < num_children; i++)
    printTypeIToken1(os, itoken->getChild(i));
}

std::string
CExprInterpImpl::
iTokenToString(CExprITokenPtr itoken)
{
  std::string str = "";

  concatITokenString(str, itoken);

  return str;
}

void
CExprInterpImpl::
concatITokenString(std::string &str, CExprITokenPtr itoken)
{
  if (! itoken)
    return;

  if (itoken->getType() == CExprTokenType::STRING) {
    char c = str[str.size() - 1];

    if (c != '\0')
      str += " ";

    std::string str2 = itoken->getString();
    std::string str1 = CStrUtil::insertEscapeCodes(str2);

    str += "\"";

    if (str1 != "")
      str += str1;
    else
      str += str2;

    str += "\"";

    uint num_children = itoken->getNumChildren();

    for (uint i = 0; i < num_children; i++)
      concatITokenString(str, itoken->getChild(i));

    return;
  }

  std::string str2 = "";

  bool add_space = true;

  switch (itoken->getType()) {
    case CExprTokenType::IDENTIFIER:
      str2 = itoken->getIdentifier();

      break;
    case CExprTokenType::OPERATOR:
      str2 = itoken->getOperator()->text;

      if (str2[0] == '.' || str2[0] == ',' || str2[0] == ';' || str2[0] == '[' || str2[0] == ']')
        add_space = false;

      break;
    case CExprTokenType::INTEGER:
      str2 = CStrUtil::toString(itoken->getInteger());

      break;
    case CExprTokenType::REAL:
      str2 = CStrUtil::toString(itoken->getReal());

      break;
    default:
      assert(false);
      break;
  }

  if (str2 != "") {
    char c = str[str.size() - 1];

    if (c == '\0' || c == '.')
      add_space = false;

    if (add_space)
      str += " ";

    str += str2;
  }

  uint num_children = itoken->getNumChildren();

  for (uint i = 0; i < num_children; i++)
    concatITokenString(str, itoken->getChild(i));
}
#endif

//------

CExprIToken::
CExprIToken(CExprITokenType itype) :
 itype_(itype), base_()
{
}

CExprIToken::
CExprIToken(const CExprTokenBaseP &base) :
 itype_(CExprITokenType::TOKEN_TYPE), base_(base)
{
}

CExprTokenType
CExprIToken::
getType() const
{
  if (base_)
    return base_->type();

  return CExprTokenType::NONE;
}

#if 0
CExprIToken *
CExprIToken::
dup() const
{
  auto *itoken = new CExprIToken(itype_);

  if (base_)
    itoken->base_ = base_->dup();

  uint num_children = children_.size();

  for (uint i = 0; i < num_children; ++i)
    itoken->children_.push_back(children_[i]);

  return itoken;
}
#endif

uint
CExprIToken::
countITokenChildrenOfType(CExprITokenType type)
{
  uint num_children1 = 0;

  uint num_children = getNumChildren();

  for (uint i = 0; i < num_children; i++) {
    if      (getChild(i)->getIType() == type)
      ++num_children1;
    else if (getChild(i)->getNumChildren() > 0) {
      num_children1 += getChild(i)->countITokenChildrenOfType(type);
    }
  }

  return num_children1;
}

void
CExprIToken::
print(std::ostream &os, bool children) const
{
  if (base_)
    base_->print(os);
  else
    os << "<" << CExprInterpUtil::getTypeName(getIType(), CExprTokenType::UNKNOWN) << ">";

  if (children) {
    auto num_children = children_.size();

    if (num_children > 0) {
      os << " [";

      for (uint i = 0; i < num_children; i++) {
        if (i > 0) os << " ";

        children_[i]->print(os);
      }

      os << "]";
    }
  }
}

//------

void
CExprITokenStack::
print(std::ostream &os) const
{
  auto len = stack_.size();

  for (uint i = 0; i < len; ++i) {
    if (i > 0) os << " ";

    stack_[i]->print(os);
  }
}
