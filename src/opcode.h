#ifndef OPCODE_H
#define OPCODE_H

typedef enum {
    /* Constants */
    OP_LOAD_CONSTANT, OP_LOAD_TRUE, OP_LOAD_FALSE, OP_LOAD_NIL,

    /* Equality */
    OP_NOT_EQUAL, OP_EQUAL,

    /* Comparison */
    OP_GREATER, OP_GREATER_EQUAL, OP_LESS, OP_LESS_EQUAL,

    /* Arithmetic */
    OP_NOT, OP_NEGATE, OP_INC, OP_DEC, OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE, OP_MODULO, OP_POWER,

    /* Bitwise */
    OP_BITWISE_NOT, OP_BITWISE_AND, OP_BITWISE_OR, OP_BITWISE_XOR, OP_BITWISE_LEFT_SHIFT, OP_BITWISE_RIGHT_SHIFT,

    /* Control Flow */
    OP_JUMP, OP_JUMP_IF_FALSE, OP_POP_JUMP_IF_FALSE, OP_POP_JUMP_IF_EQUAL, OP_JUMP_IF_NOT_NIL, OP_LOOP,

    /* Globals */
    OP_DEFINE_GLOBAL, OP_LOAD_GLOBAL, OP_STORE_GLOBAL,

    /* Locals */
    OP_LOAD_LOCAL, OP_STORE_LOCAL,

    /* Functions */
    OP_CALL, OP_RETURN, OP_CLOSURE, OP_CLOSE_UPVALUE, OP_LOAD_UPVALUE, OP_STORE_UPVALUE,

    /* Classes */
    OP_CLASS, OP_INHERIT, OP_LOAD_PROPERTY, OP_LOAD_PROPERTY_SAFE, OP_STORE_PROPERTY, OP_STORE_PROPERTY_SAFE, OP_METHOD,
    OP_STATIC_METHOD, OP_INVOKE, OP_INVOKE_SAFE, OP_GET_SUPER, OP_SUPER_INVOKE, OP_END_CLASS,

    /* Collections */
    OP_LOAD_SUBSCRIPT, OP_LOAD_SUBSCRIPT_SAFE, OP_STORE_SUBSCRIPT, OP_STORE_SUBSCRIPT_SAFE, OP_LIST, OP_MAP,

    /* Stack */
    OP_POP, OP_DUP, OP_DUP_TWO, OP_SWAP, OP_SWAP_THREE, OP_SWAP_FOUR,
    
    /* Miscellaneous */
    OP_PRINT, OP_BUILD_STRING
} OpCode;

#endif
