#include <CExprI.h>

CExprValuePtr
CExprBooleanValue::
execUnaryOp(CExpr *expr, CExprOpType op) const
{
  switch (op) {
    case CExprOpType::LOGICAL_NOT:
      return expr->createBooleanValue(! boolean_);
    default:
      return CExprValuePtr();
  }
}

CExprValuePtr
CExprBooleanValue::
execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const
{
  bool rboolean = false;

  if (! rhs->getBooleanValue(rboolean))
    return CExprValuePtr();

  switch (op) {
    case CExprOpType::LOGICAL_AND:
      return expr->createBooleanValue(boolean_ && rboolean);
    case CExprOpType::LOGICAL_OR:
      return expr->createBooleanValue(boolean_ || rboolean);
    default:
      return CExprValuePtr();
  }
}
