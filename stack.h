#include <stdlib.h>

#ifndef STACK_H
#define STACK_H
#include "stack.c"

Stack* create_stack();

void resize_stack(Stack* stack);

T pop(Stack* stack);

void push(Stack* stack, T elem);

int is_empty(Stack* stack);

#endif