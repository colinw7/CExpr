#include <CExprI.h>
#include <sstream>

class CExprExecuteImpl {
 public:
  CExprExecuteImpl(CExpr *expr) : expr_(expr) { }

 ~CExprExecuteImpl() { }

  bool executeCTokenStack(const CExprTokenStack &stack, CExprValueArray &values);
  bool executeCTokenStack(const CExprTokenStack &stack, CExprValuePtr &value);

 private:
  bool executeToken                (const CExprTokenBaseP &ctoken);
  bool executeOperator             (const CExprTokenBaseP &ctoken);
  void executeQuestionOperator     ();
  void executeUnaryOperator        (CExprOpType type);
  void executeLogicalUnaryOperator (CExprOpType type);
  void executeBitwiseUnaryOperator (CExprOpType type);
  bool executeBinaryOperator       (CExprOpType type);
  void executeLogicalBinaryOperator(CExprOpType type);
  void executeBitwiseBinaryOperator(CExprOpType type);
  void executeColonOperator        ();
  void executeCommaOperator        ();
  void executeEqualsOperator       ();
  bool executeFunction             (const CExprFunctionPtr &function, CExprValuePtr &value);
  bool executeBlock                (const CExprTokenStack &stack, CExprValuePtr &value);

  CExprValuePtr  etokenToValue(const CExprTokenBaseP &etoken);
#if 0
  CExprValueType etokenToValueType(const CExprTokenBaseP &etoken);
#endif

  void stackValue (const CExprValuePtr &value);
  void stackBlock ();
  void stackEToken(const CExprTokenBaseP &etoken);

  CExprValuePtr   unstackValue ();
  CExprTokenBaseP unstackEToken();

 private:
  CExpr*          expr_       { nullptr };
  CExprTokenStack ctokenStack_;
  uint            ctokenPos_  { 0 };
  uint            numCTokens_ { 0 };
  CExprTokenStack etokenStack_;
};

//------------

CExprExecute::
CExprExecute(CExpr *expr) :
 expr_(expr)
{
  impl_ = std::make_unique<CExprExecuteImpl>(expr);
}

CExprExecute::
~CExprExecute()
{
}

bool
CExprExecute::
executeCTokenStack(const CExprTokenStack &stack, CExprValueArray &values)
{
  return impl_->executeCTokenStack(stack, values);
}

bool
CExprExecute::
executeCTokenStack(const CExprTokenStack &stack, CExprValuePtr &value)
{
  return impl_->executeCTokenStack(stack, value);
}

//------------

bool
CExprExecuteImpl::
executeCTokenStack(const CExprTokenStack &stack, CExprValueArray &values)
{
  ctokenStack_ = stack;

  etokenStack_.clear();

  numCTokens_ = ctokenStack_.getNumTokens();
  ctokenPos_  = 0;

  while (ctokenPos_ < numCTokens_) {
    auto ctoken = ctokenStack_.getToken(ctokenPos_++);

    if (! executeToken(ctoken))
      return false;

    if (expr_->getDebug())
      std::cerr << "EToken Stack:" << etokenStack_ << std::endl;
  }

  std::deque<CExprValuePtr> values1;

  bool rc = true;

  while (! etokenStack_.empty()) {
    auto value = unstackValue();

    if (value)
      values1.push_front(value);
    else
      rc = false;
  }

  std::copy(values1.begin(), values1.end(), std::back_inserter(values));

  return rc;
}

bool
CExprExecuteImpl::
executeCTokenStack(const CExprTokenStack &stack, CExprValuePtr &value)
{
  CExprValueArray values;

  if (! executeCTokenStack(stack, values))
    return false;

  if (values.empty())
    value = CExprValuePtr();
  else
    value = values.back();

  return true;
}

