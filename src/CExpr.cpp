#include <CExprI.h>
#include <CPrintF.h>

CExpr *
CExpr::
instance()
{
  using CExprP = std::unique_ptr<CExpr>;

  static CExprP instance;

  if (! instance)
    instance = std::make_unique<CExpr>();

  return instance.get();
}

CExpr::
CExpr()
{
  parse_   = std::make_unique<CExprParse  >(this);
  interp_  = std::make_unique<CExprInterp >(this);
  compile_ = std::make_shared<CExprCompile>(this);
  execute_ = std::make_shared<CExprExecute>(this);

  operatorMgr_ = std::make_unique<CExprOperatorMgr>(this);
  variableMgr_ = std::make_unique<CExprVariableMgr>(this);
  functionMgr_ = std::make_unique<CExprFunctionMgr>(this);

  functionMgr_->addFunctions();
}

CExpr::
~CExpr()
{
}

bool
CExpr::
evaluateExpression(const std::string &str, CExprValueArray &values)
{
  auto pstack = parseLine(str);

  return executePTokenStack(pstack, values);
}

bool
CExpr::
evaluateExpression(const std::string &str, CExprValuePtr &value)
{
  auto pstack = parseLine(str);

  return executePTokenStack(pstack, value);
}

bool
CExpr::
executePTokenStack(const CExprTokenStack &pstack, CExprValueArray &values)
{
  auto itoken = interpPTokenStack(pstack);
  auto cstack = compileIToken(itoken);

  return executeCTokenStack(cstack, values);
}

bool
CExpr::
executePTokenStack(const CExprTokenStack &pstack, CExprValuePtr &value)
{
  auto itoken = interpPTokenStack(pstack);
  auto cstack = compileIToken(itoken);

  return executeCTokenStack(cstack, value);
}

CExprTokenStack
CExpr::
parseLine(const std::string &line)
{
  auto pstack = parse_->parseLine(line);

  if (getDebug())
    std::cerr << "PTokenStack:" << pstack << std::endl;

  return pstack;
}

CExprITokenPtr
CExpr::
interpPTokenStack(const CExprTokenStack &stack)
{
  auto itoken = interp_->interpPTokenStack(stack);

  if (getDebug())
    std::cerr << "IToken:" << itoken << std::endl;

  return itoken;
}

CExprTokenStack
CExpr::
compileIToken(CExprITokenPtr itoken)
{
  auto cstack = compile_->compileIToken(itoken);

  if (getDebug())
    std::cerr << "CTokenStack:" << cstack << std::endl;

  return cstack;
}

bool
CExpr::
skipExpression(const std::string &line, uint &i)
{
  return parse_->skipExpression(line, i);
}

bool
CExpr::
executeCTokenStack(const CExprTokenStack &stack, CExprValueArray &values)
{
  if (! execute_->executeCTokenStack(stack, values))
    return false;

  if (getDebug()) {
    std::cerr << "Values:";
    for (uint i = 0; i < values.size(); ++i)
      std::cerr << " " << values[i] << std::endl;
    std::cerr << std::endl;
  }

  return true;
}

bool
CExpr::
executeCTokenStack(const CExprTokenStack &stack, CExprValuePtr &value)
{
  if (! execute_->executeCTokenStack(stack, value))
    return false;

  if (getDebug())
    std::cerr << "Value: " << value << std::endl;

  return true;
}

void
CExpr::
saveCompileState()
{
  compiles_.push_back(compile_);
  executes_.push_back(execute_);

  compile_ = std::make_shared<CExprCompile>(this);
  execute_ = std::make_shared<CExprExecute>(this);
}

void
CExpr::
restoreCompileState()
{
  assert(! compiles_.empty() && ! executes_.empty());

  compile_ = compiles_.back();
  execute_ = executes_.back();

  compiles_.pop_back();
  executes_.pop_back();
}

bool
CExpr::
isValidVariableName(const std::string &name) const
{
  auto len = name.size();

  if (len == 0)
    return false;

  if (name[0] != '_' && ! isalpha(name[0]))
    return false;

  for (uint i = 1; i < len; ++i)
    if (! isalnum(name[i]))
      return false;

  return true;
}

CExprVariablePtr
CExpr::
getVariable(const std::string &name) const
{
  return variableMgr_->getVariable(name);
}

