#include <CExprI.h>
#include <CPrintF.h>

CExpr *
CExpr::
instance()
{
  typedef std::unique_ptr<CExpr> CExprP;

  static CExprP instance;

  if (! instance)
    instance = CExprP(new CExpr);

  return instance.get();
}

CExpr::
CExpr() :
 quiet_(false), debug_(false), trace_(false), degrees_(false)
{
  parse_   = CExprParseP  (new CExprParse  (this));
  interp_  = CExprInterpP (new CExprInterp (this));
  compile_ = CExprCompileP(new CExprCompile(this));
  execute_ = CExprExecuteP(new CExprExecute(this));

  operatorMgr_ = CExprOperatorMgrP(new CExprOperatorMgr(this));
  variableMgr_ = CExprVariableMgrP(new CExprVariableMgr(this));
  functionMgr_ = CExprFunctionMgrP(new CExprFunctionMgr(this));

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
  CExprTokenStack pstack = parseLine(str);

  return executePTokenStack(pstack, values);
}

bool
CExpr::
evaluateExpression(const std::string &str, CExprValuePtr &value)
{
  CExprTokenStack pstack = parseLine(str);

  return executePTokenStack(pstack, value);
}

bool
CExpr::
executePTokenStack(const CExprTokenStack &pstack, CExprValueArray &values)
{
  CExprITokenPtr  itoken = interpPTokenStack(pstack);
  CExprTokenStack cstack = compileIToken(itoken);

  return executeCTokenStack(cstack, values);
}

bool
CExpr::
executePTokenStack(const CExprTokenStack &pstack, CExprValuePtr &value)
{
  CExprITokenPtr  itoken = interpPTokenStack(pstack);
  CExprTokenStack cstack = compileIToken(itoken);

  return executeCTokenStack(cstack, value);
}

CExprTokenStack
CExpr::
parseLine(const std::string &line)
{
  CExprTokenStack pstack = parse_->parseLine(line);

  if (getDebug())
    std::cerr << "PTokenStack:" << pstack << std::endl;

  return pstack;
}

CExprITokenPtr
CExpr::
interpPTokenStack(const CExprTokenStack &stack)
{
  CExprITokenPtr itoken = interp_->interpPTokenStack(stack);

  if (getDebug())
    std::cerr << "IToken:" << itoken << std::endl;

  return itoken;
}

CExprTokenStack
CExpr::
compileIToken(CExprITokenPtr itoken)
{
  CExprTokenStack cstack = compile_->compileIToken(itoken);

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
  CExprCompile *compile = compile_.release();
  CExprExecute *execute = execute_.release();

  compiles_.push_back(compile);
  executes_.push_back(execute);

  compile_ = CExprCompileP(new CExprCompile(this));
  execute_ = CExprExecuteP(new CExprExecute(this));
}

void
CExpr::
restoreCompileState()
{
  assert(! compiles_.empty() && ! executes_.empty());

  compile_ = CExprCompileP(compiles_.back());
  execute_ = CExprExecuteP(executes_.back());

  compiles_.pop_back();
  executes_.pop_back();
}

bool
CExpr::
isValidVariableName(const std::string &name) const
{
  int len = name.size();

  if (len == 0)
    return false;

  if (name[0] != '_' && ! isalpha(name[0]))
    return false;

  for (int i = 1; i < len; ++i)
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
addFunction(const std::string &name, const std::vector<std::string> &args,
            const std::string &proc)
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
  CExprOperatorPtr op = operatorMgr_->getOperator(type);

  if (op.isValid())
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

  int  getInt     () const { return getLong(); }
  long getLongLong() const { return getLong(); }

  long getLong() const {
    CExprValuePtr value = nextValue();
    long l = 0;
    if (value.isValid())
      value->getIntegerValue(l);
    return l;
  }

  double getDouble() const {
    CExprValuePtr value = nextValue();
    double r = 0.0;
    if (value.isValid())
      value->getRealValue(r);
    return r;
  }

  std::string getString() const {
    CExprValuePtr value = nextValue();
    std::string s;
    if (value.isValid())
      value->getStringValue(s);
    return s;
  }

 private:
  CExprValuePtr nextValue() const {
    if (iv_ >= values_.size() || ! values_[iv_].isValid()) return CExprValuePtr();
    return values_[iv_++];
  }

 private:
  std::string     fmt_;
  CExprValueArray values_;
  mutable uint    iv_;
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
