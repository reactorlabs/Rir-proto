#include "test.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

extern void checkInt(Code* c, int i) {
  Interpreter run;
  Value* res = run(c);
  Int* iv = dynamic_cast<Int*>(res);
  EXPECT_NE(iv, nullptr);
  EXPECT_EQ(iv->val, i);
}
