#ifndef CExprBValue_H
#define CExprBValue_H

class CExprBooleanValue : public CExprValueBase {
 public:
  CExprBooleanValue(bool boolean) :
   boolean_(boolean) {
  }

  CExprBooleanValue *dup() const override {
    return new CExprBooleanValue(boolean_);
  }

  bool getBooleanValue(bool        &b) const override {
    b = boolean_;                      return true; }
  bool getIntegerValue(long        &l) const override {
    l = (boolean_ ?    1   :     0  ); return true; }
  bool getRealValue   (double      &r) const override {
    r = (boolean_ ?    1.0 :     0.0); return true; }
  bool getStringValue (std::string &s) const override {
    s = (boolean_ ? "true" : "false"); return true; }

  void setBooleanValue(bool b) override { boolean_ = b; }

  CExprValuePtr execUnaryOp (CExpr *expr, CExprOpType op) const override;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const override;

  void print(std::ostream &os) const override {
    os << (boolean_ ? "true" : "false");
  }

 private:
  bool boolean_ { false };
};

#endif
