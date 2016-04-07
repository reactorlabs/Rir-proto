#include "interpreter.h"



Value* Interpreter::operator () (Closure* cls) {
  code = cls->code;
  pc = cls->code->bc;

  $.push(cls);
  assert(*pc == BC::enter_fun);

  while (true) {
    switch(*pc++) {
      case BC::mkenv: {
        storeEnv();
        Env* enclos = $.pop<Env*>();
        rho = new Env(enclos);
        break;
      }

      case BC::loadenv: {
        storeEnv();
        Promise* p = $.top<Promise*>();
        rho = p->rho;
        break;
      }

      case BC::pushenv: {
        $.push(rho);
        break;
      }

      case BC::enter_fun: {
        storeEnv();
        unsigned arity = immediate<unsigned>();
        Closure* fun = $.at<Closure*>(arity);
        Env* enclos = fun->rho;
        rho = new Env(enclos);
        break;
      }

      case BC::leave: {
        restoreEnv();
        break;
      }

      case BC::leave_fun: {
        Value* res = $.pop<Value*>();
        $.pop<Closure*>();
        $.push(res);
        restoreEnv();
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
        invoke(p->code);
        break;
      }

      case BC::store: {
        assert(rho);
        Symbol* sym = immediate<Symbol*>();
        Value* val = $.pop();
        rho->set(sym, val);
        break;
      }

      case BC::load: {
        assert(rho);
        Symbol* sym = immediate<Symbol*>();
        Value* val = rho->get(sym);
        if (val == Symbols::missing_arg) {
          std::cout << "missing arg " << *sym << "\n";
          assert(false);
        }
        $.push(val);
        break;
      }

      case BC::mkclosure: {
        assert(rho);
        Code* code = immediate<Code*>();
        Vector* formals = immediate<Vector*>();
        Closure* cls = new Closure(code, rho, formals);
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
        unsigned num_args = immediate<unsigned>();
        Closure* fun = $.at<Closure*>(num_args);
        unsigned arity = fun->formals->size();
        assert(num_args == arity);
        invoke(fun->code);
        break;
      }

      case BC::call_arg_adapt: {
        unsigned num_args = immediate<unsigned>();
        Closure* fun = $.at<Closure*>(num_args);
        unsigned arity = fun->formals->size();
        if (num_args > arity) {
          std::cout << "too many args given\n";
          assert(false);
        }
        for (int i = num_args; i < arity; ++i) {
          $.push(Symbols::missing_arg);
        }
        invoke(fun->code);
        break;
      }

      case BC::call_name_arg_adapt: {
        Vector* names = immediate<Vector*>();
        unsigned num_args = names->size();

        Closure* fun = $.at<Closure*>(num_args);
        unsigned arity = fun->formals->size();

        if (num_args > arity) {
          std::cout << "too many args given\n";
          assert(false);
        }

        std::deque<Value*> args;
        for (int i = 0; i < num_args; ++i)
          args.push_front($.pop());

        int positional = 0;
        for (auto f : fun->formals->values) {
          int found;
          for (found = 0; found < names->size(); ++found) {
            if (names->values[found] == f) {
              $.push(args[found]);
              break;
            }
          }
          if (found == names->size()) {
            while (true) {
              if (positional == args.size()) {
                $.push(Symbols::missing_arg);
                break;
              }
              if (names->values[positional] == Symbols::positional_arg) {
                $.push(args[positional++]);
                break;
              }
              positional++;
            }
          }
        }
        invoke(fun->code);
        break;
      }

      case BC::call_fast: {
        Code* fun = immediate<Code*>();
        invoke(fun);
        break;
      }

      case BC::ret: {
        if (ctx.empty()) goto done;
        restoreContext();
        break;
      }

      case BC::leave_prom: {
        Value* res = $.pop<Value*>();
        Promise* prom = $.pop<Promise*>();
        prom->value = res;
        $.push(res);
        restoreEnv();
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
  pc = nullptr;
  code = nullptr;
  assert(ctx.empty());
  assert(envCtx.empty());
  return $.pop();
}
