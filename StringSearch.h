#ifndef STRING_SEARCH_H
#define STRING_SEARCH_H

typedef struct flags
{
    bool ignore_case;
    bool invert_match;
    bool line_count;
    bool line_number;
    bool bytes;
    bool perfect_match;
    bool after_context;
    int after_context_val;
    bool regex;

} command_flags;

void searchInSource(char* expression, command_flags* flags, FILE* source);

#endif
