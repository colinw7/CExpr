#ifndef CExprTokenMgr_H
#define CExprTokenMgr_H

#define CExprTokenMgrInst CExprTokenMgr::instance()

class CExprTokenMgr {
 public:
  static CExprTokenMgr *instance() {
    static CExprTokenMgr *instance;

    if (! instance)
      instance = new CExprTokenMgr;

    return instance;
  }

  CExprTokenIdentifier *createIdentifierToken(const std::string &identifier) {
    return new CExprTokenIdentifier(identifier);
  }

  CExprTokenOperator *createOperatorToken(CExprOpType id) {
    return new CExprTokenOperator(id);
  }

  CExprTokenInteger *createIntegerToken(long integer) {
    return new CExprTokenInteger(integer);
  }

  CExprTokenReal *createRealToken(double real) {
    return new CExprTokenReal(real);
  }

  CExprTokenString *createStringToken(const std::string &str) {
    return new CExprTokenString(str);
  }

  CExprTokenFunction *createFunctionToken(CExprFunctionPtr function) {
    return new CExprTokenFunction(function);
  }

  CExprTokenValue *createValueToken(CExprValuePtr value) {
    return new CExprTokenValue(value);
  }

  CExprTokenUnknown *createUnknownToken() {
    return new CExprTokenUnknown();
  }
};

#endif
