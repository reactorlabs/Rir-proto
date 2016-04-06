#include <gtest/gtest.h>

#include "../interpreter.h"
#include "../builder.h"

#include <iostream>

const static Symbol a = Symbols::intern("a");
const static Symbol b = Symbols::intern("b");
const static Symbol f = Symbols::intern("f");

Value* eval(Code* c) {
  Interpreter i;
  return i(c);
}

void checkInt(Value* v, int i) {
  Int* iv = dynamic_cast<Int*>(v);
  EXPECT_NE(iv, nullptr);
  EXPECT_EQ(iv->val, i);
}

TEST(Interpreter, simple) {
  Builder code;

  code << BC::mkenv
       << BC::push     << C(1)
       << BC::store    << a
       << BC::push     << C(1)
       << BC::load     << a
       << BC::add
       << BC::ret;

  Value* res = eval(code());
  checkInt(res, 2);
}

TEST(Interpreter, two_fun_prom) {
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

  Value* res = eval(f0());

  checkInt(res, 1332);
}

TEST(Interpreter, fastcall) {
  Builder f0, f1;

  // Demo faster calling conventions
  f1 << BC::mkenv
     << BC::add
     << BC::ret;

  f0 << BC::mkenv
     << BC::push           << C(1)
     << BC::push           << C(1)
     << BC::call_fast_env  << f1()
     << BC::ret;

  Value* res = eval(f0());
  checkInt(res, 2);
}


TEST(Interpreter, fastcall_noenv) {
  Builder f0, f1;

  // Demo faster calling conventions: inner function has no env
  f1 << BC::add
     << BC::ret;

  f0 << BC::mkenv
     << BC::push         << C(1)
     << BC::push         << C(1)
     << BC::call_fast    << f1()
     << BC::ret;

  Value* res = eval(f0());
  checkInt(res, 2);
}
