#ifndef CExprInterp_H
#define CExprInterp_H

class CExprIToken {
 public:
  static CExprITokenPtr createIToken(CExprITokenType itype) {
    return CExprITokenPtr(new CExprIToken(itype));
  }

  static CExprITokenPtr createIToken(const CExprTokenBaseP &ptoken) {
    return CExprITokenPtr(new CExprIToken(ptoken));
  }

  CExprIToken(CExprITokenType itype);
  CExprIToken(const CExprTokenBaseP &ptoken);

 ~CExprIToken() { }

  const CExprTokenBaseP &base() const { return base_; }

  CExprTokenType getType() const;

  CExprITokenType getIType() const { return itype_; }

  //CExprIToken *dup() const;

  const std::string &getIdentifier() const { return base()->getIdentifier(); }
  CExprOpType        getOperator  () const { return base()->getOperator  (); }
  long               getInteger   () const { return base()->getInteger   (); }
  double             getReal      () const { return base()->getReal      (); }
  const std::string &getString    () const { return base()->getString    (); }

  uint getNumChildren() const {
    return children_.size();
  }

  CExprITokenPtr getChild(uint i) {
    return children_[i];
  }

  void addChild(CExprITokenPtr child) {
    children_.push_back(child);
  }

  void removeAllChildren() {
    children_.clear();
  }

  uint countITokenChildrenOfType(CExprITokenType type);

  void print(std::ostream &os, bool children=true) const;

  friend std::ostream &operator<<(std::ostream &os, const CExprIToken &token) {
    token.print(os);

    return os;
  }

 private:
  typedef std::vector<CExprITokenPtr> ITokenList;

  CExprITokenType itype_;
  CExprTokenBaseP base_;
  ITokenList      children_;
};

//---

class CExprITokenStack {
 public:
  CExprITokenStack() { }

  void addToken(CExprITokenPtr token) {
    stack_.push_back(token);
  }

  CExprITokenPtr pop_back() {
    if (stack_.empty()) return CExprITokenPtr();

    CExprITokenPtr token = stack_.back();

    stack_.pop_back();

    return token;
  }

  uint getNumTokens() const {
    return stack_.size();
  }

  CExprITokenPtr getToken(uint i) const {
    return stack_[i];
  }

  void clear() {
    stack_.clear();
  }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CExprITokenStack &stack) {
    stack.print(os);

    return os;
  }

 private:
  typedef std::vector<CExprITokenPtr> Stack;

  Stack stack_;
};

//---

class CExpr;
class CExprInterpImpl;

class CExprInterp {
 public:
 ~CExprInterp();

  CExprITokenPtr interpPTokenStack(const CExprTokenStack &stack);

 private:
  friend class CExpr;

  CExprInterp(CExpr *expr);

 private:
  CExpr*                    expr_;
  CAutoPtr<CExprInterpImpl> impl_;
};

#endif
