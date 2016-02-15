#ifndef CExprFunction_H
#define CExprFunction_H

class CExpr;

//------

class CExprFunctionObj {
 public:
  CExprFunctionObj() { }

  virtual ~CExprFunctionObj() { }

  virtual CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) = 0;

  virtual bool isOverload() const { return false; }
};

//------

struct CExprFunctionArg {
  CExprFunctionArg(CExprValueType type1=CExprValueType::NONE) :
   type(type1) {
  }

  CExprValueType type;
};

//------

class CExprFunction {
 public:
  CExprFunction(const std::string &name) :
   name_(name), builtin_(false), variableArgs_(false) {
  }

  virtual ~CExprFunction() { }

  const std::string &name() const { return name_; }

  bool isBuiltin() const { return builtin_; }
  void setBuiltin(bool b) { builtin_ = b; }

  bool isVariableArgs() const { return variableArgs_; }
  void setVariableArgs(bool b) { variableArgs_ = b; }

  virtual uint numArgs() const = 0;

  virtual CExprValueType argType(uint) const { return CExprValueType::ANY; }

  virtual bool checkValues(const CExprValueArray &) const { return true; }

  virtual void reset() { }

  virtual CExprValuePtr exec(CExpr *expr, const CExprValueArray &values) = 0;

  friend std::ostream &operator<<(std::ostream &os, const CExprFunction &fn) {
    fn.print(os);

    return os;
  }

  virtual void print(std::ostream &os, bool expanded=true) const {
    os << name_ << "(";

    if (expanded && variableArgs_)
      os << "...";

    os << ")";
  }

 protected:
  std::string name_;
  bool        builtin_;
  bool        variableArgs_;
};

//------

class CExprProcFunction : public CExprFunction {
 public:
  typedef std::vector<CExprFunctionArg> Args;

 public:
  CExprProcFunction(const std::string &name, const Args &args, CExprFunctionProc proc);
  CExprProcFunction(const std::string &name, const std::string &argsStr, CExprFunctionProc proc);

  uint numArgs() const override { return args_.size(); }

  CExprValueType argType(uint i) const override {
    return (i < args_.size() ? args_[i].type : CExprValueType::NUL);
  }

  bool checkValues(const CExprValueArray &) const override;

  CExprValuePtr exec(CExpr *expr, const CExprValueArray &values) override;

 private:
  Args              args_;
  CExprFunctionProc proc_;
};

//------

class CExprObjFunction : public CExprFunction {
 public:
  typedef std::vector<CExprFunctionArg> Args;

 public:
  CExprObjFunction(const std::string &name, const Args &args, CExprFunctionObj *proc);
 ~CExprObjFunction();

  uint numArgs() const override { return args_.size(); }

  CExprValueType argType(uint i) const override {
    return (i < args_.size() ? args_[i].type : CExprValueType::NUL);
  }

  bool checkValues(const CExprValueArray &values) const override;

  CExprValuePtr exec(CExpr *expr, const CExprValueArray &values) override;

 private:
  Args              args_;
  CExprFunctionObj *proc_;
};

//------

class CExprUserFunction : public CExprFunction {
 public:
  typedef std::vector<std::string> Args;

 public:
  CExprUserFunction(const std::string &name, const Args &args, const std::string &proc);

  uint numArgs() const override { return args_.size(); }

  const std::string &proc() const { return proc_; }

  bool checkValues(const CExprValueArray &) const override;

  void reset() override;

  CExprValuePtr exec(CExpr *expr, const CExprValueArray &values) override;

  void print(std::ostream &os, bool expanded=true) const override {
    os << name_ << "(";

    for (uint i = 0; i < args_.size(); ++i) {
      if (i > 0) os << ", ";

      os << args_[i];
    }

    os << ")";

    if (expanded)
      os << "= " << proc_;
  }

 private:
  Args                    args_;
  std::string             proc_;
  mutable bool            compiled_;
  mutable CExprTokenStack pstack_;
  mutable CExprITokenPtr  itoken_;
  mutable CExprTokenStack cstack_;
};

//------

#endif
