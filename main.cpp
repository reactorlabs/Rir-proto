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

void t1() {
  Builder code;

  code << BC::push     << C(1)
       << BC::store    << a
       << BC::push     << C(1)
       << BC::load     << a
       << BC::add
       << BC::ret;

  eval(code());
}

void t2() {
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
  p0 << BC::load << b << BC::force
     << BC::ret_prom;

  // outer function
  f0 << BC::mkenv
     << BC::push      << C(666)
     << BC::store     << b
     << BC::mkclosure << f1()
     << BC::store     << f
     // the call sequence
     << BC::load      << f     << BC::force
     << BC::set_fun
     << BC::mkprom    << p0()                                   // push arg
     << BC::call      << 1                                      // call
     << BC::ret;

  eval(f0());
}

void t3() {
  Builder f0, f1;

  // Demo faster calling conventions: inner function has no env

  f1 << BC::add
     << BC::ret;

  f0 << BC::mkenv
     << BC::push           << C(1)
     << BC::push           << C(1)
     << BC::call_fast_leaf << f1()
     << BC::ret;

  eval(f0());
}

int main() {
  t1();
  t2();
  t3();
  return 0;
}
