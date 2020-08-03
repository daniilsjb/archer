#pragma once

typedef enum {
    TOKEN_LEFT_PARENTHESIS, TOKEN_RIGHT_PARENTHESIS, TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_SEMICOLON, TOKEN_QUESTION_MARK, TOKEN_COLON, TOKEN_TILDE,

    TOKEN_PLUS, TOKEN_PLUS_EQUAL, TOKEN_PLUS_PLUS,
    TOKEN_MINUS, TOKEN_MINUS_EQUAL, TOKEN_MINUS_MINUS,
    TOKEN_STAR, TOKEN_STAR_EQUAL,
    TOKEN_SLASH, TOKEN_SLASH_EQUAL,
    TOKEN_PERCENT, TOKEN_PERCENT_EQUAL,
    TOKEN_AMPERSAND, TOKEN_AMPERSAND_EQUAL,
    TOKEN_PIPE, TOKEN_PIPE_EQUAL,
    TOKEN_CARET, TOKEN_CARET_EQUAL,

    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_GREATER_GREATER, TOKEN_GREATER_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_LESS_LESS, TOKEN_LESS_LESS_EQUAL,

    TOKEN_AND, TOKEN_BREAK, TOKEN_CASE, TOKEN_CLASS, TOKEN_CONTINUE, TOKEN_DEFAULT, TOKEN_ELSE, TOKEN_FALSE, TOKEN_FUN, TOKEN_FOR, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SWITCH, TOKEN_STATIC, TOKEN_SUPER, TOKEN_THIS, TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    TOKEN_ERROR,
    TOKEN_NONE,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

void scanner_init(char const* source);
Token scan_token();