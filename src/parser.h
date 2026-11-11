/*
 * This is the parser which actually reads the program's input
 * code using the syntax specified
 */
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "element-map.h"
#include "ti-error.h"

// The idea is to use a switch condition here to test
// for the input instruction and convert it properly using all parameters 
// to a single calculator instruction
typedef enum input_instructions_enum {
    instr_number = 0,
    prog_pointer = 1, // Used as destination for a GOTO or SUBROUTINE call to abstract from actual line numbers
    ret_alias = 2, // Simple alias to the INV SBR instruction
    reg_variable = 3,
    instr_CLR = 20,
    instr_NatLog = 23,
    instr_CE = 24,
    instr_LOG = 28,
    instr_CP = 29,
    instr_TAN = 30,
    instr_EXCXT = 32,
    instr_SQR = 33,
    instr_SQRT = 34,
    instr_RECIP = 35,
    instr_PGM = 36,
    instr_PTOR = 37,
    instr_SIN = 38,
    instr_COS = 39,
    instr_STO = 42,
    instr_RCL = 43,
    instr_SUM = 44,
    instr_POW = 45,
    instr_CMs = 47,
    instr_EXC = 48,
    instr_PROD = 49,
    instr_ABS = 50,
    instr_EE = 52,
    instr_POPEN = 53,
    instr_PCLOSE = 54,
    instr_DIV = 55,
    instr_ENG = 57,
    instr_FIX = 58,
    instr_INTG = 59,
    instr_DEG = 60,
    instr_GTO = 61,
    instr_MUL = 65,
    instr_PAUSE = 66,
    instr_EQ = 67,
    instr_NOP = 68,
    instr_OP = 69,
    instr_RAD = 70,
    instr_SBR = 71,
    instr_SUB = 75,
    instr_LBL = 76,
    instr_GE = 77,
    instr_STATSUM = 78,
    instr_MEAN = 79,
    instr_GRAD = 80,
    instr_RST = 81,
    instr_ADD = 85,
    instr_STFLG = 86,
    instr_IFFLG = 87,
    instr_DMS = 88,
    instr_PI = 89,
    instr_LIST = 90,
    instr_STOP = 91,
    instr_NEG = 94,
    instr_RESULT = 95,
    instr_WRITE = 96,
    instr_DSZ = 97,
    instr_ADV = 98,
    instr_PRT = 99
} input_instructions;

typedef enum parameter_type_enum {
    program_code, // Used with the PGM key to select a program number to run from the library
    register_index, // Used for STO, RCL, SUM, and various functions as an index to a memory register
                    // In case register_index is used, the indirect flag in the input parameter indicates
                    //  whether this register is a pointer or not
    decimal_amount, // Used with FIX display mode to indicate a number between 0 and 8 decimal digits to use statically
    location,   // Used with GTO, SBR, to indicate a location or a label as a destination
                // With this parameter type, the value indicates the program pointer ID, otherwise
                //  label is used to indicate the destination label
    label,  // Used with LBL to create a new label
    operation,  // Used with OP to run a specialized operation
    digit   // The parameter value here must be a digit between 0 and 9, this is
            //  used with STFLG or IFFLG to indicate the flag to set/clear/check,
            //  or with DSZ for the register value to read, which must be single
            //  digit.
} parameter_type;

typedef enum uncommon_labels_enum {
    ulabel_INV = 22, // The key relative to the inverse operation
    ulabel_DEC = 93  // The decimal point key
} uncommon_labels;

typedef enum user_defined_keys_enum {
    // Here follow all the 10 user-defined keys which are not valid instructions in this program
    A_lbl = 11,
    B_lbl = 12,
    C_lbl = 13,
    D_lbl = 14,
    E_lbl = 15,
    A_first = 16,
    B_first = 17,
    C_first = 18,
    D_first = 19,
    E_first = 10
} user_defined_keys;

typedef struct label_code_str {
    /*
     * All of this program's instructions are usable as labels,
     * except the number, program pointer and return instructions which, if inserted in the 
     * enumerator, renders this label invalid. (those are values under 10 so they 
     * can be easily checked).
     * However, there are some possible keyboard keys usable as labels
     * which are not valid instructions in this program, this structure
     * then contains three enumerators, one with the regular instruction
     * set as domain, another which contains the uncommon instructions
     * not available in this program, and a third is separately for the user
     * defined keys (A-E and A'-E').
     * This label type being set to 0 indicates the usage of a common label,
     * a negative value indicates the usage of a user defined key, and a
     * positive value indicates the usage of one of the uncommon labels.
    */
    signed char label_type;
    union {
        input_instructions common_label;
        uncommon_labels uncommon_label;
        user_defined_keys user_defined_key;
    } label;
} label_code;

typedef struct input_parameter_str {
    parameter_type type;
    // This indicates whether this parameter is indirect (which means it contains a register pointer)
    //  when this is true, the value always contains the register pointer which contains the indirect value to place here
    bool indirect;
    // This union either contains the value or the label in case of a label type,
    // this flag saves whether it contains a raw value, it's false in case of a label.
    bool is_value; // NOTE: only used for location parameter_type.
    union {
        int value;
        label_code label;
    } data;
} input_parameter;

/*
 * Union used for the instruction data.
 * This either contains the number if a 
 * number instruction is used, or the ID
 * to a program pointer
 */
typedef union instr_data_union {
    unsigned int prog_pointer;
    struct number_str {
        // The number
        double number;
        // Amount of digits (counting decimal point) this number has
        unsigned int size;
    } number_instr;
} instr_data;

/* This is the object which wraps a single line of code*/
typedef struct input_line_str {
    // Whether this instruction is inverted
    // Edge case: setting this to true on a return instruction
    //  will revert the instruction back to SBR since return
    //  is just an alias for the inversion of the subroutine call.
    bool inverse;
    input_instructions instruction;
    // In case of a number instruction, this contains the number to type in the program
    // In case of a program pointer, this contains the assigned ID to that program pointer.
    instr_data data;
    // Here follow the parameters used for the instruction
    unsigned short parameters_amount;
    input_parameter* parameters;
} input_line;

// ====== FUNCTION DEFINITION ======

/*
 * This function parses a line from the file
 * allocating the correct amount of memory to
 * it. 
 * Comments are already discarded here, to save memory
 * they are not contained in the dynamic memory allocation.
 *
 * NOTE: The returned pointer is the pointer to 
 *       a dynamically allocated heap array,
 *       which MUST be manually free()'d to avoid
 *       memory leaks!
 */
char* f_getline(FILE* in, size_t* line_length);

/*
 * After having read a line of code from the file
 * using f_getline(FILE*, size_t*), this function 
 * parses the entire line into a specialized
 * input_line type which allows for the compiler
 * to properly reconstruct the amount of instructions
 * required for that line of code.
 */
return_val parse_line(input_line* output, char* line_buffer, size_t bufsize);

/*
 * Since this parser uses a bunch of memory
 * allocations for the hashmaps, this function
 * manually clears all of them.
 *
 * NOTE: After reading the entire input file
 *          this function MUST be called to free
 *          dynamically-allocated memory!
 */
void clear_parser_memory();

#endif