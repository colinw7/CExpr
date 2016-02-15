#include <CExprI.h>

const std::string &
CExprTokenBase::
getIdentifier() const
{
  assert(type() == CExprTokenType::IDENTIFIER);

  return static_cast<const CExprTokenIdentifier *>(this)->getIdentifier();
}

CExprOpType
CExprTokenBase::
getOperator() const
{
  assert(type() == CExprTokenType::OPERATOR);

  return static_cast<const CExprTokenOperator *>(this)->getType();
}

long
CExprTokenBase::
getInteger() const
{
  assert(type() == CExprTokenType::INTEGER);

  return static_cast<const CExprTokenInteger *>(this)->getInteger();
}

double
CExprTokenBase::
getReal() const
{
  assert(type() == CExprTokenType::REAL);

  return static_cast<const CExprTokenReal *>(this)->getReal();
}

const std::string &
CExprTokenBase::
getString() const
{
  assert(type() == CExprTokenType::STRING);

  return static_cast<const CExprTokenString *>(this)->getString();
}

CExprFunctionPtr
CExprTokenBase::
getFunction() const
{
  assert(type() == CExprTokenType::FUNCTION);

  return static_cast<const CExprTokenFunction *>(this)->getFunction();
}

CExprValuePtr
CExprTokenBase::
getValue() const
{
  assert(type() == CExprTokenType::VALUE);

  return static_cast<const CExprTokenValue *>(this)->getValue();
}

const CExprTokenStack &
CExprTokenBase::
getBlock() const
{
  assert(type() == CExprTokenType::BLOCK);

  return static_cast<const CExprTokenBlock *>(this)->stack();
}

void
CExprTokenBase::
printQualified(std::ostream &os) const
{
  switch (type()) {
    case CExprTokenType::IDENTIFIER:
      os << "<identifier>";
      break;
    case CExprTokenType::OPERATOR:
      os << "<operator>";
      break;
    case CExprTokenType::INTEGER:
      os << "<integer>";
      break;
    case CExprTokenType::REAL:
      os << "<real>";
      break;
    case CExprTokenType::STRING:
      os << "<string>";
      break;
    case CExprTokenType::FUNCTION:
      os << "<function>";
      break;
    case CExprTokenType::VALUE:
      os << "<value>";
      break;
    case CExprTokenType::BLOCK:
      os << "<block>";
      break;
    default:
      os << "<-?->";
      break;
  }

  print(os);
}

//----

void
CExprTokenOperator::
print(std::ostream &os) const
{
  os << CExpr::instance()->getOperatorName(type_);
}

void
CExprTokenFunction::
print(std::ostream &os) const
{
  function_->print(os, /*expanded*/false);
}

//------

void
CExprTokenStack::
print(std::ostream &os) const
{
  uint len = stack_.size();

  for (uint i = 0; i < len; ++i) {
    if (i > 0) os << " ";

    stack_[i]->printQualified(os);
  }
}
