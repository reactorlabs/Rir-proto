#include "bc.h"
#include "object.h"

void BCVerifier::verify(BC* bc, unsigned size) {
  unsigned i = 0; for(; i < size;) {
    BC pc = bc[i++];
    assert((B)pc < (B)BC::num_of);
    switch(BC_immediate[(B)pc]) {
      case ImmediateType::None:
        break;
      case ImmediateType::Symbol:
        i += sizeof(unsigned);
        break;
      case ImmediateType::Code: {
        Value* v = *((Value**)&bc[i]);
        assert(dynamic_cast<Code*>(v));
        i += sizeof(Value*);
        break;
      }
      case ImmediateType::Value: {
        Value* v = *((Value**)&bc[i]);
        assert(dynamic_cast<Value*>(v));
        i += sizeof(Value*);
        break;
      }
      case ImmediateType::Int:
        i += sizeof(int);
        break;
    }
  }
  assert(i == size);
}

