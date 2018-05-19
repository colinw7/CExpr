#ifndef CExprError_H
#define CExprError_H

enum class CExprErrorType {
  NONE                          = 0,
  ZERO_TO_NEG_POWER_UNDEF       = 1,
  POWER_FAILED                  = 2,
  REAL_TOO_BIG_FOR_INTEGER      = 3,
  NAN_OPERATION                 = 4,
  NON_INTEGER_POWER_OF_NEGATIVE = 5,
  DIVIDE_BY_ZERO                = 6
};

class CExprErrorData {
 public:
  CExprErrorData() { }

  void setLastError(const std::string &message) {
    if (message == "") {
      lastError_ = "";

      hasError_ = false;

      return;
    }

    if (hasError_)
      return;

    lastError_ = message;

    hasError_ = true;
  }

  bool isError() const {
    return hasError_;
  }

  const std::string &getLastError() const {
    return lastError_;
  }

 private:
  bool        hasError_ { false };
  std::string lastError_;
};

#endif
