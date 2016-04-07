#include "interpreter.h"



Value* Interpreter::operator () (Closure* cls) {
  invoke(cls->code);

  $.push(cls);
  $.push(new Int(0));
  assert(*pc == BC::enter_fun);

  while (true) {
    switch(*pc++) {
      case BC::mkenv: {
        Env* enclos = $.pop<Env*>();
        rho = new Env(enclos);
        break;
      }

      case BC::loadenv: {
        Promise* p = $.top<Promise*>();
        rho = p->rho;
        break;
      }

      case BC::pushenv: {
        $.push(rho);
        break;
      }

      case BC::enter_fun: {
        unsigned expected = immediate<unsigned>();
        Int* arity = $.pop<Int*>();
        assert(arity->val == expected);
        Closure* fun = $.at<Closure*>(expected);
        Env* enclos = fun->rho;
        rho = new Env(enclos);
        break;
      }

      case BC::leave_fun: {
        Value* res = $.pop<Value*>();
        $.pop<Closure*>();
        $.push(res);
        rho = nullptr;
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
        // TODO: should this be here or in load
        if (p->value) {
          $.pop<Promise*>();
          $.push(p->value);
          break;
        }
        storeContext();
        invoke(p->code);
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

      case BC::call_generic: {
        unsigned arity = immediate<unsigned>();
        storeContext();
        $.push(new Int(arity));
        Closure* fun = $.at<Closure*>(arity+1);
        invoke(fun->code);
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

      case BC::update_prom: {
        Value* res = $.pop<Value*>();
        Promise* prom = $.pop<Promise*>();
        prom->value = res;
        $.push(res);
        rho = nullptr;
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
