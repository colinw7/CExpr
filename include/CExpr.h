#ifndef CEXPR_H
#define CEXPR_H

#include <CExprTypes.h>
#include <CStrUtil.h>

class CExprValueBase {
 public:
  virtual ~CExprValueBase() { }

  virtual CExprValueBase *dup() const { return 0; }

  virtual bool getBooleanValue(bool        &) const { return false; }
  virtual bool getIntegerValue(long        &) const { return false; }
  virtual bool getRealValue   (double      &) const { return false; }
  virtual bool getStringValue (std::string &) const { return false; }

  virtual void setBooleanValue(bool               ) { assert(false); }
  virtual void setIntegerValue(long               ) { assert(false); }
  virtual void setRealValue   (double             ) { assert(false); }
  virtual void setStringValue (const std::string &) { assert(false); }

  virtual CExprValuePtr execUnaryOp(CExpr *, CExprOpType) const {
    return CExprValuePtr();
  }

  virtual CExprValuePtr execBinaryOp(CExpr *, CExprValuePtr, CExprOpType) const {
    return CExprValuePtr();
  }

  virtual void print(std::ostream &os) const {
    os << "<null>";
  }
};

//-------

#include <CExprBValue.h>
#include <CExprIValue.h>
#include <CExprRValue.h>
#include <CExprSValue.h>
#include <CExprValue.h>
#include <CExprOperator.h>
#include <CExprToken.h>
#include <CExprParse.h>
#include <CExprVariable.h>
#include <CExprInterp.h>
#include <CExprCompile.h>
#include <CExprFunction.h>
#include <CExprExecute.h>
#include <CExprStrgen.h>
#include <CExprError.h>
#include <CExprFunctionMgr.h>
#include <CExprOperatorMgr.h>
#include <CExprVariableMgr.h>

//-------

//#define CExprInst CExpr::instance()

class CExpr {
 public:
  typedef std::vector<CExprFunctionPtr> Functions;
  typedef std::vector<std::string>      StringArray;

 public:
  static CExpr *instance();

  CExpr();
 ~CExpr();

  bool getQuiet() const { return quiet_; }
  void setQuiet(bool b) { quiet_ = b; }

  bool getDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool getTrace() const { return trace_; }
  void setTrace(bool b) { trace_ = b; }

  bool getDegrees() const { return degrees_; }
  void setDegrees(bool b) { degrees_ = b; }

  bool evaluateExpression(const std::string &str, CExprValueArray &values);
  bool evaluateExpression(const std::string &str, CExprValuePtr &value);

  bool executePTokenStack(const CExprTokenStack &stack, CExprValueArray &values);
  bool executePTokenStack(const CExprTokenStack &stack, CExprValuePtr &value);

  CExprTokenStack parseLine(const std::string &line);
  CExprITokenPtr  interpPTokenStack(const CExprTokenStack &stack);
  CExprTokenStack compileIToken(CExprITokenPtr itoken);

  bool skipExpression(const std::string &line, uint &i);

  bool executeCTokenStack(const CExprTokenStack &stack, CExprValueArray &values);
  bool executeCTokenStack(const CExprTokenStack &stack, CExprValuePtr &value);

  void saveCompileState();
  void restoreCompileState();

  bool isValidVariableName(const std::string &name) const;

  CExprVariablePtr getVariable     (const std::string &name) const;
  CExprVariablePtr createVariable  (const std::string &name, CExprValuePtr value);
  void             removeVariable  (const std::string &name);
  void             getVariableNames(StringArray &names) const;

  CExprVariablePtr createRealVariable   (const std::string &name, double x);
  CExprVariablePtr createIntegerVariable(const std::string &name, long l);
  CExprVariablePtr createStringVariable (const std::string &name, const std::string &str);

  CExprFunctionPtr getFunction(const std::string &name);
  void             getFunctions(const std::string &name, Functions &functions);

  CExprFunctionPtr addFunction(const std::string &name, const StringArray &args,
                               const std::string &proc);
  CExprFunctionPtr addFunction(const std::string &name, const std::string &argsStr,
                               CExprFunctionObj *proc);

  void removeFunction(const std::string &name);

  void getFunctionNames(StringArray &names) const;

  CExprTokenBaseP getOperator(CExprOpType id);

  std::string getOperatorName(CExprOpType type) const;

  CExprValuePtr createBooleanValue(bool b);
  CExprValuePtr createIntegerValue(long i);
  CExprValuePtr createRealValue   (double r);
  CExprValuePtr createStringValue (const std::string &s);

  std::string printf(const std::string &fmt, const CExprValueArray &values) const;

  void errorMsg(const std::string &msg) const;

 private:
  typedef std::vector<CExprCompile *>       Compiles;
  typedef std::vector<CExprExecute *>       Executes;
  typedef std::unique_ptr<CExprParse>       CExprParseP;
  typedef std::unique_ptr<CExprInterp>      CExprInterpP;
  typedef std::unique_ptr<CExprCompile>     CExprCompileP;
  typedef std::unique_ptr<CExprExecute>     CExprExecuteP;
  typedef std::unique_ptr<CExprOperatorMgr> CExprOperatorMgrP;
  typedef std::unique_ptr<CExprVariableMgr> CExprVariableMgrP;
  typedef std::unique_ptr<CExprFunctionMgr> CExprFunctionMgrP;

  bool              quiet_ { false };
  bool              debug_ { false };
  bool              trace_ { false };
  bool              degrees_ { false };
  CExprParseP       parse_;
  CExprInterpP      interp_;
  CExprCompileP     compile_;
  CExprExecuteP     execute_;
  Compiles          compiles_;
  Executes          executes_;
  CExprOperatorMgrP operatorMgr_;
  CExprVariableMgrP variableMgr_;
  CExprFunctionMgrP functionMgr_;
};

//------

template<typename T>
class CExprUtil {
 public:
  static bool getTypeValue(CExprValuePtr value, T &v);

  static CExprValuePtr createValue(const T &v);

  static std::string argTypeStr();
};

template<>
class CExprUtil<bool> {
 public:
  static bool getTypeValue(CExprValuePtr value, bool &b) { return value->getBooleanValue(b); }

  static CExprValuePtr createValue(CExpr *expr, const bool &b) {
    return expr->createBooleanValue(b);
  }

  static std::string argTypeStr() { return "b"; }
};

template<>
class CExprUtil<long> {
 public:
  static bool getTypeValue(CExprValuePtr value, long &i) { return value->getIntegerValue(i); }

  static CExprValuePtr createValue(CExpr *expr, const long &i) {
    return expr->createIntegerValue(i);
  }

  static std::string argTypeStr() { return "i"; }
};

template<>
class CExprUtil<double> {
 public:
  static bool getTypeValue(CExprValuePtr value, double &r) { return value->getRealValue(r); }

  static CExprValuePtr createValue(CExpr *expr, const double &r) {
    return expr->createRealValue(r);
  }

  static std::string argTypeStr() { return "r"; }
};

template<>
class CExprUtil<std::string> {
 public:
  static bool getTypeValue(CExprValuePtr value, std::string &s) {
    return value->getStringValue(s);
  }

  static CExprValuePtr createValue(CExpr *expr, const std::string &s) {
    return expr->createStringValue(s);
  }

  static std::string argTypeStr() { return "s"; }
};

#endif