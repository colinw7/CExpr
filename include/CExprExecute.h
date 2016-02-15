#ifndef CExprExecute_H
#define CExprExecute_H

class CExpr;
class CExprExecuteImpl;

class CExprExecute {
 public:
 ~CExprExecute();

  bool executeCTokenStack(const CExprTokenStack &stack, CExprValueArray &values);
  bool executeCTokenStack(const CExprTokenStack &stack, CExprValuePtr &value);

 private:
  friend class CExpr;

  CExprExecute(CExpr *expr);

 private:
  CExpr*                     expr_;
  CAutoPtr<CExprExecuteImpl> impl_;
};

#endif
