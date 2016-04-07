#ifndef BUILDER_H
#define BUILDER_H

#include "bc.h"

#include <vector>

class Builder {
  std::vector<char>* code;

  unsigned pos = 0;
  unsigned size = 1024;

 public:
  Builder() {
    code = new std::vector<char>(1024);
  }

  template<typename T>
  void push(T val) {
    size_t s = sizeof(T);
    if (pos + s >= size) {
      size += 1024;
      code->resize(size);
    }
    *reinterpret_cast<T*>(&(*code)[pos]) = val;
    pos += s;
  }

  Builder& operator << (BC bc) {
    push(bc);
    return *this;
  }

  Builder& operator << (const Value* v) {
    push(v);
    return *this;
  }

  Builder& operator << (unsigned i) {
    push(i);
    return *this;
  }

  Builder& operator << (Vector* names) {
    push(names);
    return *this;
  }

  Code* operator () () {
    auto res = new Code(reinterpret_cast<BC*>(&(*code)[0]));
    code = nullptr;
    BCVerifier v;
    v.verify(res->bc, pos);
    return res;
  }
};

extern inline Value* C(int i) {
  return new Int(i);
}

extern inline Vector* L(std::initializer_list<Value*> ns) {
  return new Vector(ns);
}

#endif
