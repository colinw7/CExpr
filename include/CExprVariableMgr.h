#ifndef CExprVariableMgr_H
#define CExprVariableMgr_H

#include <list>

class CExpr;

class CExprVariableObj;

class CExprVariableMgr {
 public:
  CExprVariableMgr(CExpr *expr);
 ~CExprVariableMgr() { }

  CExpr *expr() const { return expr_; }

  CExprVariablePtr createVariable(const std::string &name, CExprValuePtr value);

  CExprVariablePtr createUserVariable(const std::string &name, CExprVariableObj *obj);

  CExprVariablePtr getVariable(const std::string &name) const;

  void getVariableNames(std::vector<std::string> &names) const;

 private:
  friend class CExpr;

  void addVariable   (CExprVariablePtr variable);
  void removeVariable(CExprVariablePtr variable);

 private:
  using VariableList = std::list<CExprVariablePtr>;

  CExpr*       expr_ { nullptr };
  VariableList variables_;
};

#endif
