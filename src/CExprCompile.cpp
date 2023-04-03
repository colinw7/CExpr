#include <CExprI.h>

class CExprCompileImpl {
 public:
  CExprCompileImpl(CExpr *expr) : expr_(expr) { }

  CExprTokenStack compileIToken(CExprITokenPtr itoken);

  bool hasFunction(const std::string &name) const;

 private:
  void compileIToken1                 (CExprITokenPtr itoken);
  void compileExpression              (CExprITokenPtr itoken);
  void compileAssignmentExpression    (CExprITokenPtr itoken);
  void compileConditionalExpression   (CExprITokenPtr itoken);
  void compileLogicalOrExpression     (CExprITokenPtr itoken);
  void compileLogicalAndExpression    (CExprITokenPtr itoken);
  void compileInclusiveOrExpression   (CExprITokenPtr itoken);
  void compileExclusiveOrExpression   (CExprITokenPtr itoken);
  void compileAndExpression           (CExprITokenPtr itoken);
  void compileEqualityExpression      (CExprITokenPtr itoken);
  void compileRelationalExpression    (CExprITokenPtr itoken);
  void compileShiftExpression         (CExprITokenPtr itoken);
  void compileAdditiveExpression      (CExprITokenPtr itoken);
  void compileMultiplicativeExpression(CExprITokenPtr itoken);
  void compilePowerExpression         (CExprITokenPtr itoken);
  void compileUnaryExpression         (CExprITokenPtr itoken);
  void compilePostfixExpression       (CExprITokenPtr itoken);
  void compilePrimaryExpression       (CExprITokenPtr itoken);
  void compileArgumentExpressionList  (CExprITokenPtr itoken);

  void compileIdentifier(CExprITokenPtr itoken);
  void compileOperator  (CExprITokenPtr itoken);
  void compileInteger   (CExprITokenPtr itoken);
  void compileReal      (CExprITokenPtr itoken);
  void compileString    (CExprITokenPtr itoken);
  void compileValue     (CExprITokenPtr itoken);
#if 0
  void compileITokenChildren(CExprITokenPtr itoken);
#endif

  void stackFunction  (CExprFunctionPtr function);
  void stackDummyValue();
  void stackCToken    (const CExprTokenBaseP &base);

 private:
  CExpr*          expr_ { 0 };
  CExprTokenStack tokenStack_;
  CExprErrorData  errorData_;
};

//------

CExprCompile::
CExprCompile(CExpr *expr) :
 expr_(expr)
{
  impl_ = std::make_unique<CExprCompileImpl>(expr);
}

CExprCompile::
~CExprCompile()
{
}

CExprTokenStack
CExprCompile::
compileIToken(CExprITokenPtr itoken)
{
  return impl_->compileIToken(itoken);
}

bool
CExprCompile::
hasFunction(const std::string &name) const
{
  return impl_->hasFunction(name);
}

//------

CExprTokenStack
CExprCompileImpl::
compileIToken(CExprITokenPtr itoken)
{
  tokenStack_.clear();

  if (! itoken)
    return tokenStack_;

  errorData_.setLastError("");

  compileIToken1(itoken);

  if (errorData_.isError()) {
    expr_->errorMsg(errorData_.getLastError());
    return CExprTokenStack();
  }

  return tokenStack_;
}

