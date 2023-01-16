#ifndef CExprStringValue_H
#define CExprStringValue_H

class CExprStringValue : public CExprValueBase {
 public:
  CExprStringValue(const std::string &str) :
   str_(str) {
  }

  CExprStringValue *dup() const override {
    return new CExprStringValue(str_);
  }

  bool getBooleanValue(bool        &b) const override;
  bool getIntegerValue(long        &l) const override;
  bool getRealValue   (double      &r) const override;
  bool getStringValue (std::string &s) const override { s = str_; return true; }

  void setStringValue(const std::string &s) override { str_ = s; }

  CExprValuePtr execUnaryOp (CExpr *expr, CExprOpType op) const override;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const override;

  void print(std::ostream &os) const override {
    os << str_;
  }

 private:
  std::string str_;
};

#endif
