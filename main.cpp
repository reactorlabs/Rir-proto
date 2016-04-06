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
  f1 << BC::mkenv
     // load args
     << BC::check_arity << 1
     << BC::store       << a
     // body
     << BC::load        << a << BC::force
     << BC::load        << a << BC::force
     << BC::add
     << BC::ret;

  // the "b" promise
  p0 << BC::loadenv
     << BC::load << b << BC::force
     << BC::ret_prom;

  // outer function
  f0 << BC::mkenv
     << BC::push          << C(666)
     << BC::store         << b
     << BC::mkclosure     << f1()
     << BC::store         << f
     // the call sequence
     << BC::load          << f     << BC::force
     << BC::set_fun
     << BC::mkprom        << p0()                                   // push arg
     << BC::call_generic  << 1                                      // call
     << BC::ret;

  eval(f0());
}

// See tests for more examples
int main() {
  t();
  return 0;
}
