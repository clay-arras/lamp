#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <memory>
#include <vector>

class Variable;
struct Function;

using variable_list = std::vector<Variable>;
using function_list = std::vector<std::pair<std::shared_ptr<Function>, int>>;

struct Function : public std::enable_shared_from_this<Function> {
  std::vector<std::pair<std::shared_ptr<Function>, int>> function_list;

  Function() = default;
  virtual ~Function() = default;

  virtual variable_list apply(const variable_list& inputs) = 0;
  variable_list operator()(const variable_list& inputs) {
    return apply(inputs);
  }
};

#endif  // _FUNCTION_H_