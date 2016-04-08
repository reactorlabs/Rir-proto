#include "test.h"

TEST(Interpreter, capture) {
  // f <- function(a) {
  //   g <- function() {
  //     a
  //   }
  //   g
  // }
  // b <- 1
  // c <- f(b + b)
  // c()

  BasicBlock f0, f1, f2, p0;

  f2 << BC::enter_fun   << (int)0
     << BC::load << a << BC::force
     << BC::leave_fun
     << BC::ret;

  f1 << BC::enter_fun << 1
     << BC::store << a
     // body
     << BC::mkclosure << f2.code()       << L({})
     << BC::store << g
     << BC::load << g << BC::force
     << BC::leave_fun
     << BC::ret;

  p0 << BC::loadenv
     << BC::load << b << BC::force
     << BC::load << b << BC::force
     << BC::add
     << BC::leave_prom
     << BC::ret;

  f0 << BC::enter_fun << (int)0
     << BC::mkclosure << f1.code()       << L({a})
     << BC::store << f
     << BC::push << C(22)
     << BC::store << b
     << BC::load << f << BC::force
     << BC::mkprom << p0.code()
     << BC::call_generic << 1
     << BC::store << c
     << BC::load << c << BC::force
     << BC::call_generic << (int)0
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 44);
}
