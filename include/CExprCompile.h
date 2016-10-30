#ifndef CExprCompile_H
#define CExprCompile_H

#include <CExprToken.h>
#include <CAutoPtr.h>

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
  CExpr*                     expr_;
  CAutoPtr<CExprCompileImpl> impl_;
};

#endif
