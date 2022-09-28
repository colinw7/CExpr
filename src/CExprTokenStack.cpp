#include <CExprTokenStack.h>
#include <CExprFunction.h>

bool
CExprTokenStack::
hasFunction(const std::string &name) const
{
  auto n = stack_.size();

  for (uint i = 0; i < n; ++i) {
    auto ctoken = stack_[i];

    if (ctoken->type() == CExprTokenType::FUNCTION) {
      auto fn = ctoken->getFunction();

      return (fn->name() == name);
    }
  }

  return false;
}
