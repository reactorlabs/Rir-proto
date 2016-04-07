#ifndef OBJECT_H
#define OBJECT_H

#include "bc.h"

#include <iostream>
#include <map>
#include <vector>
#include <cassert>


class Value {
  friend std::ostream& operator << (std::ostream& o, Value& v) {
    v.print(o);
    return o;
  }

  virtual void print(std::ostream& o) = 0;
};

class Symbol : public Value {
 public:
  const char * name;
  Symbol(const char * name) : name(name) {}

  void print(std::ostream& o) override {
    o << name;
  }
};

class Symbols {
  std::map<Symbol*, const char *> sym2name;
  std::map<const char *, Symbol*> name2sym;

  static Symbols& singleton() {
    static Symbols s;
    return s;
  }

  Symbols() {
    // Add some reserved Symbols
    missing_arg = new Symbol("?");
    sym2name.emplace(missing_arg, "?");
    positional_arg = new Symbol("_");
    sym2name.emplace(positional_arg, "_");
  }

  Symbol* intern_(const char * name) {
    if (name2sym.count(name))
      return name2sym.at(name);
    Symbol* s = new Symbol(name);
    name2sym.emplace(name, s);
    sym2name.emplace(s, name);
    return s;
  }

  const char* name_(Symbol* s) {
    return sym2name.at(s);
  }

 public:
  static Symbol* intern(const char * name) { return singleton().intern_(name); }
  static const char* name(Symbol* s) { return singleton().name_(s); }

  static Symbol* missing_arg;
  static Symbol* positional_arg;
};

class Vector : public Value {
 public:
  std::vector<Value*> values;
 public:
  Vector(std::initializer_list<Value*> values) : values(values) {}

  size_t size() {
    return values.size();
  }

  void print(std::ostream& o) override {
    o << "[";
    for (auto v : values)
      o << *v << ",";
    o << "]";
  }
};

class Env : public Value {
  std::map<Symbol*, Value*> env;
  Env* enclos;
 public:
  Env(Env* enclos) : enclos(enclos) {}

  void set(Symbol* s, Value* val) {
    env[s] = val;
  }

  Value* get(Symbol* s) {
    if (env.count(s))
      return env.at(s);
    if (enclos == nullptr) {
      std::cout << "symbol " << Symbols::name(s) << " not found\n";
      assert(false);
    }
    return enclos->get(s);
  }

  void print(std::ostream& o) override {
    o << "anEnv";
  }
};

class Code;

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

class Code : public Value {
 public:
  BC* bc;
  Code(BC* bc) : bc(bc) {}

  void print(std::ostream& o) override {
    o << "someCode";
  }
};

class Closure : public Value {
 public:
  Code* code;
  Env* rho;
  Vector* formals;

  Closure(Code* code, Env* rho, Vector* formals) :
      code(code), rho(rho), formals(formals) {}

  void print(std::ostream& o) override {
    o << "aClosure";
  }
};

class Promise : public Value {
 public:
  Code* code;
  Env* rho;
  Value* value = nullptr;
  Promise(Code* code, Env* rho) : code(code), rho(rho) {}

  void print(std::ostream& o) override {
    o << "aPromise(";
    if (value) o << value;
    else o << "-";
    o << ")";
  }
};

#endif
