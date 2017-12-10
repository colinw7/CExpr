#ifndef CExprCompile_H
#define CExprCompile_H

#include <CExprToken.h>

class CExprCompileImpl;

class CExprCompile {
 public:
 ~CExprCompile();

  CExprTokenStack compileIToken(CExprITokenPtr itoken);

  bool hasFunction(const std::string &name) const;

 private:
  friend class CExpr;

  CExprCompile(CExpr *expr);

 private:
  typedef std::unique_ptr<CExprCompileImpl> CExprCompileImplP;

  CExpr*            expr_ { nullptr };
  CExprCompileImplP impl_;
};

#endif
