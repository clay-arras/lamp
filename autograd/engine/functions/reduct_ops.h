#pragma once

#ifndef _REDUCT_OPS_H_
#define _REDUCT_OPS_H_

#include "autograd/engine/function.h"

namespace autograd {

struct Summation : public Function {
  int axis;
  explicit Summation(int axis) : axis(axis) {}
  variable_list apply(const variable_list& inputs) override;
};

struct SummationBackward : public Function {
  int axis;
  explicit SummationBackward(int axis) : axis(axis) {}
  variable_list apply(const variable_list& gradOutputs) override;
};

}  // namespace autograd

#endif  // _REDUCT_OPS_H_
