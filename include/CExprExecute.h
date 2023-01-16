#ifndef CExprExecute_H
#define CExprExecute_H

class CExpr;
class CExprExecuteImpl;

class CExprExecute {
 public:
  CExprExecute(CExpr *expr);
 ~CExprExecute();

  CExpr *expr() const { return expr_; }

  bool executeCTokenStack(const CExprTokenStack &stack, CExprValueArray &values);
  bool executeCTokenStack(const CExprTokenStack &stack, CExprValuePtr &value);

 private:
  using CExprExecuteImplP = std::unique_ptr<CExprExecuteImpl>;

  CExpr*            expr_ { nullptr };
  CExprExecuteImplP impl_;
};

#endif
