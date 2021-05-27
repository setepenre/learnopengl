MAKEFLAGS := --jobs=$(shell nproc --ignore 1)

SRCD := src
OBJD := obj
INCD := include
LIBD := lib

SRC := $(wildcard $(SRCD)/*.cpp)
OBJ := $(SRC:$(SRCD)/%.cpp=$(OBJD)/%.o)
DEP := $(SRC:$(SRCD)/%.cpp=$(OBJD)/%.d)
LIB := $(LIBD)/libglad.a

CC       := g++
CPPFLAGS := -I$(INCD) -MMD -MP
CFLAGS   := -std=c++2a -Wextra -Wall -Ofast
LDFLAGS  := -L$(LIBD)
LDLIBS   := -lglfw -lGL -ldl -lglad

.PHONY: all run clean

all: $(LIB) main 

main: $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJD)/%.o: $(SRCD)/%.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJD)/glad.o: $(SRCD)/glad/glad.c
	$(CC) -I$(INCD)/glad $(CFLAGS) -c -o $@ $<

$(LIBD)/lib%.a: $(OBJD)/%.o
	ar rcs $@ $^

run: all
	./main

clean:
	rm -f main ${LIB} obj/*

-include ${DEP}
