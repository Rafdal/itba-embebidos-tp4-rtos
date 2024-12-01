#ifndef _MENU_CALL_STACK_H_
#define _MENU_CALL_STACK_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef void* void_ptr_t;

typedef struct call_stack
{
    void_ptr_t root;
    void_ptr_t* stack;
    uint8_t depth;
    uint8_t max_depth;
} call_stack_t;


call_stack_t* call_stack_init(uint8_t max_depth, void_ptr_t root);

void call_stack_push(call_stack_t* call_stack, void_ptr_t obj);
void* call_stack_current(call_stack_t* call_stack);
void* call_stack_pop(call_stack_t* call_stack);

#endif // _MENU_CALL_STACK_H_
