#ifndef CExprStringValue_H
#define CExprStringValue_H

class CExprStringValue : public CExprValueBase {
 public:
  CExprStringValue(const std::string &str) :
   str_(str) {
  }

  CExprStringValue *dup() const {
    return new CExprStringValue(str_);
  }

  bool getBooleanValue(bool        &b) const;
  bool getIntegerValue(long        &l) const;
  bool getRealValue   (double      &r) const;
  bool getStringValue (std::string &s) const { s = str_; return true; }

  void setStringValue(const std::string &s) { str_ = s; }

  CExprValuePtr execUnaryOp(CExpr *expr, CExprOpType op) const;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const;

  void print(std::ostream &os) const {
    os << str_;
  }

 private:
  std::string str_;
};

#endif
