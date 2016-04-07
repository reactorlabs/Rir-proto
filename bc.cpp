#include "bc.h"
#include "object.h"

void BCVerifier::verify(BC* bc, unsigned size) {
  unsigned i = 0; for(; i < size;) {
    BC pc = bc[i++];
    assert((B)pc < (B)BC::num_of);
    switch(BC_immediate[(B)pc]) {
      case ImmediateType::None:
        break;
      case ImmediateType::Symbol: {
        Value* v = *((Value**)&bc[i]);
        Symbol* s = dynamic_cast<Symbol*>(v);
        assert(s);
        assert(Symbols::name(s));
        i += sizeof(Value*);
        break;
      }
      case ImmediateType::CodeVector:
      case ImmediateType::Code: {
        Value* v = *((Value**)&bc[i]);
        assert(dynamic_cast<Code*>(v));
        i += sizeof(Value*);
        if (BC_immediate[(B)pc] == ImmediateType::CodeVector)
          goto vector;
        break;
      }
      case ImmediateType::Vector: {
vector:
        Value* v = *((Value**)&bc[i]);
        assert(dynamic_cast<Vector*>(v));
        i += sizeof(Value*);
        break;
      }
      case ImmediateType::Value: {
        Value* v = *((Value**)&bc[i]);
        assert(dynamic_cast<Value*>(v));
        assert(*(int*)v);
        i += sizeof(Value*);
        break;
      }
      case ImmediateType::Unsigned: {
        i += sizeof(unsigned);
        break;
      }
    }
  }
  assert(i == size);
}

