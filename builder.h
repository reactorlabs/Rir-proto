#ifndef BUILDER_H
#define BUILDER_H

#include "bc.h"

#include <vector>
#include <string>
#include <cstring>

namespace {

class CodeStream {
  std::vector<char>* code;

  unsigned pos = 0;
  unsigned size = 1024;

 public:
  CodeStream() {
    code = new std::vector<char>(1024);
  }

  template <typename T>
  CodeStream& operator << (T val) {
    size_t s = sizeof(T);
    if (pos + s >= size) {
      size += 1024;
      code->resize(size);
    }
    *reinterpret_cast<T*>(&(*code)[pos]) = val;
    pos += s;
    return *this;
  }

  unsigned length() {
    return pos;
  }

  BC* operator () () {
    BC* res = reinterpret_cast<BC*>(&(*code)[0]);
    code = nullptr;
    BCVerifier v;
    v.verify(res, pos);
    return res;
  }

  CodeStream& operator << (CodeStream& cs) {
    size += cs.pos;
    code->resize(size);
    memcpy((void*)&((*code)[pos]), (void*)&((*cs.code)[0]), cs.pos);
    pos += cs.pos;
    return *this;
  }

  void patchLabel(unsigned pos, int off) {
      *(int*)&code[pos] = off;
  }

  friend class Builder;
};

}

class BasicBlock;

class Next : public std::vector<BasicBlock*> {};

class BasicBlock {
 public:
  CodeStream code_;
  Next next;
  bool done = false;

  template <typename T>
  BasicBlock& operator << (T bc) {
    assert(!done and "cannot have multiple branches");
    code_ << bc;
    return *this;
  }

  BasicBlock& operator << (BasicBlock* bb) {
    next.push_back(bb);
    done = true;
    return *this;
  }

  inline Code* code();
};

class Serializer {
  std::map<BasicBlock*, unsigned> labelPos;
  std::map<unsigned, BasicBlock*> patchPos;

 public:
  static Code* get(BasicBlock* start) {
    Serializer s;
    return s.get_(start);
  }

 private:
  Code* get_(BasicBlock* start) {
    CodeStream result;
    writeOut(start, result);
    patch(result);
    return new Code(result());
  }

  void patch(CodeStream& result) {
    for (auto p : patchPos) {
      int pos = labelPos[p.second];
      result.patchLabel(p.first, pos - p.first);
    }
  }

  void writeOut(BasicBlock* bb, CodeStream& result) {
    labelPos[bb] = result.length();
    result << bb->code_;
    if (bb->next.empty())
      return;

    for (auto n : bb->next) {
      patchPos[result.length()] = n;
      result << (int)0;
    }

    for (auto n : bb->next) {
      writeOut(n, result);
    }
  }
};

Code* BasicBlock::code() {
  return Serializer::get(this);
}

extern inline Value* C(int i) {
  return new Int(i);
}

extern inline Vector* L(std::initializer_list<Value*> ns) {
  return new Vector(ns);
}

#endif
