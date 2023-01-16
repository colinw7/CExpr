#ifndef CExprToken_H
#define CExprToken_H

#include <CExprTokenBase.h>
#include <CExpr.h>

//---

class CExprTokenUnknown : public CExprTokenBase {
 public:
  CExprTokenUnknown() : CExprTokenBase(CExprTokenType::UNKNOWN) { }

  //CExprTokenUnknown *dup() const override { return new CExprTokenUnknown(); }

  void print(std::ostream &os) const override { os << "unknown"; }
};

class CExprTokenIdentifier : public CExprTokenBase {
 public:
  CExprTokenIdentifier(const std::string &identifier) :
   CExprTokenBase(CExprTokenType::IDENTIFIER), identifier_(identifier) {
  }

  const std::string &getIdentifier() const { return identifier_; }

  //CExprTokenIdentifier *dup() const override { return new CExprTokenIdentifier(identifier_); }

  void print(std::ostream &os) const override { os << identifier_; }

 private:
  std::string identifier_;
};

//---

class CExprTokenOperator : public CExprTokenBase {
 public:
  CExprTokenOperator(CExprOpType type) :
   CExprTokenBase(CExprTokenType::OPERATOR), type_(type) {
  }

  CExprOpType getType() const { return type_; }

  //CExprTokenOperator *dup() const override { return new CExprTokenOperator(type_); }

  void print(std::ostream &os) const override;

 private:
  CExprOpType type_ { CExprOpType::UNKNOWN };
};

//---

class CExprTokenInteger : public CExprTokenBase {
 public:
  CExprTokenInteger(long integer) :
   CExprTokenBase(CExprTokenType::INTEGER), integer_(integer) {
  }

  long getInteger() const { return integer_; }

  //CExprTokenInteger *dup() const override { return new CExprTokenInteger(integer_); }

  void print(std::ostream &os) const override { os << integer_; }

 private:
  long integer_ { 0 };
};

//---

class CExprTokenReal : public CExprTokenBase {
 public:
  CExprTokenReal(double real) :
   CExprTokenBase(CExprTokenType::REAL), real_(real) {
  }

  double getReal() const { return real_; }

  //CExprTokenReal *dup() const override { return new CExprTokenReal(real_); }

  void print(std::ostream &os) const override { os << real_; }

 private:
  double real_ { 0.0 };
};

//---

class CExprTokenString : public CExprTokenBase {
 public:
  CExprTokenString(const std::string &str) :
   CExprTokenBase(CExprTokenType::STRING), str_(str) {
  }

  const std::string &getString() const { return str_; }

  //CExprTokenString *dup() const override { return new CExprTokenString(str_); }

  void print(std::ostream &os) const override { os << "\"" << str_ << "\""; }

 private:
  std::string str_;
};

//---

class CExprTokenFunction : public CExprTokenBase {
 public:
  CExprTokenFunction(CExprFunctionPtr function) :
   CExprTokenBase(CExprTokenType::FUNCTION), function_(function) {
  }

  CExprFunctionPtr getFunction() const { return function_; }

  //CExprTokenFunction *dup() const override { return new CExprTokenFunction(function_); }

  void print(std::ostream &os) const override;

 private:
  CExprFunctionPtr function_;
};

//---

class CExprTokenValue : public CExprTokenBase {
 public:
  CExprTokenValue(CExprValuePtr value) :
   CExprTokenBase(CExprTokenType::VALUE), value_(value) {
  }

  CExprValuePtr getValue() const { return value_; }

  //CExprTokenValue *dup() const override { return new CExprTokenValue(value_); }

  void print(std::ostream &os) const override { os << *value_; }

 private:
  CExprValuePtr value_;
};

//---

#include <CExprTokenStack.h>

//---

class CExprTokenBlock : public CExprTokenBase {
 public:
  CExprTokenBlock(const CExprTokenStack &stack) :
   CExprTokenBase(CExprTokenType::BLOCK), stack_(stack) {
  }

  const CExprTokenStack &stack() const { return stack_; }

  //CExprTokenBlock *dup() const override { return new CExprTokenBlock(stack_); }

  void print(std::ostream &os) const override { stack_.print(os); }

 private:
  CExprTokenStack stack_;
};

#endif