void
CExprCompileImpl::
compileIToken1(CExprITokenPtr itoken)
{
  switch (itoken->getIType()) {
    case CExprITokenType::EXPRESSION:
      compileExpression(itoken);

      break;
    case CExprITokenType::ASSIGNMENT_EXPRESSION:
      compileAssignmentExpression(itoken);

      break;
    case CExprITokenType::CONDITIONAL_EXPRESSION:
      compileConditionalExpression(itoken);

      break;
    case CExprITokenType::LOGICAL_OR_EXPRESSION:
      compileLogicalOrExpression(itoken);

      break;
    case CExprITokenType::LOGICAL_AND_EXPRESSION:
      compileLogicalAndExpression(itoken);

      break;
    case CExprITokenType::INCLUSIVE_OR_EXPRESSION:
      compileInclusiveOrExpression(itoken);

      break;
    case CExprITokenType::EXCLUSIVE_OR_EXPRESSION:
      compileExclusiveOrExpression(itoken);

      break;
    case CExprITokenType::AND_EXPRESSION:
      compileAndExpression(itoken);

      break;
    case CExprITokenType::EQUALITY_EXPRESSION:
      compileEqualityExpression(itoken);

      break;
    case CExprITokenType::RELATIONAL_EXPRESSION:
      compileRelationalExpression(itoken);

      break;
    case CExprITokenType::SHIFT_EXPRESSION:
      compileShiftExpression(itoken);

      break;
    case CExprITokenType::ADDITIVE_EXPRESSION:
      compileAdditiveExpression(itoken);

      break;
    case CExprITokenType::MULTIPLICATIVE_EXPRESSION:
      compileMultiplicativeExpression(itoken);

      break;
    case CExprITokenType::POWER_EXPRESSION:
      compilePowerExpression(itoken);

      break;
    case CExprITokenType::UNARY_EXPRESSION:
      compileUnaryExpression(itoken);

      break;
    case CExprITokenType::POSTFIX_EXPRESSION:
      compilePostfixExpression(itoken);

      break;
    case CExprITokenType::PRIMARY_EXPRESSION:
      compilePrimaryExpression(itoken);

      break;
    case CExprITokenType::ARGUMENT_EXPRESSION_LIST:
      compileArgumentExpressionList(itoken);

      break;
    case CExprITokenType::TOKEN_TYPE: {
      switch (itoken->getType()) {
        case CExprTokenType::IDENTIFIER:
          compileIdentifier(itoken);

          break;
        case CExprTokenType::OPERATOR:
          compileOperator(itoken);

          break;
        case CExprTokenType::INTEGER:
          compileInteger(itoken);

          break;
        case CExprTokenType::REAL:
          compileReal(itoken);

          break;
        case CExprTokenType::STRING:
          compileString(itoken);

          break;
        default:
          assert(false);
          break;
      }
      break;
    }
    default:
      assert(false);
      break;
  }
}

/*
 * <expression>:= <assignment_expression>
 * <expression>:= <expression> , <assignment_expression>
 */

void
CExprCompileImpl::
compileExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if      (num_children == 3) {
    compileExpression(itoken->getChild(0));

    compileAssignmentExpression(itoken->getChild(2));

    stackCToken(itoken->getChild(1)->base());
  }
  else if (num_children == 1)
    compileAssignmentExpression(itoken->getChild(0));
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

void
CExprCompileImpl::
compileAssignmentExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileUnaryExpression(itoken->getChild(0));

    auto itoken1 = itoken->getChild(1);

    auto op = itoken1->getOperator();

    switch (op) {
      case CExprOpType::EQUALS:
        compileAssignmentExpression(itoken->getChild(2));

        break;
      case CExprOpType::TIMES_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::TIMES));

        break;
      case CExprOpType::DIVIDE_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::DIVIDE));

        break;
      case CExprOpType::MODULUS_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::MODULUS));

        break;
      case CExprOpType::PLUS_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::PLUS));

        break;
      case CExprOpType::MINUS_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::MINUS));

        break;
      case CExprOpType::BIT_LSHIFT_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::BIT_LSHIFT));

        break;
      case CExprOpType::BIT_RSHIFT_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::BIT_RSHIFT));

        break;
      case CExprOpType::BIT_AND_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::BIT_AND));

        break;
      case CExprOpType::BIT_XOR_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::BIT_XOR));

        break;
      case CExprOpType::BIT_OR_EQUALS:
        compileUnaryExpression(itoken->getChild(0));

        compileAssignmentExpression(itoken->getChild(2));

        stackCToken(expr_->getOperator(CExprOpType::BIT_OR));

        break;
      default:
        assert(false);
        break;
    }

    stackCToken(expr_->getOperator(CExprOpType::EQUALS));
  }
  else
    compileConditionalExpression(itoken->getChild(0));
}

