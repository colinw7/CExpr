#ifndef CExprTokenBase_H
#define CExprTokenBase_H

#include <CExprTypes.h>
#include <deque>
#include <iostream>
#include <complex>

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
  CExprTokenType type_ { CExprTokenType::UNKNOWN };
};

typedef CRefPtr<CExprTokenBase> CExprTokenBaseP;

#endif
