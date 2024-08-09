#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <stdio.h>
#include <stdbool.h>


enum {
    DIGIT           = 'd',
    WORDCHAR        = 'w',
    WHITESPACE      = 's',
    NOT_DIGIT       = 'D',
    NOT_WORDCHAR    = 'W',
    NOT_WHITESPACE  = 'S'
};


typedef enum {
    TOKEN_LEFT_PAREN,   TOKEN_RIGHT_PAREN,      // ()
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,    // []
    TOKEN_LEFT_BRACE,   TOKEN_RIGHT_BRACE,      // {}
    TOKEN_PIPE,                                 // |
    TOKEN_QUESTION,                             // ?
    TOKEN_STAR,                                 // *
    TOKEN_PLUS,                                 // +
    TOKEN_CIRCUMFLEX,                           // ^
    TOKEN_COMMA,                                // ,
    TOKEN_DOT,                                  // .
    TOKEN_CHAR_CLASS,                           // \d \D \w \W \s \S
    TOKEN_CHAR,
    TOKEN_ESCAPED_CHAR,
    TOKEN_ERROR,
    TOKEN_EOF
} token_type_t;


typedef struct {
    token_type_t type;
    int value;
} token_t;


#endif


