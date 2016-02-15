#include <CExprI.h>

bool
CExprStringValue::
getBooleanValue(bool &b) const
{
  return CStrUtil::toBool(str_, &b);
}

bool
CExprStringValue::
getIntegerValue(long &l) const
{
  return CStrUtil::toInteger(str_, &l);
}

bool
CExprStringValue::
getRealValue(double &r) const
{
  return CStrUtil::toReal(str_, &r);
}

CExprValuePtr
CExprStringValue::
execUnaryOp(CExpr *, CExprOpType) const
{
  return CExprValuePtr();
}

CExprValuePtr
CExprStringValue::
execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const
{
  std::string rstr;

  if (! rhs->getStringValue(rstr))
    return CExprValuePtr();

  //---

  std::string result;

  switch (op) {
    case CExprOpType::LESS:
      return expr->createBooleanValue(str_ < rstr);
    case CExprOpType::LESS_EQUAL:
      return expr->createBooleanValue(str_ <= rstr);
    case CExprOpType::GREATER:
      return expr->createBooleanValue(str_ > rstr);
    case CExprOpType::GREATER_EQUAL:
      return expr->createBooleanValue(str_ >= rstr);
    // TODO: disable for gnuplot ?
    case CExprOpType::EQUAL:
      return expr->createBooleanValue(str_ == rstr);
    case CExprOpType::NOT_EQUAL:
      return expr->createBooleanValue(str_ != rstr);
    case CExprOpType::PLUS:
      return expr->createStringValue(str_ + rstr);
    default:
      return CExprValuePtr();
  }
}