bool
CExprExecuteImpl::
executeToken(const CExprTokenBaseP &ctoken)
{
  switch (ctoken->type()) {
    case CExprTokenType::IDENTIFIER:
      stackEToken(ctoken);

      break;
    case CExprTokenType::OPERATOR:
      if (! executeOperator(ctoken))
        return false;

      break;
    case CExprTokenType::INTEGER: {
      auto value = expr_->createIntegerValue(ctoken->getInteger());

      stackValue(value);

      break;
    }
    case CExprTokenType::REAL: {
      auto value = expr_->createRealValue(ctoken->getReal());

      stackValue(value);

      break;
    }
    case CExprTokenType::STRING: {
      auto value = expr_->createStringValue(ctoken->getString());

      stackValue(value);

      break;
    }
    case CExprTokenType::FUNCTION: {
      stackEToken(ctoken);

      break;
    }
    case CExprTokenType::VALUE:
      stackEToken(ctoken);

      break;
    default:
      assert(false);
      break;
  }

  return true;
}

bool
CExprExecuteImpl::
executeOperator(const CExprTokenBaseP &ctoken)
{
  auto type = ctoken->getOperator();

  switch (type) {
    case CExprOpType::LOGICAL_NOT:
      executeLogicalUnaryOperator(type);
      break;
    case CExprOpType::BIT_NOT:
      executeBitwiseUnaryOperator(type);
      break;
    case CExprOpType::UNARY_PLUS:
    case CExprOpType::UNARY_MINUS:
      executeUnaryOperator(type);
      break;
    case CExprOpType::POWER:
    case CExprOpType::TIMES:
    case CExprOpType::DIVIDE:
    case CExprOpType::MODULUS:
    case CExprOpType::PLUS:
    case CExprOpType::MINUS:
    case CExprOpType::LESS:
    case CExprOpType::LESS_EQUAL:
    case CExprOpType::GREATER:
    case CExprOpType::GREATER_EQUAL:
    case CExprOpType::EQUAL:
    case CExprOpType::NOT_EQUAL:
      if (! executeBinaryOperator(type))
        return false;

      break;
    case CExprOpType::LOGICAL_AND:
    case CExprOpType::LOGICAL_OR:
      executeLogicalBinaryOperator(type);
      break;
    case CExprOpType::BIT_LSHIFT:
    case CExprOpType::BIT_RSHIFT:
    case CExprOpType::BIT_AND:
    case CExprOpType::BIT_XOR:
    case CExprOpType::BIT_OR:
      executeBitwiseBinaryOperator(type);
      break;
    case CExprOpType::QUESTION:
      executeQuestionOperator();
      break;
    case CExprOpType::COLON:
      executeColonOperator();
      break;
    case CExprOpType::EQUALS:
      executeEqualsOperator();
      break;
    case CExprOpType::COMMA:
      executeCommaOperator();
      break;
    case CExprOpType::OPEN_RBRACKET:
      stackEToken(ctoken);
      break;
    case CExprOpType::START_BLOCK:
      stackBlock();
      break;
    case CExprOpType::UNKNOWN:
      stackEToken(ctoken);
      break;
    default: {
      expr_->errorMsg("Invalid operator for 'executeOperator'");
      return false;
    }
  }

  return true;
}

void
CExprExecuteImpl::
executeQuestionOperator()
{
  // pop boolean
  auto value = unstackValue();
  if (! value) return;

  bool flag;

  if (! value->getBooleanValue(flag))
    flag = false;

  //---

  // pop second token (rhs=false)
  auto etoken2 = unstackEToken();

  //---

  // pop first token (lhs=tue)
  auto etoken1 = unstackEToken();

  //---

  CExprValuePtr value1;

  if (flag) {
    if (etoken1)
      value1 = etokenToValue(etoken1);
  }
  else {
    if (etoken2)
      value1 = etokenToValue(etoken2);
  }

  if (! value1)
    return;

  stackValue(value1);
}

/* <value> <unary_op> */
void
CExprExecuteImpl::
executeUnaryOperator(CExprOpType type)
{
  auto value = unstackValue();
  if (! value) return;

  auto value1 = value->execUnaryOp(expr_, type);
  if (! value1) return;

  stackValue(value1);
}

