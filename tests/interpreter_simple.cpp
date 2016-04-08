#include "test.h"

TEST(Interpreter, simple) {
  BasicBlock code;

  code << BC::enter_fun  << (int)0
       << BC::push       << C(1)
       << BC::store      << a
       << BC::push       << C(1)
       << BC::load       << a
       << BC::add
       << BC::leave_fun
       << BC::ret;

  checkInt(code.code(), 2);
}

TEST(Interpreter, two_fun) {
  BasicBlock f0, f1;

  // inner fun
  f1 << BC::enter_fun   << (int)0
     << BC::push        << C(1)
     << BC::leave_fun
     << BC::ret;

  // outer function
  f0 << BC::enter_fun     << (int)0
     << BC::mkclosure     << f1.code()      << L({})
     << BC::call_generic  << (int)0
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 1);
}


TEST(Interpreter, two_fun_prom) {
  BasicBlock f0, f1, p0;

  // Function is:
  //
  // function() {
  //   f1 <- function(a) a + a
  //   b <- 666
  //   f1(b)
  // }

  // inner fun
  f1 << BC::enter_fun   << 1
     // load args
     << BC::store       << a
     // body
     << BC::load        << a << BC::force
     << BC::load        << a << BC::force
     << BC::add
     << BC::leave_fun
     << BC::ret;

  // the "b" promise
  p0 << BC::loadenv
     << BC::load << b
     << BC::force
     << BC::leave_prom
     << BC::ret;

  // outer function
  f0 << BC::enter_fun     << (int)0
     << BC::push          << C(666)
     << BC::store         << b
     << BC::mkclosure     << f1.code()       << L({a})
     << BC::store         << f
     // the call sequence
     << BC::load          << f          << BC::force
     << BC::mkprom        << p0.code()                              // push arg
     << BC::call_generic  << 1                                      // call
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 1332);
}

TEST(Interpreter, fastcall) {
  BasicBlock f0, f1;

  // Demo faster calling conventions
  f1 << BC::mkenv
     << BC::add
     << BC::leave
     << BC::ret;

  f0 << BC::enter_fun      << (int)0
     << BC::push           << C(1)
     << BC::push           << C(1)
     << BC::pushenv
     << BC::call_fast      << f1.code()
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 2);
}


TEST(Interpreter, fastcall_noenv) {
  BasicBlock f0, f1;

  // Demo faster calling conventions: inner function has no env
  f1 << BC::add
     << BC::ret;

  f0 << BC::enter_fun    << (int)0
     << BC::push         << C(1)
     << BC::push         << C(1)
     << BC::call_fast    << f1.code()
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 2);
}
