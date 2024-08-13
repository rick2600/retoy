#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "ast.h"


parser_t parser;


static bool end() {
    return parser.current.type == TOKEN_EOF;
}


static inline token_t current() {
    return parser.current;
}


static inline token_t previous() {
    return parser.previous;
}


static token_t consume() {
    if (end()) return current();
    parser.previous = current();
    parser.current = scan_token();
    return previous();
}


static bool match(token_type_t type) {
    if (current().type == type) {
        consume();
        return true;
    }
    return false;
}


static void parser_abort(const char* msg) {
    fprintf(stderr, "ABORT: %s\n", msg);
    exit(EXIT_FAILURE);
}


static void match_or_abort(token_type_t type, const char* msg) {
    if (!match(type)) {
        parser_abort(msg);
    }
}

static ast_node_t* expression();


static void parser_init() {
    parser.current = scan_token();
    parser.inside_set = false;
}


static bool match_range_separator() {
    bool res = current().type == TOKEN_CHAR && current().value == '-';
    if (res) {
        consume();
    }
    return res;
}


static bool follow_quantifier() {
    token_type_t t = current().type;
    return (t == TOKEN_STAR ||
            t == TOKEN_PLUS ||
            t == TOKEN_QUESTION ||
            t == TOKEN_LEFT_BRACE);
}


static bool follow_char_set() {
    return current().type == TOKEN_LEFT_BRACKET;
}


static bool follow_group() {
    return current().type == TOKEN_LEFT_PAREN;
}


static bool follow_char_class() {
    return current().type == TOKEN_CHAR_CLASS;
}


static bool follow_match_char_class() {
    return follow_char_set() || follow_char_class();
}


static bool follow_match_char() {
    token_type_t t = current().type;
    if (t == TOKEN_CHAR) return true;
    if (t == TOKEN_ESCAPED_CHAR) return true;
    if (parser.inside_set == false) {
        if (t == TOKEN_RIGHT_BRACKET) return true;
    }
    return false;
    // TOKEN_ESCAPED_CHAR too ?
}


static bool follow_char_range() {
    // Within a character set, only \, -, and ] need to be escaped.


    if (current().type == TOKEN_ESCAPED_CHAR) return true;
    if (current().type == TOKEN_CHAR && current().value != '-') return true;
    return false;


    /*
    if (current().type == TOKEN_RIGHT_BRACKET) return false;
    if (current().type == TOKEN_CHAR) {
        if (current().value == '-') return false;
    }
    return true;
    */
    //return current().type == TOKEN_CHAR;
    // or TOKEN_ESCAPED_CHAR too ?
}


static bool follow_match() {
    return (current().type == TOKEN_DOT ||
            follow_match_char_class() ||
            follow_match_char());
}


static bool follow_subexpression_item() {
    return follow_match() || follow_group();
}


static bool follow_char_set_item() {
    return follow_char_class() || follow_char_range();
}


static bool token_is_digit() {
    token_t token = current();
    return (token.type == TOKEN_CHAR && isdigit(token.value));
}


static bool range_quantifier_bound(uint32_t* value) {
    char s[16] = {0};
    size_t pos = 0;
    while (token_is_digit() && pos <= 12) {
        s[pos] = consume().value;
        pos++;
    }
    if (pos == 0) return false;
    *value = strtoull(s, NULL, 10);
    return true;
}


// RangeQuantifier ::= '{' RangeQuantifierLowerBound ( ',' RangeQuantifierUpperBound? )? '}'
static ast_node_t* range_quantifier() {
    uint32_t lower = 0;
    uint32_t upper = INF;
    match(TOKEN_LEFT_BRACE);
    if (!range_quantifier_bound(&lower))
        parser_abort("missing lower range for quantifier");
    upper = lower;
    if (lower > 10000)
        parser_abort("lower range is too large for the quantifier");
    if (match(TOKEN_COMMA)) {
        if (range_quantifier_bound(&upper)) {
            if (upper > 10000)
                parser_abort("upper range is too large for the quantifier");
            if (upper < lower)
                parser_abort("lower range is greater than upper range for the quantifier");
        } else {
            upper = INF;
        }
    }
    match_or_abort(TOKEN_RIGHT_BRACE, "unclosed range quantifier, expect '}'");
    return ast_node_quantifier(lower, upper);
}


// QuantifierType   ::= '*' | '+' | '?' | RangeQuantifier
static ast_node_t* quantifier_type() {
    if (match(TOKEN_STAR))      return ast_node_quantifier(0, INF);
    if (match(TOKEN_PLUS))      return ast_node_quantifier(1, INF);
    if (match(TOKEN_QUESTION))  return ast_node_quantifier(0, 1);
    return range_quantifier();
}


