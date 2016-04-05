#include "interpreter.h"
#include "builder.h"

#include <iostream>

const static Symbol a = Symbols::intern("a");
const static Symbol b = Symbols::intern("b");
const static Symbol f = Symbols::intern("f");

void t1() {
  Builder code;
  code << BC::push_int << 1
       << BC::store    << a
       << BC::push     << new Int(1)
       << BC::load     << a
       << BC::add
       << BC::ret;

  Interpreter run;
  Value* res = run(code());

  std::cout << *res << "\n";
}

void t2() {
  Builder f0;
  Builder f1;
  Builder p0;

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
     << BC::push_int  << 666
     << BC::store     << b
     << BC::mkclosure << f1()
     << BC::store     << f
     // the call sequence
     << BC::load      << f     << BC::force
     << BC::set_fun
     << BC::mkprom    << p0()                                   // push arg
     << BC::call      << 1                                      // call
     << BC::ret;

  Code* f = f0();
  Interpreter i;
  Value* res = i(f);
  std::cout << *res << "\n";
}

int main() {
  t1();
  t2();
  return 0;
}