/* <value> <unary_op> */
void
CExprExecuteImpl::
executeLogicalUnaryOperator(CExprOpType type)
{
  auto value = unstackValue();
  if (! value) return;

  if (! value->isBooleanValue()) {
    value = CExprValuePtr(value->dup());

    if (! value->convToBoolean())
      return;
  }

  auto value1 = value->execUnaryOp(expr_, type);
  if (! value1) return;

  stackValue(value1);
}

/* <value> <unary_op> */
void
CExprExecuteImpl::
executeBitwiseUnaryOperator(CExprOpType type)
{
  auto value = unstackValue();
  if (! value) return;

  if (! value->isIntegerValue()) {
    value = CExprValuePtr(value->dup());

    if (! value->convToInteger())
      return;
  }

  auto value1 = value->execUnaryOp(expr_, type);
  if (! value1) return;

  stackValue(value1);
}

/* <value1> <value2> <binary_op> */
bool
CExprExecuteImpl::
executeBinaryOperator(CExprOpType type)
{
  // pop rhs
  auto value2 = unstackValue();

  // pop lhs
  auto value1 = unstackValue();

  //---

  if (! value1 || ! value2)
    return false;

  bool convReal = false;

  if (value1->isRealValue() || value2->isRealValue())
    convReal = true;

  if (! convReal && type == CExprOpType::DIVIDE && value2->isIntegerValue()) {
    long l = 0;

    value2->getIntegerValue(l);

    if (l == 0)
      convReal = true;
  }

  if (convReal) {
    if (! value1->isRealValue())
      value1 = CExprValuePtr(value1->dup());
    if (! value2->isRealValue())
      value2 = CExprValuePtr(value2->dup());

    if (! value1->convToReal()) return false;
    if (! value2->convToReal()) return false;
  }

  auto value = value1->execBinaryOp(expr_, value2, type);

  stackValue(value);

  return true;
}

/* <value1> <value2> <binary_op> */
void
CExprExecuteImpl::
executeLogicalBinaryOperator(CExprOpType type)
{
  // pop rhs
  auto value2 = unstackValue();

  // pop lhs
  auto value1 = unstackValue();

  //---

  if (! value1 || ! value2)
    return;

  if (! value1->isBooleanValue()) {
    value1 = CExprValuePtr(value1->dup());

    if (! value1->convToBoolean())
      return;
  }

  if (! value2->isBooleanValue()) {
    value2 = CExprValuePtr(value2->dup());

    if (! value2->convToBoolean())
      return;
  }

  auto value = value1->execBinaryOp(expr_, value2, type);

  stackValue(value);
}

/* <value1> <value2> <binary_op> */
void
CExprExecuteImpl::
executeBitwiseBinaryOperator(CExprOpType type)
{
  // pop rhs
  auto value2 = unstackValue();

  // pop lhs
  auto value1 = unstackValue();

  //---

  if (! value1 || ! value2)
    return;

  if (! value1->isIntegerValue()) {
    value1 = CExprValuePtr(value1->dup());

    if (! value1->convToInteger())
      return;
  }

  if (! value2->isIntegerValue()) {
    value2 = CExprValuePtr(value2->dup());

    if (! value2->convToInteger())
      return;
  }

  auto value = value1->execBinaryOp(expr_, value2, type);

  stackValue(value);
}

void
CExprExecuteImpl::
executeColonOperator()
{
}

void
CExprExecuteImpl::
executeCommaOperator()
{
}

void
CExprExecuteImpl::
executeEqualsOperator()
{
  // rhs
  auto value1 = unstackValue();
  if (! value1) return;

  //---

  auto etoken2 = unstackEToken();
  if (! etoken2) return;

  //---

  CExprValuePtr value;

  if (etoken2->type() == CExprTokenType::IDENTIFIER) {
    auto variable = expr_->createVariable(etoken2->getIdentifier(), value1);

    value = variable->getValue();
  }
  else {
    expr_->errorMsg("Non lvalue for asssignment");
    return;
  }

  stackValue(value);
}

