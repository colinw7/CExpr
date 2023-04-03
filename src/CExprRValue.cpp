#include <CExprI.h>
#include <CMathGen.h>
#include <NaN.h>
#include <cerrno>

bool
CExprRealValue::
getStringValue(std::string &s) const
{
  if (! IsNaN(real_))
    s = CStrUtil::toString(real_);
  else
    s = "NaN";

  return true;
}

CExprValuePtr
CExprRealValue::
execUnaryOp(CExpr *expr, CExprOpType op) const
{
  switch (op) {
    case CExprOpType::UNARY_PLUS:
      return expr->createRealValue(real_);
    case CExprOpType::UNARY_MINUS:
      return expr->createRealValue(-real_);
    default:
      return CExprValuePtr();
  }
}

CExprValuePtr
CExprRealValue::
execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const
{
  double rrhs;

  if (! rhs->getRealValue(rrhs))
    return CExprValuePtr();

  switch (op) {
    case CExprOpType::POWER: {
      int error_code;

      double real = realPower(real_, rrhs, &error_code);

      if (error_code != 0)
        return CExprValuePtr();

      return expr->createRealValue(real);
    }
    case CExprOpType::TIMES:
      return expr->createRealValue(real_ * rrhs);
    case CExprOpType::DIVIDE:
      return expr->createRealValue(real_ / rrhs);
    case CExprOpType::MODULUS: {
      int error_code;

      double real = realModulus(real_, rrhs, &error_code);

      if (error_code != 0)
        return CExprValuePtr();

      return expr->createRealValue(real);
    }
    case CExprOpType::PLUS:
      return expr->createRealValue(real_ + rrhs);
    case CExprOpType::MINUS:
      return expr->createRealValue(real_ - rrhs);
    case CExprOpType::LESS:
      return expr->createBooleanValue(real_ < rrhs);
    case CExprOpType::LESS_EQUAL:
      return expr->createBooleanValue(real_ <= rrhs);
    case CExprOpType::GREATER:
      return expr->createBooleanValue(real_ > rrhs);
    case CExprOpType::GREATER_EQUAL:
      return expr->createBooleanValue(real_ >= rrhs);
    case CExprOpType::EQUAL:
      return expr->createBooleanValue(real_ == rrhs);
    case CExprOpType::NOT_EQUAL:
      return expr->createBooleanValue(real_ != rrhs);
    default:
      return CExprValuePtr();
  }
}

double
CExprRealValue::
realPower(double real1, double real2, int *error_code) const
{
  *error_code = 0;

  if (IsNaN(real1) || IsNaN(real2)) {
    *error_code = int(CExprErrorType::NAN_OPERATION);
    return CMathGen::getNaN();
  }

  bool is_int = (long(real2) == real2);

  if (real1 < 0.0 && ! is_int) {
    *error_code = int(CExprErrorType::NON_INTEGER_POWER_OF_NEGATIVE);
    return CMathGen::getNaN();
  }

  if (real1 == 0.0 && real2 < 0.0) {
    *error_code = int(CExprErrorType::ZERO_TO_NEG_POWER_UNDEF);
    return CMathGen::getNaN();
  }

  errno = 0;

  double real;

  if (real2 < 0.0)
    real = 1.0/pow(real1, -real2);
  else
    real = pow(real1, real2);

  if (errno != 0) {
    *error_code = int(CExprErrorType::POWER_FAILED);
    return CMathGen::getNaN();
  }

  return real;
}

double
CExprRealValue::
realModulus(double real1, double real2, int *error_code) const
{
  *error_code = 0;

  if (IsNaN(real1) || IsNaN(real2)) {
    *error_code = int(CExprErrorType::NAN_OPERATION);
    return CMathGen::getNaN();
  }

  if (real2 == 0.0) {
    *error_code = int(CExprErrorType::DIVIDE_BY_ZERO);
    return CMathGen::getNaN();
  }

  long factor = long(real1/real2);

  double result = real1 - (real2*double(factor));

  return result;
}
