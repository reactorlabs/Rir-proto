#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "bc.h"
#include "object.h"

#include <cassert>
#include <iostream>
#include <stack>

class Interpreter {
  class Continuation {
   public:
    Code* code;
    BC* pos;
    Env* rho;
    Continuation(Code* code, BC* pos, Env* rho) :
        code(code), pos(pos), rho(rho) {}
  };

  class Stack {
    typedef std::stack<Value*> S;
    S s;
   public:
    void push(Value* v) {
      s.push(v);
    }

    Value* pop() {
      if (s.empty()) {
        std::cout << "Stack underflow\n";
        assert(false);
      }
      Value* t = s.top();
      s.pop();
      return t;
    }

    Value* top() {
      return s.top();
    }

    template <typename T>
    T pop() {
      T t = dynamic_cast<T>(s.top());
      assert(t);
      s.pop();
      return t;
    }
  };

  typedef std::stack<Continuation*> Ctx;

  // Global interpreter and context stack
  Stack $;
  Ctx ctx;

  // Interpreter state
  Code* code;
  BC* pc;

  // Registers local to the function
  Closure* next_fun = nullptr;
  Env* rho = nullptr;

  template <typename T>
  T immediate() {
    T val = *reinterpret_cast<T*>(pc);
    pc = reinterpret_cast<BC*>(
        reinterpret_cast<uintptr_t>(pc) + sizeof(T));
    return val;
  }

  void clearRegisters() {
    next_fun = nullptr;
    rho = nullptr;
  }

  void invoke(Closure* c) {
    invoke(c->code);
  }

  void invoke(Code* c) {
    code = c;
    pc   = c->bc;
    clearRegisters();
  }

  void storeContext() {
    Continuation* cont = new Continuation(code, pc, rho);
    ctx.push(cont);
  }

  void resumeContext() {
    Continuation* cont = ctx.top();
    ctx.pop();
    rho  = cont->rho;
    code = cont->code;
    pc   = cont->pos;
  }

 public:
  Value* operator () (Code* code) {
    return this->operator()(
        new Closure(code,
                    new Env(nullptr)));
  }

  Value* operator () (Closure* cls);

};

#endif