bool
CExprExecuteImpl::
executeFunction(const CExprFunctionPtr &function, CExprValuePtr &value)
{
  std::deque<CExprValuePtr> values;

  auto etoken = unstackEToken();
  if (! etoken) return false;

  while (etoken->type() != CExprTokenType::OPERATOR) {
    auto value1 = etokenToValue(etoken);

    values.push_front(value1);

    etoken = unstackEToken();
    assert(etoken);
  }

  for (uint i = 0; i < values.size(); ++i) {
    auto value1 = values[i];

    auto argType = function->argType(i);

    if (! value1) {
      if (! (uint(argType) & uint(CExprValueType::NUL))) {
        expr_->errorMsg("Invalid type for function argument");
        return false;
      }
    }
    else {
      if (! (uint(argType) & uint(CExprValueType::NUL))) {
        if (! (uint(value1->getType()) & uint(argType)))
          value1 = CExprValuePtr(value1->dup());

        if (! value1->convToType(argType)) {
          expr_->errorMsg("Invalid type for function argument");
          return false;
        }
      }
    }
  }

  assert(etoken->type() == CExprTokenType::OPERATOR);

  CExprValueArray values1;

  std::copy(values.begin(), values.end(), std::back_inserter(values1));

  if (! function->checkValues(values1)) {
    std::stringstream ostr;
    ostr << "Invalid function values : ";
    function->print(ostr);
    expr_->errorMsg(ostr.str());
    return false;
  }

  value = function->exec(expr_, values1);

  return true;
}

bool
CExprExecuteImpl::
executeBlock(const CExprTokenStack &stack, CExprValuePtr &value)
{
  CExprExecuteImpl impl(expr_);

  return impl.executeCTokenStack(stack, value);
}

CExprValuePtr
CExprExecuteImpl::
etokenToValue(const CExprTokenBaseP &etoken)
{
  switch (etoken->type()) {
    case CExprTokenType::IDENTIFIER: {
      auto variable = expr_->getVariable(etoken->getIdentifier());

      if (variable)
        return variable->getValue();

      break;
    }
    case CExprTokenType::FUNCTION: {
      CExprValuePtr value;

      if (executeFunction(etoken->getFunction(), value))
        return value;

      break;
    }
    case CExprTokenType::BLOCK: {
      CExprValuePtr value;

      if (executeBlock(etoken->getBlock(), value))
        return value;

      break;
    }
    case CExprTokenType::VALUE:
      return etoken->getValue();
    default:
      break;
  }

  return CExprValuePtr();
}

void
CExprExecuteImpl::
stackValue(const CExprValuePtr &value)
{
  if (! value) return;

  CExprTokenBaseP base(CExprTokenMgrInst->createValueToken(value));

  stackEToken(base);
}

void
CExprExecuteImpl::
stackBlock()
{
  CExprTokenStack stack;

  int brackets = 1;

  while (ctokenPos_ < numCTokens_) {
    auto ctoken = ctokenStack_.getToken(ctokenPos_++);

    if (! ctoken)
      break;

    if (ctoken->type() == CExprTokenType::OPERATOR) {
      auto op = ctoken->getOperator();

      if      (op == CExprOpType::START_BLOCK)
        ++brackets;
      else if (op == CExprOpType::END_BLOCK) {
        --brackets;

        if (brackets <= 0)
          break;
      }
    }

    stack.addToken(ctoken);
  }

  CExprTokenBaseP base(new CExprTokenBlock(stack));

  stackEToken(base);
}

void
CExprExecuteImpl::
stackEToken(const CExprTokenBaseP &base)
{
  etokenStack_.addToken(base);
}

CExprValuePtr
CExprExecuteImpl::
unstackValue()
{
  auto etoken = unstackEToken();
  if (! etoken) return CExprValuePtr();

  return etokenToValue(etoken);
}

CExprTokenBaseP
CExprExecuteImpl::
unstackEToken()
{
  auto etoken = etokenStack_.pop_back();

  return etoken;
}
