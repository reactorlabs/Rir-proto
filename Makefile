TARGET = ir
LIBS =
CC = clang++
CFLAGS = --std=c++11 -g -Wall
SUFFX = cpp

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.$(SUFFX), %.o, $(wildcard *.$(SUFFX)))
HEADERS = $(wildcard *.h)

%.o: %.$(SUFFX) $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
