#include "test.h"

// f0 calling f1
TEST(Inline, baseline_1) {
  BasicBlock f0, f1;

  f1 << BC::enter_fun     << 2
     << BC::add
     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun      << (int)0
     << BC::mkclosure      << f1.code()     << L({a, a})
     << BC::push           << C(1)
     << BC::push           << C(1)
     << BC::call_generic   << 2

     << BC::push           << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}

// f1 is inline into f0, f1 closure is still materialized
TEST(Inline, inline_1) {
  BasicBlock f0, f1;

  f1 << BC::enter_fun     << 2
     << BC::add
     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun      << (int)0
     << BC::mkclosure      << f1.code()     << L({a, a})
     << BC::push           << C(1)
     << BC::push           << C(1)

     // f1 inlined verbatim
     << BC::enter_fun     << 2
     << BC::add
     << BC::leave_fun

     << BC::push           << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}

// closure is not materialized, we use fastcall
TEST(Inline, baseline_fastcall_1) {
  BasicBlock f0, f1;

  f1 << BC::mkenv
     << BC::add
     << BC::leave
     << BC::ret;

  f0 << BC::enter_fun      << (int)0
     << BC::push           << C(1)
     << BC::push           << C(1)
     << BC::pushenv
     << BC::call_fast      << f1.code()

     << BC::push           << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}

// fastcall inlined -> no f1 codeobject needed anymore
TEST(Inline, inline_fastcall_1) {
  BasicBlock f0;

  f0 << BC::enter_fun      << (int)0
     << BC::push           << C(1)
     << BC::push           << C(1)
     << BC::pushenv

     // f1 copied verbatim
     << BC::mkenv
     << BC::add
     << BC::leave

     << BC::push           << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 3);
}


TEST(Inline, baseline_prom_1) {
  BasicBlock f0, f1, p0;

  p0 << BC::loadenv
     << BC::load << a << BC::force
     << BC::leave_prom
     << BC::ret;

  f1 << BC::enter_fun     << 1
     << BC::store         << b
     << BC::load          << b
     << BC::force
     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun      << (int)0
     << BC::mkclosure      << f1.code()    << L({b})
     << BC::mkprom         << p0.code()
     << BC::push           << C(42)
     << BC::store          << a
     << BC::call_generic   << 1
     << BC::push           << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 43);
}

TEST(Inline, inline_prom_1) {
  BasicBlock f0, f1, p0;

  f1 << BC::enter_fun     << 1
     << BC::store         << b
     << BC::load          << b

     // Inlined p0 instead of BC::force
     << BC::loadenv
     << BC::load << a << BC::force
     << BC::leave_prom

     << BC::leave_fun
     << BC::ret;

  f0 << BC::enter_fun      << (int)0
     << BC::mkclosure      << f1.code()    << L({b})
     << BC::mkprom         << p0.code()
     << BC::push           << C(42)
     << BC::store          << a
     << BC::call_generic   << 1
     << BC::push           << C(1)
     << BC::add
     << BC::leave_fun
     << BC::ret;

  checkInt(f0.code(), 43);
}