/*
 * <conditional_expression>:= <logical_or_expression>
 * <conditional_expression>:= <logical_or_expression> ?
 *                            <expression> : <conditional_expression>
 */

void
CExprCompileImpl::
compileConditionalExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 5) {
    // 0 boolean, 2 = lhs, 4 = rhs

    // stack lhs expression
    stackCToken(expr_->getOperator(CExprOpType::START_BLOCK));

    compileExpression(itoken->getChild(2));

    stackCToken(expr_->getOperator(CExprOpType::END_BLOCK));

    //---

    // stack lhs expression
    stackCToken(expr_->getOperator(CExprOpType::START_BLOCK));

    compileConditionalExpression(itoken->getChild(4));

    stackCToken(expr_->getOperator(CExprOpType::END_BLOCK));

    //---

    // stack conditional
    compileLogicalOrExpression(itoken->getChild(0));

    stackCToken(expr_->getOperator(CExprOpType::QUESTION));
  }
  else
    compileLogicalOrExpression(itoken->getChild(0));
}

/*
 * <logical_or_expression>:= <logical_and_expression>
 * <logical_or_expression>:= <logical_or_expression> || <logical_and_expression>
 */

void
CExprCompileImpl::
compileLogicalOrExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileLogicalOrExpression(itoken->getChild(0));

    compileLogicalAndExpression(itoken->getChild(2));

    stackCToken(expr_->getOperator(CExprOpType::LOGICAL_OR));
  }
  else
    compileLogicalAndExpression(itoken->getChild(0));
}

/*
 * <logical_and_expression>:= <inclusive_or_expression>
 * <logical_and_expression>:= <logical_and_expression> && <inclusive_or_expression>
 */

void
CExprCompileImpl::
compileLogicalAndExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileLogicalAndExpression(itoken->getChild(0));

    compileInclusiveOrExpression(itoken->getChild(2));

    stackCToken(expr_->getOperator(CExprOpType::LOGICAL_AND));
  }
  else
    compileInclusiveOrExpression(itoken->getChild(0));
}

/*
 * <inclusive_or_expression>:= <exclusive_or_expression>
 * <inclusive_or_expression>:= <inclusive_or_expression> | <exclusive_or_expression>
 */

void
CExprCompileImpl::
compileInclusiveOrExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileInclusiveOrExpression(itoken->getChild(0));

    compileExclusiveOrExpression(itoken->getChild(2));

    stackCToken(expr_->getOperator(CExprOpType::BIT_OR));
  }
  else
    compileExclusiveOrExpression(itoken->getChild(0));
}

/*
 * <exclusive_or_expression>:= <and_expression>
 * <exclusive_or_expression>:= <exclusive_or_expression> ^ <and_expression>
 */

void
CExprCompileImpl::
compileExclusiveOrExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileExclusiveOrExpression(itoken->getChild(0));

    compileAndExpression(itoken->getChild(2));

    stackCToken(expr_->getOperator(CExprOpType::BIT_XOR));
  }
  else
    compileAndExpression(itoken->getChild(0));
}

/*
 * <and_expression>:= <equality_expression>
 * <and_expression>:= <and_expression> & <equality_expression>
 */

void
CExprCompileImpl::
compileAndExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileAndExpression(itoken->getChild(0));

    compileEqualityExpression(itoken->getChild(2));

    stackCToken(expr_->getOperator(CExprOpType::BIT_AND));
  }
  else
    compileEqualityExpression(itoken->getChild(0));
}

/*
 * <equality_expression>:= <relational_expression>
 * <equality_expression>:= <equality_expression> == <relational_expression>
 * <equality_expression>:= <equality_expression> != <relational_expression>
 * <equality_expression>:= <equality_expression> ~= <relational_expression>
 */

