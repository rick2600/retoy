#include "re.h"
#include <stdbool.h>

// TODO: check malloc return
re_submatch_t* re_submatch(uint8_t* start, uint8_t* end) {
    re_submatch_t* sub = malloc(sizeof(re_submatch_t));
    sub->start = start;
    sub->end = end;
    sub->next = NULL;
    return sub;
}


re_match_t* re_match_create(uint8_t* input) {
    re_match_t* match = malloc(sizeof(re_match_t));
    match->input = input;
    match->head = NULL;
    match->tail = NULL;
    return match;
}


void re_add_submatch(re_match_t* match, re_submatch_t* sub) {
    if (match->head == NULL)
        match->head = sub;
    else
        match->tail->next = sub;
    match->tail = sub;
}


static bool match_part(re_match_t* match, uint8_t* p) {
    for (re_submatch_t* cur = match->head; cur; cur = cur->next)
        if (cur->start <= p && p <= cur->end) return true;
    return false;
}

void re_print_match(re_match_t* match) {
    for (uint8_t* cur = match->input; *cur; cur++) {
        if (match_part(match, cur))
            printf(YELLOW"%c"RESET, *cur);
        else
            printf("%c", *cur);
    }
    printf("\n");
}


void re_free(re_match_t* match) {
    for (re_submatch_t* cur = match->head; cur; ) {
        re_submatch_t* next = cur->next;
        free(cur);
        cur = next;
    }
    free(match);
}