// Quantifier       ::= QuantifierType '?'?
static ast_node_t* quantifier() {
    // TODO: '?'
    ast_node_t* node = quantifier_type();
    return node;
}


// Group ::= '(' '?:'? Expression ')' Quantifier?
static ast_node_t* parse_group() {
    match(TOKEN_LEFT_PAREN);
    ast_node_t* node = ast_node_group(expression());
    match_or_abort(TOKEN_RIGHT_PAREN, "unclosed subpattern, expect ')'");
    if (follow_quantifier()) node->quantifier = quantifier();
    return node;
}


// CharacterClass ::= '\w' | '\W' | '\d' | '\D' | '\s' | '\S'
static ast_node_t* char_class() {
    ast_node_t* node = ast_node_char_class(consume().value);
    return node;
}


// CharacterRange ::= Char ( '-' Char )?
static ast_node_t* char_range() {
    token_t lower = consume();
    if (match_range_separator()) {
        if (follow_char_range()) {
            token_t upper = consume();
            if (lower.value > upper.value)
                parser_abort("lower char is greater than upper char in char range");
            return ast_node_char_range(lower.value, upper.value);
        }
        parser_abort("Unclosed char range");
    }
    return ast_node_char(lower.value);
}


// CharacterGroupItem ::= CharacterClass | CharacterClassFromUnicodeCategory | CharacterRange | Char
static ast_node_t* char_set_item() {
    // TODO: Within a character set, only \, -, and ] need to be escaped.
    if (follow_char_class()) return char_class();
    if (follow_char_range()) return char_range(); // CharacterRange and Char
    parser_abort("Invalid char set item");
    return NULL;
}


// CharacterGroup ::= '[' '^'? CharacterGroupItem+ ']'
// I'm calling it CharacterSet
static ast_node_t* char_set() {
    match(TOKEN_LEFT_BRACKET);
    parser.inside_set = true;
    bool negative = match(TOKEN_CIRCUMFLEX);
    ast_node_t* node = ast_node_set(negative);
    while (follow_char_set_item()) {
        ast_node_t* item = char_set_item();
        ast_node_set_add_item(node, item);
    }
    match_or_abort(TOKEN_RIGHT_BRACKET, "unclosed char set, expect ']'");
    if (node->as.set.items.head == NULL)
        parser_abort("Empty char set");
    parser.inside_set = false;
    return node;
}


// MatchCharacterClass ::= CharacterGroup | CharacterClass | CharacterClassFromUnicodeCategory
static ast_node_t* match_char_class() {
    if (follow_char_set())    return char_set();
    if (follow_char_class())  return char_class();
    parser_abort("Unexpected char class");
    return NULL;
}


// MatchCharacter ::= Char
static ast_node_t* match_char() {
    ast_node_t* node = ast_node_char(consume().value);
    return node;
}


// MatchItem  ::= '.' | MatchCharacterClass | MatchCharacter
static ast_node_t* match_item() {
    if (match(TOKEN_DOT))               return ast_node_any();
    if (follow_match_char_class())      return match_char_class();
    if (follow_match_char())            return match_char();
    parser_abort("Unexpected item");
    return NULL;
}


// Match ::= MatchItem Quantifier?
static ast_node_t* parse_match() {
    ast_node_t* node = match_item();
    if (follow_quantifier()) node->quantifier = quantifier();
    return node;
}


// SubexpressionItem ::= Match | Group | Anchor | Backreference
static ast_node_t* subexpression_item() {
    if (follow_match())             return parse_match();
    if (follow_group())             return parse_group();
    //if (follow_anchor())          parse_anchor();
    //if (follow_backreference())   parse_backreference();
    parser_abort("Unexpected subexpression item");
    return NULL;
}


//Subexpression ::= SubexpressionItem+
static ast_node_t* subexpression() {
    if (follow_subexpression_item()) {
        ast_node_t* node = subexpression_item();
        while (follow_subexpression_item()) {
            node = ast_node_concat(node, subexpression_item());
        }
        return node;
    }
    parser_abort("Unexpected subexpression");
    return NULL;
}


// Expression ::= Subexpression ( '|' Subexpression )*
static ast_node_t* expression() {
    ast_node_t* node = subexpression();
    while (match(TOKEN_PIPE))
        node = ast_node_alternation(node, subexpression());
    return node;
}


// Regex    ::= '^'? Expression
static ast_node_t* parse_regex() {
    if (match(TOKEN_CIRCUMFLEX))
        return ast_node_start_of_line(expression());
    return expression();
}


ast_node_t* parse(const char* regex) {
    scanner_init(regex);
    parser_init();
    ast_node_t* ast = parse_regex();
    return ast;
}