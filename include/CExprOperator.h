#ifndef CExprOperator_H
#define CExprOperator_H

#include <map>

class CExprOperator;

using CExprOperatorPtr = std::shared_ptr<CExprOperator>;

class CExprOperator {
 public:
  static bool isOperatorChar(char c);

  CExprOperator(CExprOpType type, const std::string &name);

  CExprOpType getType() const { return type_; }
  std::string getName() const { return name_; }

  void print(std::ostream &os) const {
    os << name_;
  }

  friend std::ostream &operator<<(std::ostream &os, const CExprOperator &op) {
    op.print(os);

    return os;
  }

 private:
  CExprOpType type_ { CExprOpType::UNKNOWN };
  std::string name_;
};

#endif
