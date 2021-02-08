#ifndef CExprBValue_H
#define CExprBValue_H

class CExprBooleanValue : public CExprValueBase {
 public:
  CExprBooleanValue(bool boolean) :
   boolean_(boolean) {
  }

  CExprBooleanValue *dup() const {
    return new CExprBooleanValue(boolean_);
  }

  bool getBooleanValue(bool        &b) const { b = boolean_;                      return true; }
  bool getIntegerValue(long        &l) const { l = (boolean_ ?    1   :     0  ); return true; }
  bool getRealValue   (double      &r) const { r = (boolean_ ?    1.0 :     0.0); return true; }
  bool getStringValue (std::string &s) const { s = (boolean_ ? "true" : "false"); return true; }

  void setBooleanValue(bool b) { boolean_ = b; }

  CExprValuePtr execUnaryOp (CExpr *expr, CExprOpType op) const;
  CExprValuePtr execBinaryOp(CExpr *expr, CExprValuePtr rhs, CExprOpType op) const;

  void print(std::ostream &os) const {
    os << (boolean_ ? "true" : "false");
  }

 private:
  bool boolean_ { false };
};

#endif