void
CExprCompileImpl::
compileEqualityExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    auto itoken1 = itoken->getChild(1);

    auto op = itoken1->getOperator();

    if      (op == CExprOpType::EQUAL) {
      compileEqualityExpression(itoken->getChild(0));

      compileRelationalExpression(itoken->getChild(2));

      stackCToken(itoken1->base());
    }
    else if (op == CExprOpType::NOT_EQUAL) {
      compileEqualityExpression(itoken->getChild(0));

      compileRelationalExpression(itoken->getChild(2));

      stackCToken(itoken1->base());
    }
    else if (op == CExprOpType::APPROX_EQUAL) {
      compileEqualityExpression(itoken->getChild(0));

      compileRelationalExpression(itoken->getChild(2));

      stackCToken(itoken1->base());
    }
    else
      assert(false);
  }
  else
    compileRelationalExpression(itoken->getChild(0));
}

/*
 * <relational_expression>:= <shift_expression>
 * <relational_expression>:= <relational_expression> <  <shift_expression>
 * <relational_expression>:= <relational_expression> >  <shift_expression>
 * <relational_expression>:= <relational_expression> <= <shift_expression>
 * <relational_expression>:= <relational_expression> >= <shift_expression>
 */

void
CExprCompileImpl::
compileRelationalExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileRelationalExpression(itoken->getChild(0));

    compileShiftExpression(itoken->getChild(2));

    auto itoken1 = itoken->getChild(1);

    stackCToken(itoken1->base());
  }
  else
    compileShiftExpression(itoken->getChild(0));
}

/*
 * <shift_expression>:= <additive_expression>
 * <shift_expression>:= <shift_expression> << <additive_expression>
 * <shift_expression>:= <shift_expression> >> <additive_expression>
 */

void
CExprCompileImpl::
compileShiftExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileShiftExpression(itoken->getChild(0));

    compileAdditiveExpression(itoken->getChild(2));

    auto itoken1 = itoken->getChild(1);

    auto op = itoken1->getOperator();

    if      (op == CExprOpType::BIT_LSHIFT)
      stackCToken(itoken1->base());
    else if (op == CExprOpType::BIT_RSHIFT)
      stackCToken(itoken1->base());
    else
      assert(false);
  }
  else
    compileAdditiveExpression(itoken->getChild(0));
}

/*
 * <additive_expression>:= <multiplicative_expression>
 * <additive_expression>:= <additive_expression> + <multiplicative_expression>
 * <additive_expression>:= <additive_expression> - <multiplicative_expression>
 */

void
CExprCompileImpl::
compileAdditiveExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileAdditiveExpression(itoken->getChild(0));

    compileMultiplicativeExpression(itoken->getChild(2));

    auto itoken1 = itoken->getChild(1);

    auto op = itoken1->getOperator();

    if      (op == CExprOpType::PLUS)
      stackCToken(itoken1->base());
    else if (op == CExprOpType::MINUS)
      stackCToken(itoken1->base());
    else
      assert(false);
  }
  else
    compileMultiplicativeExpression(itoken->getChild(0));
}

/*
 * <multiplicative_expression>:= <unary_expression>
 * <multiplicative_expression>:= <multiplicative_expression> * <unary_expression>
 * <multiplicative_expression>:= <multiplicative_expression> / <unary_expression>
 * <multiplicative_expression>:= <multiplicative_expression> % <unary_expression>
 */

void
CExprCompileImpl::
compileMultiplicativeExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileMultiplicativeExpression(itoken->getChild(0));

    compileUnaryExpression(itoken->getChild(2));

    auto itoken1 = itoken->getChild(1);

    auto op = itoken1->getOperator();

    if      (op == CExprOpType::TIMES)
      stackCToken(itoken1->base());
    else if (op == CExprOpType::DIVIDE)
      stackCToken(itoken1->base());
    else if (op == CExprOpType::MODULUS)
      stackCToken(itoken1->base());
    else
      assert(false);
  }
  else
    compileUnaryExpression(itoken->getChild(0));
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

