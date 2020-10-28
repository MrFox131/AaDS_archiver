#include <stdlib.h>
#include <stdio.h>

#define INIT_SIZE 20

typedef struct node* T;

typedef struct Stack {
    T *data;
    size_t size;
    size_t top;
}Stack;

Stack* create_stack(){
    Stack* new_stack = (Stack*)malloc(sizeof(Stack));
    new_stack->size = INIT_SIZE;
    new_stack->top = 0;
    new_stack->data = (T*)calloc(INIT_SIZE, sizeof(T));
    return new_stack;
}

void resize_stack(Stack* stack){
    stack->data=(T*)realloc(stack->data, (stack->size+INIT_SIZE)*sizeof(T));
    stack->size+=INIT_SIZE;
    if (stack->data==NULL){
        printf("PANIC: memory was not reallocated. From resize_stack()");
    }
}

T pop(Stack* stack){
    stack->top--;
    return stack->data[stack->top];
}

void push(Stack* stack, T elem) {
    if(stack->top==stack->size)
        resize_stack(stack);
    stack->data[stack->top] = elem;
    stack->top++;
}

int is_empty(Stack* stack){
    if(stack->top==0){
        return 1;
    }
    return 0;
}