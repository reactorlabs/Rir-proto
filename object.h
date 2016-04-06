#ifndef OBJECT_H
#define OBJECT_H

#include "bc.h"

#include <iostream>
#include <map>
#include <vector>

typedef unsigned Symbol;

class Value {
  friend std::ostream& operator << (std::ostream& o, Value& v) {
    v.print(o);
    return o;
  }

  virtual void print(std::ostream& o) = 0;
};

class Env : public Value {
  std::map<Symbol, Value*> env;
  Env* enclos;
 public:
  Env(Env* enclos) : enclos(enclos) {}

  void set(Symbol s, Value* val) {
    env[s] = val;
  }

  Value* get(Symbol s) {
    return env.count(s) ? env.at(s) : enclos->get(s);
  }

  void print(std::ostream& o) override {
    o << "anEnv";
  }
};

class Code;

class Continuation : public Value {
 public:
  Code* code;
  BC*   cont;
  Env*  rho;
  Continuation(Code* code, BC* cont, Env* rho) :
      code(code), cont(cont), rho(rho) {}

  void print(std::ostream& o) override {
    o << "aCont";
  }
};

class Int : public Value {
 public:
  Int(int val) : val(val) {}
  int val;
  Int* operator + (const Int& other) {
    return new Int(val + other.val);
  }

  void print(std::ostream& o) override {
    o << val;
  }
};

class Symbols {
  std::vector<const char *> sym2name;
  std::map<const char *, Symbol> name2sym;

  static Symbols& singleton() {
    static Symbols s;
    return s;
  }

  Symbol intern_(const char * name) {
    if (name2sym.count(name))
      return name2sym.at(name);
    sym2name.push_back(name);
    name2sym.emplace(name, sym2name.size());
    return sym2name.size();
  }

  const char* name_(Symbol s) {
    return sym2name[s-1];
  }
 public:
  static Symbol intern(const char * name) { return singleton().intern_(name); }
  static const char* name(Symbol s) { return singleton().name_(s); }
};

class Code : public Value {
 public:
  BC* bc;
  Code(BC* bc) : bc(bc) {}

  void print(std::ostream& o) override {
    o << "someCode";
  }
};

class Closure : public Continuation {
 public:
  Closure(Code* code, Env* rho) : Continuation(code, code->bc, rho) {}
};

class Promise : public Continuation {
 public:
  Value* value = nullptr;
  Promise(Code* code, Env* rho) : Continuation(code, code->bc, rho) {}
};

Value* C(int i) {
  return new Int(i);
}

#endif
