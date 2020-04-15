#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "StringSearch.h"

typedef struct {
    char *line;
    size_t length;
    ssize_t bytes_read;

    bool matched;
    int line_number;
    int bytes_offset;
} line_read_args;

typedef struct {
    int match_counter;
    int total_bytes;
    int lines_to_print; 
} counters;

bool readLine(FILE* stream, line_read_args* line_params);

bool isMatchInLine(char* expression, char* line, command_flags* flags);

void convertToLowercase(char* string);

char* createLowercaseCopyOfLine(char* line);

bool shouldPrintLine(bool matched_line, command_flags* flags, counters* counter);

void formatCountPrints(command_flags* flags, int match_counter, int line_number);

void formatStringPrints(command_flags* flags, line_read_args* line_params, int lines_to_print, bool* finished_block);

void prepareLowercase(char* expression, char* line, char** lowercase_line);

void updateCounters(line_read_args* line_params, counters* counter, command_flags* flags, bool* finished_block);
            
void stripTrailingNewline(char* line);

void searchInSource(char* expression, command_flags* flags, FILE* source)
{
    bool finished_reading = false;
    line_read_args line_params = {NULL, 0, 0, false, 0, 0};
    counters counter = {0, 0, -1};

    bool finished_block = false;
    while (!finished_reading)
    {
        line_params.line_number++;
        finished_reading = readLine(source, &line_params);
        if (!finished_reading){
            line_params.matched = isMatchInLine(expression, line_params.line, flags); 
            updateCounters(&line_params, &counter, flags, &finished_block);
            if (shouldPrintLine(line_params.matched, flags, &counter))
                formatStringPrints(flags, &line_params, counter.lines_to_print, &finished_block);
            }
        }
    

    formatCountPrints(flags, counter.match_counter, line_params.line_number);
    free(line_params.line);
}

bool readLine(FILE* stream, line_read_args* line_params)
{
    line_params->bytes_read = getline(  &line_params->line,
                                        &line_params->length,
                                        stream );
    if (line_params->bytes_read == -1)
    {
        return true;
    }
    stripTrailingNewline(line_params->line);
    return false;
}

bool isMatchInLine(char* expression, char* line, command_flags* flags)
{
    bool match;
    char* lowercase_line = NULL;
    char* line_to_check = line;

    if (flags->ignore_case){
        prepareLowercase(expression, line, &lowercase_line);
        line_to_check = lowercase_line;
    }

    if (strstr(line_to_check, expression) != NULL)
    {
        if (flags->perfect_match)
            match = (strlen(line_to_check) == strlen(expression));
        else match = true;
    }
    else
    {
        match = false;
    }

    if (lowercase_line != NULL)
        free(lowercase_line);

    if (flags->invert_match)
        return !match;

    else return match;
}


void convertToLowercase(char* string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        string[i] = tolower(string[i]);
    }
}
char* createLowercaseCopyOfLine(char* line)
{
    int line_length = strlen(line) + 1;
    char* lowercase_copy = malloc(line_length * sizeof(char));
    strncpy(lowercase_copy, line, line_length);
    convertToLowercase(lowercase_copy);
    return lowercase_copy;
}

bool shouldPrintLine(bool matched_line, command_flags* flags, counters* counter){

    if (flags->line_count)
        return false;

    bool match_inversion_check = (matched_line);
    bool after_context_check = (flags->after_context && counter->lines_to_print > 0);

    return (match_inversion_check || after_context_check);
}
void formatCountPrints(command_flags* flags, int match_counter, int line_number)
{
    if (!flags->line_count)
        return;

    printf("%d\n", match_counter);
}


bool printDueToAFlag(command_flags* flags, int lines_to_print){
    if (!flags->after_context)
    {   
        return false;
    }

    bool more_lines_to_print_in_context = lines_to_print > 0; 
    bool not_the_first_match_in_context = lines_to_print < flags->after_context_val + 1;

    if (not_the_first_match_in_context && more_lines_to_print_in_context) 
        return true;
}

void formatStringPrints(command_flags* flags, line_read_args* line_params, int lines_to_print, bool* finished_block)
{
    //if (!should_print)
    //    return;

    char delimiter = ':';
    if (printDueToAFlag(flags, lines_to_print))
        {
            delimiter = '-';
        }

    if (flags->after_context && *finished_block){
        printf("--\n");
        *finished_block = false;
    }
    if (!flags->bytes && !flags->line_number)
        printf("%s\n", line_params->line);

    else if (flags->bytes && !flags->line_number)
        printf("%d%c%s\n", line_params->bytes_offset, delimiter, line_params->line);

    else if (!flags->bytes && flags->line_number)
        printf("%d%c%s\n", line_params->line_number, delimiter, line_params->line);

    else if (flags->bytes && flags->line_number)
        printf("%d%c%d%c%s\n", line_params->line_number, delimiter, line_params->bytes_offset,
                delimiter, line_params->line);
}

void prepareLowercase(char* expression, char* line, char** lowercase_line)
{
    convertToLowercase(expression);
    *lowercase_line = createLowercaseCopyOfLine(line);
}

void updateCounters(line_read_args* line_params, counters* counter, command_flags* flags, bool* finished_block)
{
    if (line_params->matched)
    {
            counter->match_counter++;
            if (flags->after_context)
                counter->lines_to_print = flags->after_context_val + 1;
    }
    
    else if (flags->after_context && counter->lines_to_print > -1)
    {
            counter->lines_to_print--;
            if (counter->lines_to_print == 0)
                *finished_block = true;
    }

    counter->total_bytes = counter->total_bytes + line_params->bytes_read;
    line_params->bytes_offset = counter->total_bytes - line_params->bytes_read;
}

void stripTrailingNewline(char* line)
{
        line[strlen(line) - 1] = '\0';
}

