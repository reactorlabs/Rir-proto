#ifndef TEST_H
#define TEST_H

#include <gtest/gtest.h>

#include "../interpreter.h"
#include "../builder.h"

#include <iostream>

static Symbol* a = Symbols::intern("a");
static Symbol* b = Symbols::intern("b");
static Symbol* c = Symbols::intern("c");
static Symbol* f = Symbols::intern("f");
static Symbol* g = Symbols::intern("g");
static Symbol* _ = Symbols::positional_arg;

extern void checkInt(Code* c, int i);
#endif
