#ifndef BUILDER_H
#define BUILDER_H

#include "ir.h"

#include <vector>

class Builder {
  std::vector<char>* code;
  Pool* pool;

  unsigned pos = 0;
  unsigned size = 1024;

 public:
  Builder() {
    code = new std::vector<char>(1024);
    pool = new Pool();
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
    unsigned idx = pool->intern(v);
    push(idx);
    return *this;
  }

  Builder& operator << (int i) {
    push(i);
    return *this;
  }

  Code* operator () () {
    auto res = new Code(reinterpret_cast<BC*>(&(*code)[0]), pool);
    code = nullptr;
    pool = nullptr;
    return res;
  }
};

#endif
