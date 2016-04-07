#ifndef BC_H
#define BC_H

#include <cstdint>
#include <cassert>

typedef uint8_t B;

enum class BC : B {
  // Create a new current environment with the tos as enclosing environment
  // immediate: 0
  // stack: -1
  mkenv,

  // Load tos as current environment
  // immediate: 0
  // stack: -1
  loadenv,

  // Push the current env to the stack
  // immediate: 0
  // stack: +1
  pushenv,

  // (1) Pop number of arguments from the stack, (2) verify it matches arity,
  //     (3) load closure at arity offset from the stack and (4) create a new
  //     env with the closure env as the enclosing env.
  // immediate: sizeof(unsigned) arity
  // stack: -1
  enter_fun,

  // Pop closure from stack (left there by enter_fun), set env to null
  // immediate: 0
  // stack: -1
  leave_fun,

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
  //   * otherwise transfer control to the promise. prom_update will be
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

  // (1) push arity to the stack and (2) invoke the closure at arity offset on
  //   the stack (args are supposed to be already on the in between stack)
  // immediate: sizeof(Int*) as the arity
  // stack: +1
  call_generic,

  // Directly invoke a code object
  // immediate: sizeof(Code*)
  // stack: 0
  call_fast,

  // Return from function (return value is passed on stack)
  // immediate: 0
  // stack: 0
  ret,

  // Leave from promise: stack contains: [ ret_val, promise, ... ]
  //   Promise is removed from stack and updated with ret_val. ret_val is
  //   kept on stack as return value.
  // immediate: 0
  // stack: -1
  update_prom,

  // Add two numbers
  // immediate: 0
  // stack: -2, +1
  add,

  num_of
};

enum class ImmediateType {
  None,
  Symbol,
  Code,
  Value,
  Int,
};

static ImmediateType BC_immediate[(B)BC::num_of] = {
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::Int,
  ImmediateType::None,
  ImmediateType::Value,
  ImmediateType::Symbol,
  ImmediateType::Symbol,
  ImmediateType::None,
  ImmediateType::Code,
  ImmediateType::Code,
  ImmediateType::Int,
  ImmediateType::Code,
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::None,
};

class BCVerifier {
 public:
  void verify(BC* bc, unsigned size);
};

#endif
