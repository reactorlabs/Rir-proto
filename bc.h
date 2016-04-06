#ifndef BC_H
#define BC_H

#include <cstdint>

typedef uint8_t B;

enum class BC : B {
  // Create a new environment with the current rho as parrent
  // immediate: 0
  // stack: 0
  mkenv,

  // Push an immediate int
  // immediate: sizeof(int) bytes
  // stack: +1
  push_int,

  // Read an index and load it from the constant pool
  // immediate: sizeof(unsigned) bytes
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

  // Set arity register and call the function in the function register
  //   (args are passed via stack)
  // immediate: sizeof(unsigned) as the arity
  // stack: 0
  call,

  // Verify that the arity register contains the expected number of args
  // immediate: sizeof(unsigned)
  // stack: 0
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
};

#endif
