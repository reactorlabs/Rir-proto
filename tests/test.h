#ifndef TEST_H
#define TEST_H

#include <gtest/gtest.h>

#include "../interpreter.h"
#include "../builder.h"

#include <iostream>

const static Symbol a = Symbols::intern("a");
const static Symbol b = Symbols::intern("b");
const static Symbol c = Symbols::intern("c");
const static Symbol f = Symbols::intern("f");
const static Symbol g = Symbols::intern("g");

extern void checkInt(Code* c, int i);
#endif
