#include <CExprI.h>

CExprValue::
CExprValue() :
 type_(CExprValueType::NONE)
{
}

CExprValue::
CExprValue(const CExprBooleanValue &boolean) :
 type_(CExprValueType::BOOLEAN), base_(boolean.dup())
{
}

CExprValue::
CExprValue(const CExprIntegerValue &integer) :
 type_(CExprValueType::INTEGER), base_(integer.dup())
{
}

CExprValue::
CExprValue(const CExprRealValue &real) :
 type_(CExprValueType::REAL), base_(real.dup())
{
}

CExprValue::
CExprValue(const CExprStringValue &str) :
 type_(CExprValueType::STRING), base_(str.dup())
{
}

CExprValue::
~CExprValue()
{
}

CExprValue *
CExprValue::
dup() const
{
  auto *value1 = new CExprValue;

  value1->type_ = type_;

  if (base_)
    value1->base_ = CExprValueBaseP(base_->dup());

  return value1;
}

bool
CExprValue::
isBooleanValue() const
{
  return isType(CExprValueType::BOOLEAN);
}

bool
CExprValue::
isIntegerValue() const
{
  return isType(CExprValueType::INTEGER);
}

bool
CExprValue::
isRealValue() const
{
  return isType(CExprValueType::REAL);
}

bool
CExprValue::
isStringValue() const
{
  return isType(CExprValueType::STRING);
}

bool
CExprValue::
getBooleanValue(bool &b) const
{
  return base_->getBooleanValue(b);
}

bool
CExprValue::
getIntegerValue(long &l) const
{
  return base_->getIntegerValue(l);
}

bool
CExprValue::
getRealValue(double &r) const
{
  return base_->getRealValue(r);
}

bool
CExprValue::
getStringValue(std::string &s) const
{
  return base_->getStringValue(s);
}

void
CExprValue::
setBooleanValue(bool b)
{
  assert(isBooleanValue());

  base_->setBooleanValue(b);
}

void
CExprValue::
setIntegerValue(long l)
{
  assert(isIntegerValue());

  base_->setIntegerValue(l);
}

void
CExprValue::
setRealValue(double r)
{
  assert(isRealValue());

  base_->setRealValue(r);
}

void
CExprValue::
setStringValue(const std::string &s)
{
  assert(isStringValue());

  base_->setStringValue(s);
}

bool
CExprValue::
convToType(CExprValueType type)
{
  uint itype = uint(type);

  if (itype & uint(type_)) return true;

  // TODO: conversion order
  if      (hasType(itype, CExprValueType::BOOLEAN))
    return convToBoolean();
  else if (hasType(itype, CExprValueType::INTEGER))
    return convToInteger();
  else if (hasType(itype, CExprValueType::REAL))
    return convToReal();
  else if (hasType(itype, CExprValueType::STRING))
    return convToString();
  else
    return false;
}

bool
CExprValue::
convToBoolean()
{
  if (isBooleanValue()) return true;

  bool boolean;

  if (! getBooleanValue(boolean))
    return false;

  type_ = CExprValueType::BOOLEAN;
  base_ = CExprValueBaseP(new CExprBooleanValue(boolean));

  return true;
}

bool
CExprValue::
convToInteger()
{
  if (isIntegerValue()) return true;

  long integer;

  if (! getIntegerValue(integer))
    return false;

  type_ = CExprValueType::INTEGER;
  base_ = CExprValueBaseP(new CExprIntegerValue(integer));

  return true;
}

bool
CExprValue::
convToReal()
{
  if (isRealValue()) return true;

  double real;

  if (! getRealValue(real))
    return false;

  type_ = CExprValueType::REAL;
  base_ = CExprValueBaseP(new CExprRealValue(real));

  return true;
}

bool
CExprValue::
convToString()
{
  if (isStringValue()) return true;

  std::string str;

  if (! getStringValue(str))
    return false;

  type_ = CExprValueType::STRING;
  base_ = CExprValueBaseP(new CExprStringValue(str));

  return true;
}

CExprValuePtr
CExprValue::
execUnaryOp(CExpr *expr, CExprOpType op) const
{
  return base_->execUnaryOp(expr, op);
}

CExprValuePtr
CExprValue::
execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const
{
  return base_->execBinaryOp(expr, rhs, op);
}

void
CExprValue::
print(std::ostream &os) const
{
  base_->print(os);
}
