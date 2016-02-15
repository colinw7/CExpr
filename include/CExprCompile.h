#ifndef CExprCompile_H
#define CExprCompile_H

#include <CExprToken.h>
#include <CAutoPtr.h>

class CExprCompileImpl;

class CExprCompile {
 public:
 ~CExprCompile();

  CExprTokenStack compileIToken(CExprITokenPtr itoken);

 private:
  friend class CExpr;

  CExprCompile(CExpr *expr);

 private:
  CExpr*                     expr_;
  CAutoPtr<CExprCompileImpl> impl_;
};

#endif
