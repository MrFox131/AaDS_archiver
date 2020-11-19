#include <stdlib.h>

#ifndef STACK_H
#define STACK_H

typedef struct node* T;

typedef struct Stack {
    T *data;
    size_t size;
    size_t top;
} Stack;

Stack* create_stack();

void resize_stack(Stack* stack);

T pop(Stack* stack);

void stack_push(Stack* stack, T elem);

int stack_is_empty(Stack* stack);

int stack_destructor(Stack* stack);

#endif