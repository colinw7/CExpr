#ifndef CEXPR_H
#define CEXPR_H

enum class CExprOpType {
  UNKNOWN           = -1,
  OPEN_RBRACKET     = 0,
  CLOSE_RBRACKET    = 1,
  LOGICAL_NOT       = 2,
  BIT_NOT           = 3,
  INCREMENT         = 4,
  DECREMENT         = 5,
  UNARY_PLUS        = 6,
  UNARY_MINUS       = 7,
  POWER             = 8,
  TIMES             = 9,
  DIVIDE            = 10,
  MODULUS           = 11,
  PLUS              = 12,
  MINUS             = 13,
  BIT_LSHIFT        = 14,
  BIT_RSHIFT        = 15,
  LESS              = 16,
  LESS_EQUAL        = 17,
  GREATER           = 18,
  GREATER_EQUAL     = 19,
  EQUAL             = 20,
  NOT_EQUAL         = 21,
  APPROX_EQUAL      = 22,
  BIT_AND           = 23,
  BIT_XOR           = 24,
  BIT_OR            = 25,
  LOGICAL_AND       = 26,
  LOGICAL_OR        = 27,
  QUESTION          = 28,
  COLON             = 29,
  EQUALS            = 30,
  PLUS_EQUALS       = 31,
  MINUS_EQUALS      = 32,
  TIMES_EQUALS      = 33,
  DIVIDE_EQUALS     = 34,
  MODULUS_EQUALS    = 35,
  BIT_AND_EQUALS    = 36,
  BIT_XOR_EQUALS    = 37,
  BIT_OR_EQUALS     = 38,
  BIT_LSHIFT_EQUALS = 39,
  BIT_RSHIFT_EQUALS = 40,
  COMMA             = 41,
  START_BLOCK       = 42,
  END_BLOCK         = 43
};

enum class CExprValueType {
  NONE    = 0,
  BOOLEAN = (1<<0),
  INTEGER = (1<<1),
  REAL    = (1<<2),
  STRING  = (1<<3),
  NUL     = (1<<4),

  ANY = (BOOLEAN | INTEGER | REAL | STRING)
};

class CExpr;
class CExprValue;
class CExprIToken;
class CExprVariable;
class CExprFunction;

#include <CRefPtr.h>
#include <CAutoPtr.h>
#include <CStrUtil.h>

typedef CRefPtr<CExprValue>    CExprValuePtr;
typedef CRefPtr<CExprIToken>   CExprITokenPtr;
typedef CRefPtr<CExprVariable> CExprVariablePtr;
typedef CRefPtr<CExprFunction> CExprFunctionPtr;

typedef std::vector<CExprValuePtr> CExprValueArray;

typedef CExprValuePtr (*CExprFunctionProc)(CExpr *expr, const CExprValueArray &values);

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

enum class CExprTokenType {
  UNKNOWN = -1,

  NONE = 0,

  IDENTIFIER = 1,
  OPERATOR   = 2,
  INTEGER    = 4,
  REAL       = 5,
  STRING     = 6,
  COMPLEX    = 7,
  FUNCTION   = 8,
  VALUE      = 9,
  BLOCK      = 10
};

enum class CExprITokenType {
  TOKEN_TYPE                = 100,
  EXPRESSION                = 101,
  ASSIGNMENT_EXPRESSION     = 102,
  CONDITIONAL_EXPRESSION    = 103,
  LOGICAL_OR_EXPRESSION     = 104,
  LOGICAL_AND_EXPRESSION    = 105,
  INCLUSIVE_OR_EXPRESSION   = 106,
  EXCLUSIVE_OR_EXPRESSION   = 107,
  AND_EXPRESSION            = 108,
  EQUALITY_EXPRESSION       = 109,
  RELATIONAL_EXPRESSION     = 110,
  SHIFT_EXPRESSION          = 111,
  ADDITIVE_EXPRESSION       = 112,
  MULTIPLICATIVE_EXPRESSION = 113,
  POWER_EXPRESSION          = 114,
  UNARY_EXPRESSION          = 115,
  POSTFIX_EXPRESSION        = 116,
  PRIMARY_EXPRESSION        = 117,
  ARGUMENT_EXPRESSION_LIST  = 118,
  DUMMY_EXPRESSION          = 119,
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
 ~CExpr() { }

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
  typedef std::vector<CExprCompile *> Compiles;
  typedef std::vector<CExprExecute *> Executes;

  bool                       quiet_;
  bool                       debug_;
  bool                       trace_;
  bool                       degrees_;
  CAutoPtr<CExprParse>       parse_;
  CAutoPtr<CExprInterp>      interp_;
  CAutoPtr<CExprCompile>     compile_;
  CAutoPtr<CExprExecute>     execute_;
  Compiles                   compiles_;
  Executes                   executes_;
  CAutoPtr<CExprOperatorMgr> operatorMgr_;
  CAutoPtr<CExprVariableMgr> variableMgr_;
  CAutoPtr<CExprFunctionMgr> functionMgr_;
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