void
CExprCompileImpl::
compileUnaryExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 2) {
    auto itoken0 = itoken->getChild(0);

    if (itoken0->base()) {
      auto op = itoken0->getOperator();

      switch (op) {
        case CExprOpType::INCREMENT:
          compileUnaryExpression(itoken->getChild(1));

          compileUnaryExpression(itoken->getChild(1));

          stackCToken(expr_->getOperator(CExprOpType::PLUS));
          stackCToken(expr_->getOperator(CExprOpType::EQUALS));

          break;
        case CExprOpType::DECREMENT:
          compileUnaryExpression(itoken->getChild(1));

          compileUnaryExpression(itoken->getChild(1));

          stackCToken(expr_->getOperator(CExprOpType::MINUS));
          stackCToken(expr_->getOperator(CExprOpType::EQUALS));

          break;
        case CExprOpType::PLUS:
          compileUnaryExpression(itoken->getChild(1));

          stackCToken(expr_->getOperator(CExprOpType::UNARY_PLUS));

          break;
        case CExprOpType::MINUS:
          compileUnaryExpression(itoken->getChild(1));

          stackCToken(expr_->getOperator(CExprOpType::UNARY_MINUS));

          break;
        case CExprOpType::BIT_NOT:
          compileUnaryExpression(itoken->getChild(1));

          stackCToken(itoken0->base());

          break;
        case CExprOpType::LOGICAL_NOT:
          compileUnaryExpression(itoken->getChild(1));

          stackCToken(itoken0->base());

          break;
        default:
          assert(false);
          break;
      }
    }
    else {
      assert(false);
    }
  }
  else
    compilePowerExpression(itoken->getChild(0));
}

/*
 * <power_expression>:= <postfix_expression>
 * <power_expression>:= <postfix_expression> ** <power_expression>
 */

void
CExprCompileImpl::
compilePowerExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compilePostfixExpression(itoken->getChild(0));

    compilePowerExpression(itoken->getChild(2));

    stackCToken(expr_->getOperator(CExprOpType::POWER));
  }
  else
    compilePostfixExpression(itoken->getChild(0));
}

/*
 * <postfix_expression>:= <primary_expression>
 * <postfix_expression>:= <identifier> ( <argument_expression_list>(opt) )
 * <postfix_expression>:= <postfix_expression> ++
 * <postfix_expression>:= <postfix_expression> --
 */

void
CExprCompileImpl::
compilePostfixExpression(CExprITokenPtr itoken)
{
  auto itoken0 = itoken->getChild(0);

  uint num_children = itoken->getNumChildren();

  if (num_children == 1) {
    compilePrimaryExpression(itoken0);
    return;
  }

  auto itoken1 = itoken->getChild(1);

  auto op = itoken1->getOperator();

  if      (op == CExprOpType::OPEN_RBRACKET) {
    stackCToken(itoken1->base());

    uint num_args = 0;

    if (num_children == 4) {
      num_args = itoken->getChild(2)->
        countITokenChildrenOfType(CExprITokenType::ASSIGNMENT_EXPRESSION);

      compileArgumentExpressionList(itoken->getChild(2));
    }

    auto itoken00 = itoken->getChild(0);

    const std::string &identifier = itoken00->getIdentifier();

    CExprFunctionMgr::Functions functions;

    expr_->getFunctions(identifier, functions);

    CExprFunctionPtr function;

    for (auto &function1 : functions) {
      if (! function1)
        continue;

      function = function1;

      uint function_num_args = function1->numArgs();

      if (function_num_args == num_args)
        break;
    }

    if (function) {
      uint function_num_args = function->numArgs();

      for (uint i = num_args; i < function_num_args; ++i) {
        if (! (uint(function->argType(i)) & uint(CExprValueType::NUL)))
          break;

        stackDummyValue();

        ++num_args;
      }

      if (function->isVariableArgs()) {
        if (function_num_args > num_args) {
          errorData_.setLastError("Function called with too few arguments");
          return;
        }
      }
      else {
        if (function_num_args != num_args) {
          errorData_.setLastError("Function called with wrong number of arguments");
          return;
        }
      }

      stackFunction(function);
    }
    else {
      errorData_.setLastError("Invalid Function '" + identifier + "'");
      return;
    }
  }
  else if (op == CExprOpType::INCREMENT) {
    compilePostfixExpression(itoken->getChild(0));
    compilePostfixExpression(itoken->getChild(0)); // dup value

    stackCToken(expr_->getOperator(CExprOpType::EQUALS));
    stackCToken(expr_->getOperator(CExprOpType::PLUS));
  }
  else if (op == CExprOpType::DECREMENT) {
    compilePostfixExpression(itoken->getChild(0));
    compilePostfixExpression(itoken->getChild(0)); // dup value

    stackCToken(expr_->getOperator(CExprOpType::EQUALS));
    stackCToken(expr_->getOperator(CExprOpType::MINUS));
  }
}

