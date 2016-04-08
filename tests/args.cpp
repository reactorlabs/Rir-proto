#include "test.h"


// f0 calling f1
TEST(Args, missing) {
  BasicBlock f0, f1;

  f1 << BC::enter_fun     << 2
     << BC::store         << b
     << BC::store         << a
     << BC::load          << a  << BC::force
     << BC::load          << a  << BC::force
     << BC::add
     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun      << (int)0
     << BC::mkclosure      << f1.code()    << L({a, b})
     << BC::push           << C(1)
     << BC::call_arg_adapt << 1

     << BC::push           << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}

TEST(Args, named) {
  BasicBlock f0, f1;

  f1 << BC::enter_fun     << 2
     << BC::store         << b
     << BC::store         << a
     << BC::load          << a  << BC::force
     << BC::load          << a  << BC::force
     << BC::add
     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun            << (int)0
     << BC::mkclosure            << f1.code()    << L({a, b})
     << BC::push                 << C(2)
     << BC::push                 << C(1)
     << BC::call_name_arg_adapt  << L({b, a})

     << BC::push                 << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}

TEST(Args, named_pos) {
  BasicBlock f0, f1;

  f1 << BC::enter_fun     << 2
     << BC::store         << b
     << BC::store         << a
     << BC::load          << a  << BC::force
     << BC::load          << a  << BC::force
     << BC::add
     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun            << (int)0
     << BC::mkclosure            << f1.code()    << L({a, b})
     << BC::push                 << C(2)
     << BC::push                 << C(1)
     << BC::call_name_arg_adapt  << L({b, _})

     << BC::push                 << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}

TEST(Args, named_missing) {
  BasicBlock f0, f1;

  f1 << BC::enter_fun     << 2
     << BC::store         << b
     << BC::store         << a
     << BC::load          << a  << BC::force
     << BC::load          << a  << BC::force
     << BC::add
     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun            << (int)0
     << BC::mkclosure            << f1.code()    << L({a, b})
     << BC::push                 << C(1)
     << BC::call_name_arg_adapt  << L({a})

     << BC::push                 << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}
