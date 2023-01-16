#ifndef CExprCompile_H
#define CExprCompile_H

#include <CExprToken.h>

class CExprCompileImpl;

class CExprCompile {
 public:
  CExprCompile(CExpr *expr);
 ~CExprCompile();

  CExpr *expr() const { return expr_; }

  CExprTokenStack compileIToken(CExprITokenPtr itoken);

  bool hasFunction(const std::string &name) const;

 private:
  using CExprCompileImplP = std::unique_ptr<CExprCompileImpl>;

  CExpr*            expr_ { nullptr };
  CExprCompileImplP impl_;
};

#endif
