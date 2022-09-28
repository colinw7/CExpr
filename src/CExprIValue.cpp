#include <CExprI.h>
#include <cerrno>
#include <cmath>

bool
CExprIntegerValue::
getStringValue(std::string &s) const
{
  s = CStrUtil::toString(integer_);

  return true;
}

CExprValuePtr
CExprIntegerValue::
execUnaryOp(CExpr *expr, CExprOpType op) const
{
  switch (op) {
    case CExprOpType::UNARY_PLUS:
      return expr->createIntegerValue(integer_);
    case CExprOpType::UNARY_MINUS:
      return expr->createIntegerValue(-integer_);
    case CExprOpType::BIT_NOT:
      return expr->createIntegerValue(~integer_);
    default:
      return CExprValuePtr();
  }
}

CExprValuePtr
CExprIntegerValue::
execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const
{
  long irhs;

  if (! rhs->getIntegerValue(irhs))
    return CExprValuePtr();

  switch (op) {
    case CExprOpType::POWER: {
      int error_code;

      long integer = integerPower(integer_, irhs, &error_code);

      if (error_code != 0)
        return CExprValuePtr();
      else
        return expr->createIntegerValue(integer);
    }
    case CExprOpType::TIMES:
      return expr->createIntegerValue(integer_ * irhs);
    case CExprOpType::DIVIDE: {
      // divide by zero
      if (irhs == 0) return CExprValuePtr();

      return expr->createIntegerValue(integer_ / irhs);
    }
    case CExprOpType::MODULUS: {
      // divide by zero
      if (irhs == 0) return CExprValuePtr();

      return expr->createIntegerValue(integer_ % irhs);
    }
    case CExprOpType::PLUS:
      return expr->createIntegerValue(integer_ + irhs);
    case CExprOpType::MINUS:
      return expr->createIntegerValue(integer_ - irhs);
    case CExprOpType::BIT_LSHIFT:
      return expr->createIntegerValue(integer_ << irhs);
    case CExprOpType::BIT_RSHIFT:
      return expr->createIntegerValue(integer_ >> irhs);
    case CExprOpType::LESS:
      return expr->createBooleanValue(integer_ < irhs);
    case CExprOpType::LESS_EQUAL:
      return expr->createBooleanValue(integer_ <= irhs);
    case CExprOpType::GREATER:
      return expr->createBooleanValue(integer_ > irhs);
    case CExprOpType::GREATER_EQUAL:
      return expr->createBooleanValue(integer_ >= irhs);
    case CExprOpType::EQUAL:
      return expr->createBooleanValue(integer_ == irhs);
    case CExprOpType::NOT_EQUAL:
      return expr->createBooleanValue(integer_ != irhs);
    case CExprOpType::BIT_AND:
      return expr->createIntegerValue(integer_ & irhs);
    case CExprOpType::BIT_XOR:
      return expr->createIntegerValue(integer_ ^ irhs);
    case CExprOpType::BIT_OR:
      return expr->createIntegerValue(integer_ | irhs);
    default:
      return CExprValuePtr();
  }
}

long
CExprIntegerValue::
integerPower(long integer1, long integer2, int *error_code) const
{
  *error_code = 0;

  if (integer1 == 0 && integer2 < 0) {
    *error_code = int(CExprErrorType::ZERO_TO_NEG_POWER_UNDEF);
    return 0;
  }

  double real;

  errno = 0;

  if (integer2 < 0)
    real = 1.0/pow(double(integer1), double(-integer2));
  else
    real = pow(double(integer1), double(integer2));

  if (errno != 0) {
    *error_code = int(CExprErrorType::POWER_FAILED);
    return 0;
  }

  long integer = realToInteger(real, error_code);

  if (*error_code != 0)
    return 0;

  return integer;
}

long
CExprIntegerValue::
realToInteger(double real, int *error_code) const
{
  long integer = long(real);

  double real1 = double(integer);

  if (fabs(real1 - real) >= 1.0)
    *error_code = int(CExprErrorType::REAL_TOO_BIG_FOR_INTEGER);

  return integer;
}
