#ifndef CExprRealValue_H
#define CExprRealValue_H

class CExprRealValue : public CExprValueBase {
 public:
  CExprRealValue(double real) :
   real_(real) {
  }

  CExprRealValue *dup() const {
    return new CExprRealValue(real_);
  }

  bool getBooleanValue(bool        &b) const { b = (real_ != 0); return true; }
  bool getIntegerValue(long        &l) const { l = long(real_) ; return true; }
  bool getRealValue   (double      &r) const { r = real_       ; return true; }
  bool getStringValue (std::string &s) const;

  void setRealValue(double r) { real_ = r; }

  CExprValuePtr execUnaryOp (CExpr *expr, CExprOpType op) const;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const;

  void print(std::ostream &os) const {
    os << real_;
  }

 private:
  double realPower  (double real1, double real2, int *error_code) const;
  double realModulus(double real1, double real2, int *error_code) const;

 private:
  double real_ { 0.0 };
};

#endif