CExprVariablePtr
CExpr::
createVariable(const std::string &name, CExprValuePtr value)
{
  return variableMgr_->createVariable(name, value);
}

CExprVariablePtr
CExpr::
createRealVariable(const std::string &name, double x)
{
  return variableMgr_->createVariable(name, createRealValue(x));
}

CExprVariablePtr
CExpr::
createIntegerVariable(const std::string &name, long l)
{
  return variableMgr_->createVariable(name, createIntegerValue(l));
}

CExprVariablePtr
CExpr::
createStringVariable(const std::string &name, const std::string &str)
{
  return variableMgr_->createVariable(name, createStringValue(str));
}

void
CExpr::
removeVariable(const std::string &name)
{
  variableMgr_->removeVariable(variableMgr_->getVariable(name));
}

void
CExpr::
getVariableNames(std::vector<std::string> &names) const
{
  variableMgr_->getVariableNames(names);
}

CExprFunctionPtr
CExpr::
getFunction(const std::string &name)
{
  return functionMgr_->getFunction(name);
}

void
CExpr::
getFunctions(const std::string &name, Functions &functions)
{
  functionMgr_->getFunctions(name, functions);
}

CExprFunctionPtr
CExpr::
addFunction(const std::string &name, const std::vector<std::string> &args, const std::string &proc)
{
  return functionMgr_->addUserFunction(name, args, proc);
}

CExprFunctionPtr
CExpr::
addFunction(const std::string &name, const std::string &argsStr, CExprFunctionObj *func)
{
  return functionMgr_->addObjFunction(name, argsStr, func);
}

void
CExpr::
removeFunction(const std::string &name)
{
  functionMgr_->removeFunction(name);
}

void
CExpr::
getFunctionNames(std::vector<std::string> &names) const
{
  functionMgr_->getFunctionNames(names);
}

CExprTokenBaseP
CExpr::
getOperator(CExprOpType id)
{
  return CExprTokenBaseP(CExprTokenMgrInst->createOperatorToken(id));
}

std::string
CExpr::
getOperatorName(CExprOpType type) const
{
  auto op = operatorMgr_->getOperator(type);

  if (op)
    return op->getName();
  else
    return "<?>";
}

CExprValuePtr
CExpr::
createBooleanValue(bool boolean)
{
  return CExprValuePtr(new CExprValue(CExprBooleanValue(boolean)));
}

CExprValuePtr
CExpr::
createIntegerValue(long integer)
{
  return CExprValuePtr(new CExprValue(CExprIntegerValue(integer)));
}

CExprValuePtr
CExpr::
createRealValue(double real)
{
  return CExprValuePtr(new CExprValue(CExprRealValue(real)));
}

CExprValuePtr
CExpr::
createStringValue(const std::string &str)
{
  return CExprValuePtr(new CExprValue(CExprStringValue(str)));
}

//------

class CExprPrintF : public CPrintF {
 public:
  CExprPrintF(const std::string &fmt) :
   fmt_(fmt) {
  }

  std::string exec(const CExprValueArray &values) {
    setFormatString(fmt_);

    values_ = values;
    iv_     = 0;

    return format();
  }

  int  getInt     () const override { return int(getLong()); }
  long getLongLong() const override { return getLong(); }

  long getLong() const override {
    auto value = nextValue();

    long l = 0;

    if (value)
      value->getIntegerValue(l);

    return l;
  }

  double getDouble() const override {
    auto value = nextValue();

    double r = 0.0;

    if (value)
      value->getRealValue(r);

    return r;
  }

  std::string getString() const override {
    auto value = nextValue();

    std::string s;

    if (value)
      value->getStringValue(s);

    return s;
  }

 private:
  CExprValuePtr nextValue() const {
    if (iv_ >= values_.size() || ! values_[iv_]) return CExprValuePtr();
    return values_[iv_++];
  }

 private:
  std::string     fmt_;
  CExprValueArray values_;
  mutable uint    iv_ { 0 };
};

std::string
CExpr::
printf(const std::string &fmt, const CExprValueArray &values) const
{
  CExprPrintF printf(fmt);

  return printf.exec(values);
}

void
CExpr::
errorMsg(const std::string &msg) const
{
  if (! getQuiet())
    std::cerr << msg << std::endl;
}
