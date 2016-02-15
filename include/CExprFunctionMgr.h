#ifndef CExprFunctionMgr_H
#define CExprFunctionMgr_H

#include <list>

class CExprFunctionMgr {
 public:
  friend class CExpr;

  typedef std::vector<CExprFunctionPtr> Functions;
  typedef std::vector<CExprFunctionArg> Args;

 public:
 ~CExprFunctionMgr() { }

  void addFunctions();

  CExprFunctionPtr getFunction(const std::string &name);

  void getFunctions(const std::string &name, Functions &functions);

  CExprFunctionPtr addProcFunction(const std::string &name, const std::string &args,
                                   CExprFunctionProc proc);
  CExprFunctionPtr addObjFunction(const std::string &name, const std::string &args,
                                  CExprFunctionObj *proc);
  CExprFunctionPtr addUserFunction(const std::string &name, const std::vector<std::string> &args,
                                   const std::string &proc);

  void removeFunction(const std::string &name);
  void removeFunction(CExprFunctionPtr function);

  void getFunctionNames(std::vector<std::string> &names) const;

  static bool parseArgs(const std::string &argsStr, Args &args, bool &variableArgs);

 private:
  void resetCompiled();

 private:
  CExprFunctionMgr(CExpr *expr);

 private:
  typedef std::list<CExprFunctionPtr> FunctionList;

  CExpr*       expr_;
  FunctionList functions_;
};

#endif