/*
 * <primary_expression>:= <integer>
 * <primary_expression>:= <real>
 * <primary_expression>:= <string>
 * <primary_expression>:= <identifier>
 * <primary_expression>:= ( <expression> )
 */

void
CExprCompileImpl::
compilePrimaryExpression(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileExpression(itoken->getChild(1));
  }
  else {
    switch (itoken->getChild(0)->getType()) {
      case CExprTokenType::INTEGER:
        compileInteger(itoken->getChild(0));

        break;
      case CExprTokenType::REAL:
        compileReal(itoken->getChild(0));

        break;
      case CExprTokenType::STRING:
        compileString(itoken->getChild(0));

        break;
      case CExprTokenType::IDENTIFIER:
        compileIdentifier(itoken->getChild(0));

        break;
      default:
        assert(false);
        break;
    }
  }
}

/*
 * <argument_expression_list>:= <assignment_expression>
 * <argument_expression_list>:= <argument_expression_list> , <assignment_expression>
 */

void
CExprCompileImpl::
compileArgumentExpressionList(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  if (num_children == 3) {
    compileArgumentExpressionList(itoken->getChild(0));

    compileAssignmentExpression(itoken->getChild(2));
  }
  else
    compileAssignmentExpression(itoken->getChild(0));
}

void
CExprCompileImpl::
compileIdentifier(CExprITokenPtr itoken)
{
  stackCToken(itoken->base());
}

void
CExprCompileImpl::
compileOperator(CExprITokenPtr itoken)
{
  stackCToken(itoken->base());
}

void
CExprCompileImpl::
compileInteger(CExprITokenPtr itoken)
{
  auto value = expr_->createIntegerValue(itoken->getInteger());

  CExprTokenBaseP base(CExprTokenMgrInst->createValueToken(value));

  stackCToken(base);
}

void
CExprCompileImpl::
compileReal(CExprITokenPtr itoken)
{
  auto value = expr_->createRealValue(itoken->getReal());

  CExprTokenBaseP base(CExprTokenMgrInst->createValueToken(value));

  stackCToken(base);
}

void
CExprCompileImpl::
compileString(CExprITokenPtr itoken)
{
  auto value = expr_->createStringValue(itoken->getString());

  CExprTokenBaseP base(CExprTokenMgrInst->createValueToken(value));

  stackCToken(base);
}

void
CExprCompileImpl::
compileValue(CExprITokenPtr itoken)
{
  stackCToken(itoken->base());
}

#if 0
void
CExprCompileImpl::
compileITokenChildren(CExprITokenPtr itoken)
{
  uint num_children = itoken->getNumChildren();

  for (uint i = 0; i < num_children; i++)
    compileIToken1(itoken->getChild(i));
}
#endif

void
CExprCompileImpl::
stackFunction(CExprFunctionPtr function)
{
  CExprTokenBaseP base(CExprTokenMgrInst->createFunctionToken(function));

  stackCToken(base);
}

void
CExprCompileImpl::
stackDummyValue()
{
  CExprTokenBaseP base(CExprTokenMgrInst->createValueToken(CExprValuePtr()));

  stackCToken(base);
}

void
CExprCompileImpl::
stackCToken(const CExprTokenBaseP &base)
{
  tokenStack_.addToken(base);
}

bool
CExprCompileImpl::
hasFunction(const std::string &name) const
{
  return tokenStack_.hasFunction(name);
}
