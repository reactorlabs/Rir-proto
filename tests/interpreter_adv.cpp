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

  Builder f0, f1, f2, p0;

  f2 << BC::mkenv
     << BC::check_arity << (int)0
     << BC::load << a << BC::force
     << BC::ret;

  f1 << BC::mkenv
     << BC::check_arity << 1
     << BC::store << a
     // body
     << BC::mkclosure << f2()
     << BC::store << g
     << BC::load << g << BC::force
     << BC::ret;

  p0 << BC::loadenv
     << BC::load << b << BC::force
     << BC::load << b << BC::force
     << BC::add
     << BC::ret_prom;

  f0 << BC::mkenv
     << BC::mkclosure << f1()
     << BC::store << f
     << BC::push << C(22)
     << BC::store << b
     << BC::load << f << BC::force
     << BC::set_fun
     << BC::mkprom << p0()
     << BC::call_generic << 1
     << BC::store << c
     << BC::load << c << BC::force
     << BC::set_fun
     << BC::call_generic << (int)0
     << BC::ret;

  checkInt(f0(), 44);
}