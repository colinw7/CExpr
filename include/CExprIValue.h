#ifndef CExprIntegerValue_H
#define CExprIntegerValue_H

class CExprIntegerValue : public CExprValueBase {
 public:
  CExprIntegerValue(long integer) :
   integer_(integer) {
  }

  CExprIntegerValue *dup() const override {
    return new CExprIntegerValue(integer_);
  }

  bool getBooleanValue(bool        &b) const override { b = (integer_ != 0) ; return true; }
  bool getIntegerValue(long        &l) const override { l = integer_        ; return true; }
  bool getRealValue   (double      &r) const override { r = double(integer_); return true; }
  bool getStringValue (std::string &s) const override;

  void setIntegerValue(long l) override { integer_ = l; }

  CExprValuePtr execUnaryOp (CExpr *expr, CExprOpType op) const override;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const override;

  void print(std::ostream &os) const override {
    os << integer_;
  }

 private:
  long integerPower(long integer1, long integer2, int *error_code) const;
  long realToInteger(double real, int *error_code) const;

 private:
  long integer_ { 0 };
};

#endif
