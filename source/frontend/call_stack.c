#include "call_stack.h"

call_stack_t* call_stack_init(uint8_t max_depth, void_ptr_t root)
{
	call_stack_t* call_stack = (call_stack_t*)malloc(sizeof(call_stack_t));
    call_stack->stack = (void_ptr_t*)malloc(max_depth * sizeof(void_ptr_t));

    call_stack->max_depth = max_depth;
    call_stack->depth = 0;
    call_stack->root = root;
    for (uint8_t i = 0; i < max_depth; i++)
    {
        call_stack->stack[i] = NULL;
    }
    return call_stack;
}

void call_stack_push(call_stack_t* call_stack, void_ptr_t obj)
{
    if (call_stack->depth < call_stack->max_depth)
    {
        call_stack->stack[call_stack->depth] = obj;
        call_stack->depth++;
    }
}

void* call_stack_current(call_stack_t* call_stack)
{
    if (call_stack->depth > 0)
    {
        return call_stack->stack[call_stack->depth - 1];
    }
    return call_stack->root;
}

void* call_stack_pop(call_stack_t* call_stack)
{
    if (call_stack->depth > 0)
    {
        call_stack->depth--;
        return call_stack->stack[call_stack->depth];
    }
    return call_stack->root;
}
