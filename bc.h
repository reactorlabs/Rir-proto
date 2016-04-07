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

  // load closure at arity offset from the stack and create a new
  //     env with the closure env as the enclosing env.
  // immediate: sizeof(unsigned) arity
  // stack: 0
  enter_fun,

  // Pop closure from stack (left there by enter_fun), and resoter env
  // immediate: 0
  // stack: -1
  leave_fun,

  // restore env
  // immediate: 0
  // stack: 0
  leave,

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
  //   * otherwise transfer control to the promise. leave_prom will be
  //     responsible for placing the correct value on the stack.
  // immediate: 0
  // stack: -1, +1 (if evaluated promise); 0 (otherwise)
  force,

  // Wrap a code object in a closure with current rho
  // immediate: Code*, formals*
  // stack: +1
  mkclosure,

  // Wrap a code object in a promise with current rho
  // immediate: sizeof(Code*)
  // stack: +1
  mkprom,

  // invoke the closure at nargs offset on the stack
  //   (args are supposed to be already on the in between stack)
  // immediate: nargs: unsigned
  // stack: 0
  call_generic,

  // Directly invoke a code object
  // immediate: Code*
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
  leave_prom,

  // Add two numbers
  // immediate: 0
  // stack: -2, +1
  add,

  // invoke the closure at nargs offset on the stack and fill up missing
  // arguments by Symbols::missing_arg
  // immediate: nargs: unsigned
  // stack: ?
  call_arg_adapt,

  // invoke the closure at nargs offset on the stack and reshuffle arguments
  // to match given names
  // immediate: nargs: unsigned, names: Vector*
  // stack: ?
  call_name_arg_adapt,

  num_of
};

enum class ImmediateType {
  None,
  Symbol,
  Code,
  Vector,
  CodeVector,
  Value,
  Unsigned,
};

static ImmediateType BC_immediate[(B)BC::num_of] = {
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::Unsigned,
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::Value,
  ImmediateType::Symbol,
  ImmediateType::Symbol,
  ImmediateType::None,
  ImmediateType::CodeVector,
  ImmediateType::Code,
  ImmediateType::Unsigned,
  ImmediateType::Code,
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::None,
  ImmediateType::Unsigned,
  ImmediateType::Vector,
};

class BCVerifier {
 public:
  void verify(BC* bc, unsigned size);
};

#endif
