#ifndef CExprOperatorMgr_H
#define CExprOperatorMgr_H

#include <map>

class CExprOperatorMgr {
 public:
  CExprOperatorMgr(CExpr *expr);
 ~CExprOperatorMgr() { }

  CExpr *expr() const { return expr_; }

  CExprOperatorPtr getOperator(CExprOpType type) const;

 private:
  using OperatorMap = std::map<CExprOpType, CExprOperatorPtr>;

  CExpr*      expr_ { nullptr };
  OperatorMap operator_map_;
};

#endif
