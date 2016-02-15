#ifndef CExprVariable_H
#define CExprVariable_H

class CExprVariable {
 public:
 ~CExprVariable();

  const std::string &name () const { return name_ ; }
  CExprValuePtr      value() const { return value_; }

  void setValue(const CExprValuePtr &value);

  void setRealValue   (CExpr *expr, double r);
  void setIntegerValue(CExpr *expr, int    i);

  const CExprValuePtr &getValue() const { return value_; }

  CExprValueType getValueType() const;

 private:
  CExprVariable(const std::string &name, const CExprValuePtr &value);

 private:
  friend class CExprVariableMgr;

  std::string   name_;
  CExprValuePtr value_;
};

#endif
