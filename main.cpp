#include "interpreter.h"
#include "builder.h"

#include <iostream>

const static Symbol a = Symbols::intern("a");
const static Symbol b = Symbols::intern("b");

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

  f1 << BC::mkenv
     << BC::store << a
     << BC::load  << a << BC::force
     << BC::load  << a << BC::force
     << BC::add
     << BC::ret;

  p0 << BC::load << b << BC::force
     << BC::ret_prom;

  f0 << BC::mkenv
     << BC::push_int  << 666   << BC::store     << b
     << BC::mkclosure << f1()  << BC::set_fun                   // evaluate f
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
