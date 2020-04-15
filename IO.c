#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "StringSearch.h"


typedef struct argument_indices
{
    int filename_index_in_argv;
    int expression_index_in_argv;
} arguments;

bool isFlag(char* argument);

bool isFlagWithValue(char* argument);

void assignFlags(command_flags* flags, char* argv[], int argc, arguments* indices);

void setFlagField(command_flags* flags, char* argument, bool* flag_with_value);

void openFile(FILE** file_ptr, char* filename);

void debugPrintFlags(command_flags* flags);

bool foundFilename(arguments* indices);

void setExpressionAndFilename(int non_flag_counter, arguments* indices, int index);

int main(int argc, char* argv[])
{

    command_flags flags = {false, false, false, false, false, false, false, false};
    arguments indices = {-1, -1};

    assignFlags(&flags, argv, argc, &indices);
    //debugPrintFlags(&flags);
    FILE* source = NULL;
    if (foundFilename(&indices))
    {
        openFile(&source, argv[indices.filename_index_in_argv]);
    }
    else
        source = stdin;

    searchInSource(argv[indices.expression_index_in_argv], &flags, source);

}


bool isFlag(char* argument){
    if (argument[0] == '-'){
        return true;
    }
    return false;
}

bool isFlagWithValue(char* argument){
    if (!isFlag(argument)){
        return false;
    }

    if (strncmp(argument, "-A", 2) != 0){
        return false;
    }

    return true;
}


void assignFlags(command_flags* flags, char* argv[], int argc, arguments* indices)
{
    bool flag_with_value = false;

    int non_flag_counter = 0;
    for (int i = 1; i < argc; i++)
    {
        if (isFlag(argv[i]))
        {
            setFlagField(flags, argv[i], &flag_with_value);
            if (flag_with_value){
                flags->after_context_val = atoi(argv[i+1]);
                i++;
                flag_with_value = false;
            }
        }
        else 
        {
            non_flag_counter++;
            setExpressionAndFilename(non_flag_counter, indices, i);
        }
    }
}


void setFlagField(command_flags* flags, char* argument, bool* flag_with_value)
{
    if (!strncmp(argument, "-i", strlen(argument)))
    {
        flags->ignore_case = true;
    }
    if (!strncmp(argument, "-v", strlen(argument)))
    {
        flags->invert_match = true;
    }
    if (!strncmp(argument, "-c", strlen(argument)))
    {
        flags->line_count = true;
    }
    if (!strncmp(argument, "-n", strlen(argument)))
    {
        flags->line_number = true;
    }
    if (!strncmp(argument, "-b", strlen(argument)))
    {
        flags->bytes = true;
    }
    if (!strncmp(argument, "-x", strlen(argument)))
    {
        flags->perfect_match= true;
    }
    if (!strncmp(argument, "-A", strlen(argument)))
    {
        flags->after_context = true;
        *flag_with_value = true;
    }
    if (!strncmp(argument, "-E", strlen(argument)))
    {
        flags->regex = true;
    }
}

void openFile(FILE** file_ptr, char* filename){
    *file_ptr = fopen(filename, "r");
    if (*file_ptr == NULL){
        printf("File not found\n");
        exit(1);
    }
}

void debugPrintFlags(command_flags* flags){
    if (flags->ignore_case)
        printf("ignore case = true\n");
    else
    {
        printf("ignore case = false\n");
    }
    if (flags->invert_match)
        printf("reverse match = true\n");
    else{
        printf("reverse match = false\n");
    }
    if (flags->line_count)
        printf("line count = true\n");
    else{
        printf("line count  = false\n");
    }
    if (flags->line_number)
        printf("line number = true\n");
    else{
        printf("line number  = false\n");
    }

    if (flags->bytes)
        printf("bytes = true\n");
    else{
        printf("bytes  = false\n");
    }
    if (flags->perfect_match)
        printf("greedy match = true\n");
    else{
        printf("greedy match  = false\n");
    }
    if (flags->after_context)
    {   printf("after lines = true\n");
        printf("after lines val = %d\n", flags->after_context_val);
    }
    else{
        printf("after lines  = false\n");
    }

    if (flags->regex)
        printf("regex = true\n");
    else{
        printf("regex  = false\n");
    }
}

bool foundFilename(arguments* indices){
    if (indices->filename_index_in_argv != -1){
        return true;
    }
    return false;
}
void setExpressionAndFilename(int non_flag_counter, arguments* indices, int index)
{
    /* We assume that if a filename appears in the CL arguments, it is the last argument
     * therefore, the first non flag argument we encounter must be the expression to search */
    if (non_flag_counter == 1)
        indices->expression_index_in_argv = index;
    else {
        indices->filename_index_in_argv = index;
    }

}
