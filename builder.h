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

  Builder& operator << (Value* v) {
    push(v);
    return *this;
  }

  Builder& operator << (unsigned i) {
    push(i);
    return *this;
  }

  Code* operator () () {
    auto res = new Code(reinterpret_cast<BC*>(&(*code)[0]));
    code = nullptr;
    return res;
  }
};

#endif
