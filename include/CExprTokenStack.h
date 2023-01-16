#ifndef CExprTokenStack_H
#define CExprTokenStack_H

#include <CExprTokenBase.h>

class CExprTokenStack {
 public:
  CExprTokenStack() { }

 ~CExprTokenStack() {
    clear();
  }

  bool empty() const {
    return stack_.empty();
  }

  uint getNumTokens() const {
    return uint(stack_.size());
  }

  void addToken(const CExprTokenBaseP &token) {
    stack_.push_back(token);
  }

  const CExprTokenBaseP &getToken(uint i) const {
    return stack_[i];
  }

  CExprTokenBaseP lastToken() const {
    if (stack_.empty())
      return CExprTokenBaseP();

    return stack_[stack_.size() - 1];
  }

  void clear() {
    stack_.clear();
  }

  CExprTokenBaseP pop_front() {
    CExprTokenBaseP token = stack_.front();

    stack_.pop_front();

    return token;
  }

  CExprTokenBaseP pop_back() {
    if (stack_.empty()) return CExprTokenBaseP();

    CExprTokenBaseP token = stack_.back();

    stack_.pop_back();

    return token;
  }

  bool hasFunction(const std::string &name) const;

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CExprTokenStack &stack) {
    stack.print(os);

    return os;
  }

 private:
  using Stack = std::deque<CExprTokenBaseP>;

  Stack stack_;
};

#endif
