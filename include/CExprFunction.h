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
  CExprFunctionArg() = default;

  CExprFunctionArg(CExprValueType type1) :
   type(type1) {
  }

  CExprValueType type { CExprValueType::NONE };
};

//------

class CExprFunction {
 public:
  CExprFunction(const std::string &name) :
   name_(name) {
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

  virtual bool hasFunction(const std::string &) const { return false; }

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
  bool        builtin_      { false };
  bool        variableArgs_ { false };
};

//------

class CExprProcFunction : public CExprFunction {
 public:
  using Args = std::vector<CExprFunctionArg>;

 public:
  CExprProcFunction(const std::string &name, const Args &args, CExprFunctionProc proc);
  CExprProcFunction(const std::string &name, const std::string &argsStr, CExprFunctionProc proc);

  uint numArgs() const override { return uint(args_.size()); }

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
  using Args = std::vector<CExprFunctionArg>;

 public:
  CExprObjFunction(const std::string &name, const Args &args, CExprFunctionObj *proc);
 ~CExprObjFunction();

  uint numArgs() const override { return uint(args_.size()); }

  CExprValueType argType(uint i) const override {
    return (i < args_.size() ? args_[i].type : CExprValueType::NUL);
  }

  bool checkValues(const CExprValueArray &values) const override;

  CExprValuePtr exec(CExpr *expr, const CExprValueArray &values) override;

 private:
  Args              args_;
  CExprFunctionObj *proc_ { nullptr };
};

//------

class CExprUserFunction : public CExprFunction {
 public:
  using Args = std::vector<std::string>;

 public:
  CExprUserFunction(const std::string &name, const Args &args, const std::string &proc);

  uint numArgs() const override { return uint(args_.size()); }

  const std::string &proc() const { return proc_; }

  bool checkValues(const CExprValueArray &) const override;

  void reset() override;

  bool isCompiled() const { return compiled_; }

  CExprValuePtr exec(CExpr *expr, const CExprValueArray &values) override;

  bool hasFunction(const std::string &name) const override {
    return compiled_ && cstack_.hasFunction(name);
  }

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
  mutable bool            compiled_ { false };
  mutable CExprTokenStack pstack_;
  mutable CExprITokenPtr  itoken_;
  mutable CExprTokenStack cstack_;
};

//------

#endif
