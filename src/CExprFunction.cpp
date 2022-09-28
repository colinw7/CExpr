#include <CExprI.h>
#include <cmath>
#include <cstdlib>

struct CExprBuiltinFunction {
  const char        *name;
  const char        *args;
  CExprFunctionProc  proc;
};

#define CEXPR_REAL_1_FUNC(NAME, F) \
static CExprValuePtr \
CExprFunction##NAME(CExpr *expr, const CExprValueArray &values) { \
  assert(values.size() == 1); \
  double real; \
  if (values[0]->getRealValue(real)) \
    return expr->createRealValue(F(real)); \
  return CExprValuePtr(); \
}

#define CEXPR_ANGLE_1_FUNC(NAME, F) \
static CExprValuePtr \
CExprFunction##NAME(CExpr *expr, const CExprValueArray &values) { \
  assert(values.size() == 1); \
  double real; \
  if (values[0]->getRealValue(real)) { \
    if (expr->getDegrees()) \
      real = M_PI*real/180.0; \
    return expr->createRealValue(F(real)); \
  } \
  return CExprValuePtr(); \
}

#define CEXPR_REAL_INTEGER_1_FUNC(NAME, RF, IF) \
static CExprValuePtr \
CExprFunction##NAME(CExpr *expr, const CExprValueArray &values) { \
  assert(values.size() == 1); \
  if      (values[0]->isRealValue()) { \
    double real; \
    if (values[0]->getRealValue(real)) \
      return expr->createRealValue(RF(real)); \
  } \
  else if (values[0]->isIntegerValue()) { \
    long integer; \
    if (values[0]->getIntegerValue(integer)) \
      return expr->createIntegerValue(IF(integer)); \
  } \
  else { \
    assert(false); \
  } \
  return CExprValuePtr(); \
}

CEXPR_REAL_1_FUNC(Sqrt , ::sqrt)
CEXPR_REAL_1_FUNC(Exp  , ::exp)
CEXPR_REAL_1_FUNC(Log  , ::log)
CEXPR_REAL_1_FUNC(Log10, ::log10)

CEXPR_ANGLE_1_FUNC(Sin, ::sin)
CEXPR_ANGLE_1_FUNC(Cos, ::cos)
CEXPR_ANGLE_1_FUNC(Tan, ::tan)

CEXPR_ANGLE_1_FUNC(ASin, ::asin)
CEXPR_ANGLE_1_FUNC(ACos, ::acos)
CEXPR_ANGLE_1_FUNC(ATan, ::atan)

CEXPR_REAL_INTEGER_1_FUNC(Abs, ::fabs, std::abs)

static CExprBuiltinFunction
builtinFns[] = {
  { "sqrt" , "r" , CExprFunctionSqrt  },
  { "exp"  , "r" , CExprFunctionExp   },
  { "log"  , "r" , CExprFunctionLog   },
  { "log10", "r" , CExprFunctionLog10 },
  { "sin"  , "r" , CExprFunctionSin   },
  { "cos"  , "r" , CExprFunctionCos   },
  { "tan"  , "r" , CExprFunctionTan   },
  { "abs"  , "ri", CExprFunctionAbs   },
  { "asin" , "r" , CExprFunctionASin  },
  { "acos" , "r" , CExprFunctionACos  },
  { "atan" , "r" , CExprFunctionATan  },
  { ""     , ""  , 0                  }
};

//------

CExprFunctionMgr::
CExprFunctionMgr(CExpr *expr) :
 expr_(expr)
{
}

CExprFunctionMgr::
~CExprFunctionMgr()
{
}

void
CExprFunctionMgr::
addFunctions()
{
  for (uint i = 0; builtinFns[i].proc; ++i) {
    CExprFunctionPtr function =
      addProcFunction(builtinFns[i].name, builtinFns[i].args, builtinFns[i].proc);

    function->setBuiltin(true);
  }
}

CExprFunctionPtr
CExprFunctionMgr::
getFunction(const std::string &name)
{
  for (const auto &func : functions_)
    if (func->name() == name)
      return func;

  return CExprFunctionPtr();
}

void
CExprFunctionMgr::
getFunctions(const std::string &name, Functions &functions)
{
  for (const auto &func : functions_)
    if (func->name() == name)
      functions.push_back(func);
}

CExprFunctionPtr
CExprFunctionMgr::
addProcFunction(const std::string &name, const std::string &argsStr, CExprFunctionProc proc)
{
  Args args;
  bool variableArgs;

  (void) parseArgs(argsStr, args, variableArgs);

  CExprFunctionPtr function(new CExprProcFunction(name, args, proc));

  function->setVariableArgs(variableArgs);

  removeFunction(name);

  functions_.push_back(function);

  resetCompiled();

  return function;
}

CExprFunctionPtr
CExprFunctionMgr::
addObjFunction(const std::string &name, const std::string &argsStr, CExprFunctionObj *proc)
{
  Args args;
  bool variableArgs;

  (void) parseArgs(argsStr, args, variableArgs);

  CExprFunctionPtr function(new CExprObjFunction(name, args, proc));

  function->setVariableArgs(variableArgs);

  if (! proc->isOverload())
    removeFunction(name);

  functions_.push_back(function);

  resetCompiled();

  return function;
}

