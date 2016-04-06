#include "interpreter.h"



Value* Interpreter::operator () (Closure* cls) {
  $.push(cls->rho);
  invoke(cls);
  assert(*pc == BC::mkenv);

  while (true) {
    switch(*pc++) {
      case BC::loadenv: {
        rho = $.pop<Env*>();
        break;
      }

      case BC::mkenv: {
        Env* enclos = $.pop<Env*>();
        rho = new Env(enclos);
        break;
      }

      case BC::push: {
        Value* val = immediate<Value*>();
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
        $.push(rho);
        invoke(p);
        break;
      }

      case BC::store: {
        assert(rho);
        Symbol sym = immediate<Symbol>();
        Value* val = $.pop();
        rho->set(sym, val);
        break;
      }

      case BC::load: {
        assert(rho);
        Symbol sym = immediate<Symbol>();
        Value* val = rho->get(sym);
        $.push(val);
        break;
      }

      case BC::mkclosure: {
        assert(rho);
        Code* code = immediate<Code*>();
        Closure* cls = new Closure(code, rho);
        $.push(cls);
        break;
      }

      case BC::mkprom: {
        assert(rho);
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
        Int* arity = $.pop<Int*>();
        assert(arity->val == expected);
        break;
      }

      case BC::call_generic: {
        unsigned arity = immediate<unsigned>();
        storeContext();
        $.push(new Int(arity));
        $.push(next_fun->rho);
        invoke(next_fun);
        break;
      }

      case BC::call_fast_env: {
        Code* fun = immediate<Code*>();
        storeContext();
        $.push(rho);
        invoke(fun);
        break;
      }

      case BC::call_fast: {
        Code* fun = immediate<Code*>();
        storeContext();
        invoke(fun);
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
