#ifndef CExprParse_H
#define CExprParse_H

#include <CExprToken.h>

class CExpr;
class CExprParseImpl;

class CExprParse {
 public:
 ~CExprParse();

  CExprTokenStack parseFile(const std::string &filename);
  CExprTokenStack parseFile(FILE *fp);
  CExprTokenStack parseLine(const std::string &str);

  bool skipExpression(const std::string &str, uint &i);

 private:
  friend class CExpr;

  CExprParse(CExpr *expr);

 private:
  CExpr*                   expr_;
  CAutoPtr<CExprParseImpl> impl_;
};

#endif
