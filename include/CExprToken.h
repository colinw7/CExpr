#ifndef CExprToken_H
#define CExprToken_H

#include <CExpr.h>
#include <deque>
#include <iostream>

class CExprTokenStack;

class CExprTokenBase {
 public:
  CExprTokenBase(CExprTokenType type) :
   type_(type) {
  }

  virtual ~CExprTokenBase() { }

  CExprTokenType type() const { return type_; }

  //virtual CExprTokenBase *dup() const = 0;

  const std::string     &getIdentifier() const;
  CExprOpType            getOperator  () const;
  long                   getInteger   () const;
  double                 getReal      () const;
  const std::string     &getString    () const;
  CExprFunctionPtr       getFunction  () const;
  CExprValuePtr          getValue     () const;
  const CExprTokenStack &getBlock     () const;

  void printQualified(std::ostream &os) const;

  virtual void print(std::ostream &os) const = 0;

 protected:
  CExprTokenType type_;
};

typedef CRefPtr<CExprTokenBase> CExprTokenBaseP;

//---

class CExprTokenUnknown : public CExprTokenBase {
 public:
  CExprTokenUnknown() : CExprTokenBase(CExprTokenType::UNKNOWN) { }

  //CExprTokenUnknown *dup() const { return new CExprTokenUnknown(); }

  void print(std::ostream &os) const { os << "unknown"; }
};

class CExprTokenIdentifier : public CExprTokenBase {
 public:
  CExprTokenIdentifier(const std::string &identifier) :
   CExprTokenBase(CExprTokenType::IDENTIFIER), identifier_(identifier) {
  }

  const std::string &getIdentifier() const { return identifier_; }

  //CExprTokenIdentifier *dup() const { return new CExprTokenIdentifier(identifier_); }

  void print(std::ostream &os) const { os << identifier_; }

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

  //CExprTokenOperator *dup() const { return new CExprTokenOperator(type_); }

  void print(std::ostream &os) const;

 private:
  CExprOpType type_;
};

//---

class CExprTokenInteger : public CExprTokenBase {
 public:
  CExprTokenInteger(long integer) :
   CExprTokenBase(CExprTokenType::INTEGER), integer_(integer) {
  }

  long getInteger() const { return integer_; }

  //CExprTokenInteger *dup() const { return new CExprTokenInteger(integer_); }

  void print(std::ostream &os) const { os << integer_; }

 private:
  long integer_;
};

//---

class CExprTokenReal : public CExprTokenBase {
 public:
  CExprTokenReal(double real) :
   CExprTokenBase(CExprTokenType::REAL), real_(real) {
  }

  double getReal() const { return real_; }

  //CExprTokenReal *dup() const { return new CExprTokenReal(real_); }

  void print(std::ostream &os) const { os << real_; }

 private:
  double real_;
};

//---

class CExprTokenString : public CExprTokenBase {
 public:
  CExprTokenString(const std::string &str) :
   CExprTokenBase(CExprTokenType::STRING), str_(str) {
  }

  const std::string &getString() const { return str_; }

  //CExprTokenString *dup() const { return new CExprTokenString(str_); }

  void print(std::ostream &os) const { os << "\"" << str_ << "\""; }

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

  //CExprTokenFunction *dup() const { return new CExprTokenFunction(function_); }

  void print(std::ostream &os) const;

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

  //CExprTokenValue *dup() const { return new CExprTokenValue(value_); }

  void print(std::ostream &os) const { os << *value_; }

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

  //CExprTokenBlock *dup() const { return new CExprTokenBlock(stack_); }

  void print(std::ostream &os) const { stack_.print(os); }

 private:
  CExprTokenStack stack_;
};

#endif
