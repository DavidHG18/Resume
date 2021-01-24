/** 
 * Assignment goal: to read source code and output the  
 * Javadoc/doxygen-style tags contained in the comments.
 * 
 * 2 Cases:
 *  1. All tags are output
 *  2. Only tags at the beginning of a line are output
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum curr_mode {ALL, LEADING} mode;
typedef enum state {
    CODE, CODE_COMMENT_BEFORE, CODE_SINGLE, STRING_WAITING, STRING_CODE,
    CODE_MULTI, CODE_SINGLE_FORWARD, WAITING, BACKSLASH, 
    SINGLE_LINE_BEFORE, SINGLE_LINE_BEFORE_FORWARD_SLASH, SINGLE_LINE_BEFORE_WORD, 
    SINGLE_LINE, SINGLE_LINE_AFTER, SINGLE_FORWARD_SLASH,
    MULTI_LINE_BEFORE, MULTI_LINE, MULTI_LINE_AFTER, MULTI_LINE_BEFORE_ASTERISK, MULTI_LINE_BEFORE_FORWARD,
    MULTI_LINE_ASTERISK, MULTI_FORWARD_SLASH
    } State;

int star_counter;
int end = 0;
int close = 0;
int code_multi_end = 0;
State curr = WAITING;
char value;

int main(int argc, char* argv[])
{

    // initialize input file, output file, and mode type
    FILE* input = stdin;
    FILE* output = stdout;
    mode mode_type = LEADING;

    // iterate through command-line arguments and check for conditions
    int i = 0;
    while (i < argc)
        {
            // conditions where we have an input or output file
            if (strcmp(argv[i], "-i") == 0)
                {
                    if (i == argc - 1)
                        {
                            fprintf(stderr, "%s: state input file.", argv[0]);
                            return 1;
                        }
                    else
                        {
                            input = fopen(argv[i + 1], "r");
                            i += 2;
                        }
                }
            else if (strcmp(argv[i], "-o") == 0)
                {
                    if (i == argc - 1)
                        {
                            fprintf(stderr, "%s: state output file.", argv[0]);
                            return 1;
                        }
                    else
                        {
                            output = fopen(argv[i + 1], "w");
                            i += 2;
                        }
                }
            // condition where we are going to be in "all" mode
            else if (strcmp(argv[i], "-a") == 0)
                {
                    mode_type = ALL;
                    i++;
                }
            else 
                {
                    i++;
                }
        }
    
    if (!input || !output)
        {
            fprintf(stderr, "%s: could not open file.", argv[0]);
            return 1;
        }
    
    // begin to read through file
    while (fscanf(input, "%c", &value) > 0)
        {
            switch (curr)
                {
                case CODE:
                    if (value == '/') // if potential start of comment, enter CODE_COMMENT?
                        {
                            curr = CODE_COMMENT_BEFORE;
                        }
                    else if (value == '\"')
                        {
                            curr = STRING_CODE;
                        }
                    else if (value == '}')
                        {
                            close --;
                            if (close == 0)
                                {
                                    curr = WAITING;
                                }
                        }
                    else if (value == '{')
                        {
                            close++;
                        }
                    break;

                case STRING_CODE:
                    if (value == '"')
                        {
                            curr = CODE;
                        }
                    break;
                
                case CODE_COMMENT_BEFORE:
                    if (value == '/')
                        {
                            curr = CODE_SINGLE;
                        }
                    else if (value == '*')
                        {
                            curr = CODE_MULTI;
                        }
                    else
                        {
                            curr = CODE;
                        }
                
                case CODE_SINGLE: // single-line comment in code
                    if (value == '\n')
                        {
                            curr = CODE;
                        }
                    else if (value == '\\')
                        {
                            curr = CODE_SINGLE_FORWARD;
                        }
                    break;
                
                case CODE_SINGLE_FORWARD: // just meant to skip the forward slash regardless of what it is
                    curr = CODE_SINGLE;
                    break;

                case CODE_MULTI:
                    if (value == '*')
                        {
                            code_multi_end = 1;
                        }
                    else if (value == '/' && code_multi_end == 1)
                        {
                            curr = CODE;
                        }
                    else
                        {
                            code_multi_end = 0;
                        }
                    break;

                case WAITING:
                    if (value == '{')
                        {
                            close = 1;
                            curr = CODE;
                        }
                    else if (value == '/')
                        {
                            curr = BACKSLASH;
                        }
                    else if (value == '\"')
                        {
                            curr = STRING_WAITING;
                        }
                    break;
                
                case BACKSLASH:
                    if (value == '/')
                        {
                            curr = SINGLE_LINE_BEFORE;
                        }
                    else if (value == '*')
                        {
                            star_counter = 1;
                            curr = MULTI_LINE_BEFORE;
                        }
                    else
                        {
                            curr = WAITING;
                        }
                    break;
                
                case SINGLE_LINE_BEFORE: // case where we have entered a single line comment but still have whitespace
                    
                    if (value == '\n') // edge case where we have an empty comment
                        {
                            curr = WAITING;
                        }
                    else if (value == '@')
                        {
                            fprintf(output, "%c", value);
                            curr = SINGLE_LINE;
                        }
                    else if (value == '\\')
                        {
                            curr = SINGLE_LINE_BEFORE_FORWARD_SLASH;
                        }
                    else if (!isspace(value) && value != '*') // we've seen a non-whitespace character, need to wait for another space
                        {
                            if (mode_type == LEADING)
                                {
                                    curr = SINGLE_LINE_AFTER;
                                }
                            else
                                {
                                    curr = SINGLE_LINE_BEFORE_WORD;
                                }
                        }
                    break;

                case SINGLE_LINE_BEFORE_WORD:
                    if (value == '\n') // new line, done with comment
                        {
                            curr = WAITING;
                        }
                    else if (isspace(value)) // saw a whitespace character, go back to the pre-tag era
                        {
                            curr = SINGLE_LINE_BEFORE;
                        }
                    else if (value == '\\') // forward slash, need to go to foward slash before state
                        {
                            curr = SINGLE_LINE_BEFORE_FORWARD_SLASH;
                        } 
                    break; // otherwise it is just another non-whitespace character so we stay here
                
                case SINGLE_LINE_BEFORE_FORWARD_SLASH:
                    if (value == '\n') // going to new line but should be ignored
                        {
                            curr = SINGLE_LINE_BEFORE;
                        }
                    else if (value == '@')
                        {
                            fprintf(output, "%c", value);
                            curr = SINGLE_LINE;
                        }
                    else if (isspace(value)) // if we see a space, we treat it as literal
                        {
                            curr = SINGLE_LINE_BEFORE;
                        }
                    break;

                case SINGLE_LINE: // case where we are actually reading in a single line comment
                    if (value == '\\') // if character is a forward slash we need to go to forward slash state
                        {
                            curr = SINGLE_FORWARD_SLASH;
                        }
                    else if (!isspace(value))
                        {
                            fprintf(output, "%c", value);
                        }
                    else if (value == '\n') //this is a newline, we need to go back to waiting for another comment
                        {
                            fprintf(output, "\n");
                            curr = WAITING;
                        }
                    else if (mode_type == ALL) // this means that it was some other form of space which means we should stay in read mode
                        {
                            fprintf(output, "\n");
                            curr = SINGLE_LINE_BEFORE;
                        }
                    else   
                        {
                            fprintf(output, "\n"); // print new line after comment ends
                            curr = SINGLE_LINE_AFTER;
                        }
                    break;
                
                case SINGLE_LINE_AFTER: // we're done reading a single line comment but need to get to the end of the comment
                    if (value == '\n')
                        {
                            curr = WAITING;
                        }
                    break;
    
                case SINGLE_FORWARD_SLASH: // we've seen one forward slash and do not know whether it is a line continuation character or not
                    if (value == '\n')
                        {
                            curr = SINGLE_LINE;
                        }
                    else //this means that the forward slash should be taken literally as part of the tag
                        {
                            fprintf(output, "\\");
                            if (!isspace(value))
                                {
                                    fprintf(output, "%c", value);
                                    curr = SINGLE_LINE;
                                }
                            else if (mode_type == ALL)
                                {
                                    fprintf(output, "\n");
                                    curr = SINGLE_LINE_BEFORE;
                                }
                            else 
                                {
                                    fprintf(output, "\n");
                                    curr = SINGLE_LINE_AFTER;
                                }
                        }

                case MULTI_LINE_BEFORE: // case where we have entered a single line comment but still have whitespace
                    if (value == '*') // keeping track of how many asterisks we've seen
                        {
                            star_counter++;
                            curr = MULTI_LINE_BEFORE_ASTERISK;
                        }
                    else if (value == '@')
                        {
                            fprintf(output, "%c", value);
                            curr = MULTI_LINE;
                        }
                    else if (value == '\\')
                        {
                            curr = MULTI_LINE_BEFORE_FORWARD;
                        }
                    else if (!isspace(value))
                        {
                            if (mode_type == LEADING)
                                {
                                    curr = MULTI_LINE_AFTER;
                                }
                        }
                    break;
                
                case MULTI_LINE_BEFORE_FORWARD: // goes back to multi line before regardless
                    curr = MULTI_LINE_BEFORE;

                case MULTI_LINE_BEFORE_ASTERISK:
                    if (value == '/')
                        {
                            curr = WAITING;
                        }
                    else if (value == '*')
                        {
                            star_counter++;
                        }
                    else if (isspace(value))
                        {
                            star_counter = 1;
                            curr = MULTI_LINE_BEFORE;
                        }
                    else if (value == '@')
                        {
                            fprintf(output, "%c", value);
                            curr = MULTI_LINE;
                        }
                    break;

                case MULTI_LINE: // case where we are actually reading in a multi line comment
                    if (value == '\n') //this is a newline, we need to go back to waiting for another comment
                        {
                            fprintf(output, "\n");
                            curr = MULTI_LINE_BEFORE;
                        }
                    else if (value == '*')
                        {
                            star_counter = 1;
                            curr = MULTI_LINE_ASTERISK;
                        }
                    else if (value == '\\') // if character is a forward slash we need to go to forward slash state
                        {
                            curr = MULTI_FORWARD_SLASH;
                        }
                    else if (!isspace(value))
                        {
                            fprintf(output, "%c", value);
                        }
                    else if (mode_type == ALL) // this means that it was some other form of space which means we should stay in read mode
                        {
                            fprintf(output, "\n");
                            curr = MULTI_LINE_BEFORE;
                        }
                    else   
                        {
                            fprintf(output, "\n"); // print new line after comment ends
                            curr = MULTI_LINE_AFTER;
                        }
                    break;

                case MULTI_LINE_ASTERISK:
                    if (value == '/') // end of the comment
                        {
                            for (int i = 0; i < star_counter - 1; i++)
                                {
                                    fprintf(output, "*");
                                }
                            fprintf(output, "\n");
                            curr = WAITING;
                        }
                    else if (value == '*') // we stay in this mode and continue to count the asterisks
                        {
                            star_counter++;
                        }
                    else if (isspace(value)) // end of the tag but not the comment
                        {
                            for (int i = 0; i < star_counter; i++)
                                {
                                    fprintf(output, "*");
                                }
                            star_counter = 0;
                            fprintf(output, "\n");

                            if (mode_type == ALL)
                                {
                                    curr = MULTI_LINE_BEFORE; // go back and search for next comment
                                }
                            else 
                                {
                                    curr = MULTI_LINE_AFTER;
                                }
                        }
                    else //this means that we have seen another non-whitespace character, indicating the continuation of the tag
                        {
                            for (int i = 0; i < star_counter; i++)
                                {
                                    fprintf(output, "*");
                                }
                            star_counter = 0;
                            curr = MULTI_LINE;
                        }
                    break;
                
                case MULTI_LINE_AFTER: // done reading in tags but waiting to get to end of comment
                    if (value == '\n')
                        {
                            curr = MULTI_LINE_BEFORE;
                        }
                    else if (value == '*') // see an asterisk, indicate potential end of comment
                        {
                            end = 1;
                        }
                    else if (value == '/' && end == 1)
                        {
                            curr = WAITING;
                        }
                    else // see a non-asterisk or non-newline character, keep going
                        {
                            end = 0;
                        }
                    break;

                case MULTI_FORWARD_SLASH: // we've seen one forward slash and do not know whether it is a line continuation character or not
                    if (value == '\n')
                        {
                            curr = MULTI_LINE;
                        }
                    else //this means that the forward slash should be taken literally as part of the tag
                        {
                            fprintf(output, "\\");
                            if (!isspace(value))
                                {
                                    fprintf(output, "%c", value);
                                    curr = MULTI_LINE;
                                }
                            else if (value == '*')
                                {
                                    star_counter = 1;
                                    curr = MULTI_LINE_ASTERISK;
                                }
                            else if (mode_type == ALL)
                                {
                                    fprintf(output, "\n");
                                    curr = MULTI_LINE_BEFORE;
                                }
                            else 
                                {
                                    fprintf(output, "\n");
                                    curr = MULTI_LINE_AFTER;
                                }
                        }
                    break;
                
                case STRING_WAITING:
                    if (value == '\"')
                        {
                            curr = WAITING;
                        }
                    break;
                }
        }
    return 0;
}


