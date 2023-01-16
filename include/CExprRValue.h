#ifndef CExprRealValue_H
#define CExprRealValue_H

class CExprRealValue : public CExprValueBase {
 public:
  CExprRealValue(double real) :
   real_(real) {
  }

  CExprRealValue *dup() const override {
    return new CExprRealValue(real_);
  }

  bool getBooleanValue(bool        &b) const override { b = (real_ != 0); return true; }
  bool getIntegerValue(long        &l) const override { l = long(real_) ; return true; }
  bool getRealValue   (double      &r) const override { r = real_       ; return true; }
  bool getStringValue (std::string &s) const override;

  void setRealValue(double r) override { real_ = r; }

  CExprValuePtr execUnaryOp (CExpr *expr, CExprOpType op) const override;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const override;

  void print(std::ostream &os) const override {
    os << real_;
  }

 private:
  double realPower  (double real1, double real2, int *error_code) const;
  double realModulus(double real1, double real2, int *error_code) const;

 private:
  double real_ { 0.0 };
};

#endif
