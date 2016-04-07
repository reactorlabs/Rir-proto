#include "interpreter.h"
#include "builder.h"

#include <iostream>

const static Symbol a = Symbols::intern("a");
const static Symbol b = Symbols::intern("b");
const static Symbol f = Symbols::intern("f");

void eval(Code* c) {
  Interpreter i;
  Value* res = i(c);
  std::cout << *res << "\n";
}

void t() {
  Builder f0, f1, p0;

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
     << BC::load << b << BC::force
     << BC::update_prom
     << BC::ret;

  // outer function
  f0 << BC::enter_fun     << (int)0
     << BC::push          << C(666)
     << BC::store         << b
     << BC::mkclosure     << f1()
     << BC::store         << f
     // the call sequence
     << BC::load          << f     << BC::force
     << BC::mkprom        << p0()                                   // push arg
     << BC::call_generic  << 1                                      // call
     << BC::leave_fun
     << BC::ret;

  eval(f0());
}

// See tests for more examples
int main() {
  t();
  return 0;
}
