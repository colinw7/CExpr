#ifndef CExprVariable_H
#define CExprVariable_H

class CExprVariableObj {
 public:
  CExprVariableObj() { }

  virtual ~CExprVariableObj() { }

  virtual CExprValuePtr get() const = 0;
  virtual void set(CExprValuePtr value) = 0;

  virtual CExprValuePtr subscript(const CExprValueArray &) { return CExprValuePtr(); }
};

class CExprVariable {
 public:
  CExprVariable(const std::string &name, const CExprValuePtr &value);
 ~CExprVariable();

  const std::string &name () const { return name_ ; }
  CExprValuePtr      value() const { return value_; }

  CExprValuePtr getValue() const;
  void setValue(const CExprValuePtr &value);

  void setRealValue   (CExpr *expr, double r);
  void setIntegerValue(CExpr *expr, long   i);

  CExprValueType getValueType() const;

  CExprVariableObj *obj() const { return obj_; }
  void setObj(CExprVariableObj *obj) { obj_ = obj; }

  void print(std::ostream &os) const { os << name_; }

 private:
  std::string       name_;
  CExprValuePtr     value_;
  CExprVariableObj *obj_ { nullptr };
};

#endif
