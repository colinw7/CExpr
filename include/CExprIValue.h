#ifndef CExprIntegerValue_H
#define CExprIntegerValue_H

class CExprIntegerValue : public CExprValueBase {
 public:
  CExprIntegerValue(long integer) :
   integer_(integer) {
  }

  CExprIntegerValue *dup() const {
    return new CExprIntegerValue(integer_);
  }

  bool getBooleanValue(bool        &b) const { b = (integer_ != 0); return true; }
  bool getIntegerValue(long        &l) const { l = integer_       ; return true; }
  bool getRealValue   (double      &r) const { r = integer_       ; return true; }
  bool getStringValue (std::string &s) const;

  void setIntegerValue(long l) { integer_ = l; }

  CExprValuePtr execUnaryOp(CExpr *expr, CExprOpType op) const;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const;

  void print(std::ostream &os) const {
    os << integer_;
  }

 private:
  long integerPower(long integer1, long integer2, int *error_code) const;
  long realToInteger(double real, int *error_code) const;

 private:
  long integer_;
};

#endif
