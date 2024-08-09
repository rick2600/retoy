#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "scanner.h"
#include "token.h"


scanner_t scanner;


static bool eof() {
    return scanner.current[0] == '\0';
}


static void fatal_error(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}


static inline char previous() {
    return scanner.current[-1];
}


static inline char current() {
    return scanner.current[0];
}


static char consume() {
    scanner.current++;
    return scanner.current[-1];
}


static bool match(char c) {
    if (scanner.current[0] == c) {
        scanner.current++;
        return true;
    }
    return false;
}


static token_t make_token(token_type_t type) {
    token_t token;
    token.type = type;
    token.value = previous();
    return token;
}


static token_t make_token_value(token_type_t type, int value) {
    token_t token;
    token.type = type;
    token.value = value;
    return token;
}


static token_t parse_backslash() {
    /*
    if (match('d')) return make_token_value(TOKEN_CHAR_CLASS, DIGIT);
    if (match('D')) return make_token_value(TOKEN_CHAR_CLASS, NOT_DIGIT);
    if (match('w')) return make_token_value(TOKEN_CHAR_CLASS, WORDCHAR);
    if (match('W')) return make_token_value(TOKEN_CHAR_CLASS, NOT_WORDCHAR);
    if (match('s')) return make_token_value(TOKEN_CHAR_CLASS, WHITESPACE);
    if (match('S')) return make_token_value(TOKEN_CHAR_CLASS, NOT_WHITESPACE);
    */

    if (match('d')) return make_token(TOKEN_CHAR_CLASS);
    if (match('D')) return make_token(TOKEN_CHAR_CLASS);
    if (match('w')) return make_token(TOKEN_CHAR_CLASS);
    if (match('W')) return make_token(TOKEN_CHAR_CLASS);
    if (match('s')) return make_token(TOKEN_CHAR_CLASS);
    if (match('S')) return make_token(TOKEN_CHAR_CLASS);

    if (match('t')) return make_token_value(TOKEN_CHAR, '\t');

    const char* error_msg = "Invalid escape sequence.";

    if (match('x')) {
        char hex_str[3] = {0};
        for (int i = 0; i < 2; i++) {
            if (eof()) fatal_error(error_msg);
            hex_str[i] = consume();
            if (!isxdigit(hex_str[i])) fatal_error(error_msg);
        }
        int hex = strtol(hex_str, NULL, 16);
        return make_token_value(TOKEN_CHAR, hex);
    }

    if (!eof() && isprint(current()))
        return make_token_value(TOKEN_ESCAPED_CHAR, consume());

    fatal_error(error_msg);
    return make_token(TOKEN_ERROR);
}

void scanner_init(const char* input) {
    scanner.start = input;
    scanner.current = input;
}


token_t scan_token() {
    if (eof())          return make_token(TOKEN_EOF);
    if (match('?'))     return make_token(TOKEN_QUESTION);
    if (match('*'))     return make_token(TOKEN_STAR);
    if (match('+'))     return make_token(TOKEN_PLUS);
    if (match('|'))     return make_token(TOKEN_PIPE);
    if (match('.'))     return make_token(TOKEN_DOT);
    if (match('^'))     return make_token(TOKEN_CIRCUMFLEX);
    if (match('('))     return make_token(TOKEN_LEFT_PAREN);
    if (match(')'))     return make_token(TOKEN_RIGHT_PAREN);
    if (match('['))     return make_token(TOKEN_LEFT_BRACKET);
    if (match(']'))     return make_token(TOKEN_RIGHT_BRACKET);
    if (match('{'))     return make_token(TOKEN_LEFT_BRACE);
    if (match('}'))     return make_token(TOKEN_RIGHT_BRACE);
    if (match(','))     return make_token(TOKEN_COMMA);
    if (match('\\'))    return parse_backslash();
    char c = consume();
    if (isprint(c))     return make_token(TOKEN_CHAR);
    fatal_error("Unexpected char.");
    return make_token(TOKEN_ERROR);
}
