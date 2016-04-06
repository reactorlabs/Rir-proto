#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "bc.h"
#include "object.h"

#include <cassert>
#include <iostream>
#include <stack>

class Interpreter {
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

  Env* rho;
  Stack $;
  Code* code;
  BC* pc;
  Ctx ctx;

  // Registers
  unsigned arity = 0;
  Closure* next_fun = nullptr;

  template <typename T>
  T immediate() {
    T val = *reinterpret_cast<T*>(pc);
    pc = reinterpret_cast<BC*>(
        reinterpret_cast<uintptr_t>(pc) + sizeof(T));
    return val;
  }

  void resume(Continuation* cont) {
    rho  = cont->rho;
    code = cont->code;
    pc   = cont->cont;
  }

  void storeContext() {
    Continuation* cont = new Continuation(code, pc, rho);
    ctx.push(cont);
  }

  void resumeContext() {
    resume(ctx.top());
    ctx.pop();
  }

 public:
  Value* operator () (Code* code) {
    return this->operator()(
        new Closure(code,
                    new Env(nullptr)));
  }

  Value* operator () (Closure* cls) {
    resume(cls);

    while (true) {
      switch(*pc++) {
        case BC::mkenv:
          rho = new Env(rho);
          break;

        case BC::push: {
          Value*   val = immediate<Value*>();
          $.push(val);
          break;
        }

        case BC::force: {
          Promise* p = dynamic_cast<Promise*>($.top());
          if (!p)
            break;
          if (p->value) {
            $.pop<Promise*>();
            $.push(p->value);
            break;
          }
          storeContext();
          resume(p);
          break;
        }

        case BC::store: {
          Symbol sym = immediate<Symbol>();
          Value* val = $.pop();
          rho->set(sym, val);
          break;
        }

        case BC::load: {
          Symbol sym = immediate<Symbol>();
          Value* val = rho->get(sym);
          $.push(val);
          break;
        }

        case BC::mkclosure: {
          Code* code = immediate<Code*>();
          Closure* cls = new Closure(code, rho);
          $.push(cls);
          break;
        }

        case BC::mkprom: {
          Code* code = immediate<Code*>();
          Promise* prm = new Promise(code, rho);
          $.push(prm);
          break;
        }

        case BC::set_fun: {
          assert(next_fun == nullptr);
          next_fun = $.pop<Closure*>();
          break;
        }

        case BC::check_arity: {
          unsigned expected = immediate<unsigned>();
          assert(arity == expected);
          break;
        }

        case BC::call: {
          arity = immediate<unsigned>();
          storeContext();
          resume(next_fun);
          next_fun = nullptr;
          break;
        }

        case BC::call_fast: {
          Code* fun = immediate<Code*>();
          storeContext();
          code = fun;
          pc   = code->bc;
          break;
        }

        case BC::call_fast_leaf: {
          Code* fun = immediate<Code*>();
          storeContext();
          code = fun;
          pc   = code->bc;
          rho  = nullptr;
          break;
        }

        case BC::ret: {
          if (ctx.empty()) goto done;
          resumeContext();
          break;
        }

        case BC::ret_prom: {
          Value* res = $.pop<Value*>();
          Promise* prom = $.pop<Promise*>();
          prom->value = res;
          $.push(res);
          resumeContext();
          break;
        }

        case BC::add: {
          Int* l = $.pop<Int*>();
          Int* r = $.pop<Int*>();
          $.push(*l + *r);
          break;
        }

        default:
          std::cout << "Invalid BC " << *(uint8_t*)pc << "\n";
          assert(false);
      }
    }

done:
    return $.pop();
  }
};

#endif
