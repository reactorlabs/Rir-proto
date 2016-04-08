#include "test.h"

// f0 calling f1
TEST(ControlFlow, jump) {
  BasicBlock b1, b2, b3, b4;

  b1 << BC::enter_fun << (int)0
     << BC::push      << C(1)
     << BC::jump      << &b2;

  b2 << BC::push      << C(1)
     << BC::jump      << &b3;

  b3 << BC::add
     << BC::jump      << &b4;

  b4 << BC::leave_fun
     << BC::ret;

  checkInt(b1.code(), 2);
}


