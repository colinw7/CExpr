#ifndef CExprParse_H
#define CExprParse_H

#include <CExprToken.h>

class CExpr;
class CExprParseImpl;

class CExprParse {
 public:
  CExprParse(CExpr *expr);
 ~CExprParse();

  CExpr *expr() const { return expr_; }

  CExprTokenStack parseFile(const std::string &filename);
  CExprTokenStack parseFile(FILE *fp);
  CExprTokenStack parseLine(const std::string &str);

  bool skipExpression(const std::string &str, uint &i);

 private:
  using CExprParseImplP = std::unique_ptr<CExprParseImpl>;

  CExpr*          expr_ { nullptr };
  CExprParseImplP impl_;
};

#endif
