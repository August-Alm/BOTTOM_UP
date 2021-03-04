/* ***** ***** */

#ifndef STACK_H
#define STACK_H

/* ***** ***** */

struct sll;

/* ***** ***** */

#define POP(data, stack) do { \
    data = (*stack)->current; \
    *stack = (*stack)->next;  \
} while(0)

#define PUSH(data, stack) do { \
    *stack = &(struct sll) {   \
        .next = *stack,        \
        .current = data        \
    };                         \
} while(0)

/* ***** ***** */

#endif // STACK_H
