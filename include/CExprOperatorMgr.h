#ifndef CExprOperatorMgr_H
#define CExprOperatorMgr_H

#include <map>

class CExprOperatorMgr {
 public:
 ~CExprOperatorMgr() { }

  CExprOperatorPtr getOperator(CExprOpType type) const;

 private:
  friend class CExpr;

  CExprOperatorMgr(CExpr *expr);

 private:
  typedef std::map<CExprOpType,CExprOperatorPtr> OperatorMap;

  CExpr*      expr_ { nullptr };
  OperatorMap operator_map_;
};

#endif
