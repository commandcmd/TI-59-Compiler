#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"
#include "compiler.h"
#include "ti-error.h"

/*
 * The file pointer of the input to compile with the source code
 * and the output file to write with all explicitly written
 * instructions.
*/
FILE *input_file, *output_file;

struct program_arguments_str {
    char* output_file; // The output file string to use, if NULL just use stdout
    bool no_keystrokes; // Don't display the program keystrokes in the output file
    bool no_opcodes; // Don't display the program opcodes in the output file
} program_arguments;

/*
 * Parser for the program parameters passed as input.
 * Fills the contents of the global struct program_arguments.
*/
void parse_args(int argc, char** argv) {
    // Start from argument 2 (0 is the program name, 1 is the input file)
    int current_arg_index = 2;

    // Initialise the program arguments structure
    program_arguments.output_file = NULL;
    program_arguments.no_keystrokes = false;
    program_arguments.no_opcodes = false;

    // This skips immediately in case argc is 2 (only the input file has been specified)
    while(current_arg_index < argc) {
        char* current_arg = argv[current_arg_index];
        // If present, the parameter is equal to the next argument in the argument vector
        char* current_param = NULL; // This might not even be relevant for arguments who don't accept parameters
        if(current_arg_index < argc - 1)
            current_param = argv[current_arg_index + 1];

        if(strcmp(current_arg, "-o") == 0) { // Parameter is output file
            // If the output file isn't present, ignore
            if(current_param != NULL) {
                program_arguments.output_file = current_param;
            }
        } else if(strcmp(current_arg, "-nk")) { // No keystrokes argument, no parameter
            program_arguments.no_keystrokes = true;
        } else if(strcmp(current_arg, "-no")) { // No op codes argument, no parameter
            program_arguments.no_opcodes = true;
        } 

        // Increment the current argument iterator
        current_arg_index++;
    }
}

// Counter used to count the current instruction in the output
int instruction_counter = 0;

int main(int argc, char** argv) {
    // Check if an input file has been specified
    if(argc < 2) {
        fprintf(stderr, "Error! No input file specified!");
        return 1;
    }

    // Open the input file passed for writing, if it doesn't exist, it's created
    input_file = fopen(argv[1], "r");
    // If the file didn't exist then show an error and skip
    if(input_file == NULL) {
        fprintf(stderr, "Error! Input file specified doesn't exist!");
        return 2;
    }

    // Parse the rest of the arguments
    parse_args(argc, argv);
    // Open the output file, if requested
    if(program_arguments.output_file == NULL) output_file = stdout;
    else output_file = fopen(program_arguments.output_file, "w+");

    // Program line counter
    unsigned int line_counter = 0;
    // Keep reading until EOF
    do {
        // Increment the line counter
        line_counter++;
        // Obtain a line
        size_t current_line_len;
        char* current_line = f_getline(input_file, &current_line_len);

        if(current_line == NULL) continue;

        // Structure where to parse the current line
        input_line parse_output;
        return_val parse_result = parse_line(&parse_output, current_line, current_line_len);

        if(!is_success(parse_result)) {
            // Empty line, ignore
            if(parse_result.error == PARSER_NO_TOKENS)
                continue;

            // Otherwise print the error
            fprintf(stderr, "Parse error at line %d", line_counter);
            if(parse_result.location >= 0) fprintf(stderr, " - Argument %d", parse_result.location + 1);
            fprintf(stderr, "\n%s\n", error_to_str(parse_result.error));
            free(current_line);
            clear_parser_memory();
            free_compiler_mem();
            return 1;
        }

        // Free the memory for the obtained line
        free(current_line);

        // Now that a structure is present, compile the parsed line in the output file
        return_val compile_result = convert_line_to_instr(parse_output, output_file, &instruction_counter);

        if(!is_success(compile_result)) {
            fprintf(stderr, "Compilation error at line %d", line_counter);
            if(compile_result.location >= 0) fprintf(stderr, " - Argument %d", compile_result.location + 1);
            fprintf(stderr, ":\n%s\n", error_to_str(compile_result.error));
            if(compile_result.cause != SUCCESS) fprintf(stderr, "\n\tCause: %d\n", error_to_str(compile_result.cause));
            clear_parser_memory();
            free_compiler_mem();
            return 2;
        }
    } while(!feof(input_file));

    // Free the parser memory, won't read from input file again
    clear_parser_memory();

    // Perform the final compile step before freeing the compiler's memory
    return_val final_result = final_compile(output_file);
    if(!is_success(final_result)) {
        fprintf(stderr, "Post-compilation error at line %d", line_counter);
        if(final_result.location >= 0) fprintf(stderr, " - Location %d", final_result.location + 1);
        fprintf(stderr, ":\n%s\n", error_to_str(final_result.error));
        free_compiler_mem();
        return 3;
    }

    // Clear the compiler memory, stopped compiling
    free_compiler_mem();

    return 0; // Success output
}