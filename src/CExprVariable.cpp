#include <CExprI.h>

CExprVariableMgr::
CExprVariableMgr(CExpr *expr) :
 expr_(expr)
{
}

CExprVariablePtr
CExprVariableMgr::
createVariable(const std::string &name, CExprValuePtr value)
{
  auto variable = getVariable(name);

  if (! variable) {
    variable = std::make_shared<CExprVariable>(name, value);

    addVariable(variable);
  }
  else
    variable->setValue(value);

  return variable;
}

CExprVariablePtr
CExprVariableMgr::
createUserVariable(const std::string &name, CExprVariableObj *obj)
{
  auto variable = getVariable(name);

  if (! variable) {
    variable = std::make_shared<CExprVariable>(name, CExprValuePtr());

    addVariable(variable);
  }

  variable->setObj(obj);

  return variable;
}

CExprVariablePtr
CExprVariableMgr::
getVariable(const std::string &name) const
{
  for (const auto &var : variables_)
    if (var->name() == name)
      return var;

  return CExprVariablePtr();
}

void
CExprVariableMgr::
addVariable(CExprVariablePtr variable)
{
  variables_.push_back(variable);
}

void
CExprVariableMgr::
removeVariable(CExprVariablePtr variable)
{
  variables_.remove(variable);
}

void
CExprVariableMgr::
getVariableNames(std::vector<std::string> &names) const
{
  for (const auto &var : variables_)
    names.push_back(var->name());
}

//------

CExprVariable::
CExprVariable(const std::string &name, const CExprValuePtr &value) :
 name_(name), value_(value)
{
}

CExprVariable::
~CExprVariable()
{
}

CExprValuePtr
CExprVariable::
getValue() const
{
  if (obj_)
    return obj_->get();
  else
    return value_;
}

void
CExprVariable::
setValue(const CExprValuePtr &value)
{
  if (obj_)
    obj_->set(value);
  else
    value_ = value;
}

void
CExprVariable::
setRealValue(CExpr *expr, double r)
{
  if (! obj_ && value_->isRealValue())
    value_->setRealValue(r);
  else
    setValue(expr->createRealValue(r));
}

void
CExprVariable::
setIntegerValue(CExpr *expr, long i)
{
  if (! obj_ && value_->isIntegerValue())
    value_->setIntegerValue(i);
  else
    setValue(expr->createIntegerValue(i));
}

CExprValueType
CExprVariable::
getValueType() const
{
  return getValue()->getType();
}