CExprFunctionPtr
CExprFunctionMgr::
addUserFunction(const std::string &name, const std::vector<std::string> &args,
                const std::string &proc)
{
  CExprFunctionPtr function(new CExprUserFunction(name, args, proc));

  removeFunction(name);

  functions_.push_back(function);

  resetCompiled();

  return function;
}

void
CExprFunctionMgr::
removeFunction(const std::string &name)
{
  removeFunction(getFunction(name));
}

void
CExprFunctionMgr::
removeFunction(CExprFunctionPtr function)
{
  if (function.isValid())
    functions_.remove(function);
}

void
CExprFunctionMgr::
getFunctionNames(std::vector<std::string> &names) const
{
  for (const auto &func : functions_)
    names.push_back(func->name());
}

void
CExprFunctionMgr::
resetCompiled()
{
  for (const auto &func : functions_)
    func->reset();
}

bool
CExprFunctionMgr::
parseArgs(const std::string &argsStr, Args &args, bool &variableArgs)
{
  variableArgs = false;

  bool rc = true;

  std::vector<std::string> args1;

  CStrUtil::addTokens(argsStr, args1, ", ");

  auto num_args = args1.size();

  for (uint i = 0; i < num_args; ++i) {
    const std::string &arg = args1[i];

    if (arg == "..." && i == num_args - 1) {
      variableArgs = true;
      break;
    }

    uint types = uint(CExprValueType::NONE);

    auto len = arg.size();

    for (uint j = 0; j < len; j++) {
      char c = arg[j];

      if      (c == 'b') types |= uint(CExprValueType::BOOLEAN);
      else if (c == 'i') types |= uint(CExprValueType::INTEGER);
      else if (c == 'r') types |= uint(CExprValueType::REAL);
      else if (c == 's') types |= uint(CExprValueType::STRING);
      else if (c == 'n') types |= uint(CExprValueType::NUL);
      else {
        CExpr::instance()->
          errorMsg("Invalid argument type char '" + std::string(&c, 1) + "'");
        rc = false;
      }
    }

    args.push_back(CExprFunctionArg(CExprValueType(types)));
  }

  return rc;
}

//----------

CExprProcFunction::
CExprProcFunction(const std::string &name, const Args &args, CExprFunctionProc proc) :
 CExprFunction(name), args_(args), proc_(proc)
{
}

bool
CExprProcFunction::
checkValues(const CExprValueArray &values) const
{
  return (values.size() == numArgs());
}

CExprValuePtr
CExprProcFunction::
exec(CExpr *expr, const CExprValueArray &values)
{
  assert(checkValues(values));

  return (*proc_)(expr, values);
}

//----------

CExprObjFunction::
CExprObjFunction(const std::string &name, const Args &args, CExprFunctionObj *proc) :
 CExprFunction(name), args_(args), proc_(proc)
{
}

CExprObjFunction::
~CExprObjFunction()
{
  delete proc_;
}

bool
CExprObjFunction::
checkValues(const CExprValueArray &values) const
{
  if (isVariableArgs())
    return (values.size() >= numArgs());
  else
    return (values.size() == numArgs());
}

CExprValuePtr
CExprObjFunction::
exec(CExpr *expr, const CExprValueArray &values)
{
  assert(checkValues(values));

  return (*proc_)(expr, values);
}

//----------

CExprUserFunction::
CExprUserFunction(const std::string &name, const Args &args, const std::string &proc) :
 CExprFunction(name), args_(args), proc_(proc), compiled_(false)
{
}

bool
CExprUserFunction::
checkValues(const CExprValueArray &values) const
{
  return (values.size() >= numArgs());
}

void
CExprUserFunction::
reset()
{
  compiled_ = false;

  pstack_.clear();
  cstack_.clear();

  itoken_ = CExprITokenPtr();
}

CExprValuePtr
CExprUserFunction::
exec(CExpr *expr, const CExprValueArray &values)
{
  assert(checkValues(values));

  //---

  if (! compiled_) {
    pstack_ = expr->parseLine(proc_);
    itoken_ = expr->interpPTokenStack(pstack_);
    cstack_ = expr->compileIToken(itoken_);

    compiled_ = true;
  }

  //---

  typedef std::map<std::string,CExprValuePtr> VarValues;

  VarValues varValues;

  // set arg values (save previous values)
  for (uint i = 0; i < numArgs(); ++i) {
    const auto &arg = args_[i];

    auto var = expr->getVariable(arg);

    if (var.isValid()) {
      varValues[arg] = var->getValue();

      var->setValue(values[i]);
    }
    else {
      varValues[arg] = CExprValuePtr();

      expr->createVariable(arg, values[i]);
    }
  }

  // run proc
  expr->saveCompileState();

  CExprValuePtr value;

//if (! expr->evaluateExpression(proc_, value))
//  value = CExprValuePtr();
//if (! expr->executePTokenStack(pstack_, value))
//  value = CExprValuePtr();
  if (! expr->executeCTokenStack(cstack_, value))
    value = CExprValuePtr();

  expr->restoreCompileState();

  // restore variables
  for (const auto &v : varValues) {
    const auto &varName = v.first;
    auto        value1  = v.second;

    if (value1.isValid()) {
      auto var = expr->getVariable(varName);

      var->setValue(value1);
    }
    else
      expr->removeVariable(varName);
  }

  return value;
}
