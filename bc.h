#ifndef BC_H
#define BC_H

#include <cstdint>
#include <cassert>

typedef uint8_t B;

enum class BC : B {
  // Create a new environment with the tos as enclosing environment
  // immediate: 0
  // stack: -1
  mkenv,

  // Load tos as new environment
  // immediate: 0
  // stack: -1
  loadenv,

  // Push a constant
  // immediate: sizeof(Value*) bytes
  // stack: +1
  push,

  // Read a symbol and store tos under that name in the current rho
  // immediate: sizeof(Symbol)
  // stack: -1
  store,

  // Read a symbol and load it from rho
  // immediate: sizeof(Symbol)
  // stack: +1
  load,

  // Force tos value: if tos is a promise, then:
  //   * if it has a value replace tos by that value
  //   * otherwise transfer control to the promise. prom_ret will be
  //     responsible for placing the correct value on the stack.
  // immediate: 0
  // stack: -1, +1 (if evaluated promise); 0 (otherwise)
  force,

  // Wrap a code object in a closure with current rho
  // immediate: sizeof(Code*)
  // stack: +1
  mkclosure,

  // Wrap a code object in a promise with current rho
  // immediate: sizeof(Code*)
  // stack: +1
  mkprom,

  // Set the function register to tos
  // immediate: 0
  // stack: -1
  set_fun,

  // Push arity and rho to the stack and call the function in the function
  //   register (args are passed via stack)
  // immediate: sizeof(Int*) as the arity
  // stack: 2
  call_generic,

  // Directly invoke a code object, push rho to stack
  // immediate: sizeof(Code*)
  // stack: 1
  call_fast_env,

  // Directly invoke a code object which does not have access to parent env
  // immediate: sizeof(Code*)
  // stack: 0
  call_fast,

  // Verify that the arity register contains the expected number of args
  // immediate: sizeof(unsigned)
  // stack: -1
  check_arity,

  // Return from function (return value is passed on stack)
  // immediate: 0
  // stack: 0
  ret,

  // Return from promise: stack contains: [ ret_val, promise, ... ]
  //   Promise is removed from stack and updated with ret_val. ret_val is
  //   pushed on stack as return value.
  // immediate: 0
  // stack: -2, +1
  ret_prom,

  // Add two numbers
  // immediate: 0
  // stack: -2, +1
  add,


  num_of,
};

enum class ImmediateType {
  None,
  Symbol,
  Code,
  Value,
  Int,
};

namespace {
  static ImmediateType* getImmediateTypeList() {
    ImmediateType* itl = new ImmediateType[(B)BC::num_of];
    itl[(int)BC::mkenv]         = ImmediateType::None;
    itl[(int)BC::loadenv]       = ImmediateType::None;
    itl[(int)BC::push]          = ImmediateType::Value;
    itl[(int)BC::store]         = ImmediateType::Symbol;
    itl[(int)BC::load]          = ImmediateType::Symbol;
    itl[(int)BC::force]         = ImmediateType::None;
    itl[(int)BC::mkclosure]     = ImmediateType::Code;
    itl[(int)BC::mkprom]        = ImmediateType::Code;
    itl[(int)BC::set_fun]       = ImmediateType::None;
    itl[(int)BC::call_generic]  = ImmediateType::Int;
    itl[(int)BC::call_fast_env] = ImmediateType::Code;
    itl[(int)BC::call_fast]     = ImmediateType::Code;
    itl[(int)BC::check_arity]   = ImmediateType::Int;
    itl[(int)BC::ret]           = ImmediateType::None;
    itl[(int)BC::ret_prom]      = ImmediateType::None;
    itl[(int)BC::add]           = ImmediateType::None;


    return itl;
  }

  static ImmediateType* BC_immediate = getImmediateTypeList();
};

class BCVerifier {
 public:
  void verify(BC* bc, unsigned size);
};

#endif
