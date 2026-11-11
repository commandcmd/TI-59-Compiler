#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "parser.h"
#include "ti-error.h"

/* Merged-opcode indirect instructions
 * are defined here.
 */
typedef enum ind_instr_enum { 
    instr_IND = 40,
    instr_PGM_ind = 62,
    instr_EXC_ind = 63,
    instr_PRD_ind = 64,
    instr_STO_ind = 72,
    instr_RCL_ind = 73,
    instr_SUM_ind = 74,
    instr_GTO_ind = 83,
    instr_OP_ind = 84
} ind_instructions;

/*
 * Structure for the user definition of
 * a raw calculator instruction, which puts
 * together a bunch of pieces from enumerators
 * defined in the parser.
*/
typedef struct instruction_str {
    /*
     * This enumerator contains what kind of
     * instruction is saved in this structure.
    */
    enum {
        instyp_ucommon_label, // Used when the uncommon_label is used
        instyp_common_instr, // Used when the common_instr is defined
        instyp_ind_instr, // Used when a special indirect instruction is specified
        instyp_user_key, // Used when the user_key is used
        // Used when a raw number is defined, which can either
        //  be an actual number instruction or a custom opcode
        //  for the argument of the instruction sitting in the
        //  memory slot right before.
        instyp_number,
        // Same as instyp_number but used with a single digit
        instyp_digit,
        // Used for program indexes, they are written to the
        //  file as placeholder values for linking, they start
        //  with a dollar sign and the linker then associates 
        //  them with the correct line number.
        instyp_progindex,
        // This is a specific instruction type
        // used to trigger an error into the 
        // function which prints the instruction
        // to file in certain situations.
        instyp_invalid
    } instr_type;

    union {
        /* Most of common instructions are saved in this object,
         * the aliases or special instructions (enumerator
         *  values under 10) are not valid in this object
         */
        input_instructions common_instr;
        /* Uncommon labels are saved here */
        uncommon_labels uncommon_label;
        /* In case of a user key instruction, it is saved here */
        user_defined_keys user_key;
        /* Custom indirect instructions with merged opcode*/
        ind_instructions ind_instr;
        /* In case of a number, it's saved here.
         * numbers are independent instructions but are saved
         * in the calculator memory directly as is.
         * This value can be used to specify generic
         * opcodes and argument values for instructions
         * that require such like the GTO one which needs
         * two memory slots containing the destination to jump to.
         * This distinction is based on the sign of this character,
         * which can either be a custom code from 0-99 or a number
         * instruction if negative, except for the 0 number 
         * instruction, which is represented as -10.
         */
        signed char number;
    } instr;
} raw_instruction;

/*
 * Given an input line of code, this converts it into
 * a series of strings, each representing a location
 * register in the calculator.
 * The output location registers are written directly to the 
 * output file, each following a pattern:
 * [location line] [instruction code]\t[equivalent key]
 * This is done so that the user can look in the calculator's
 * LRN mode to check if the program has been typed
 * correctly by using location lines and instruction codes
 * and they can still type the program in the calculator
 * by pressing the keys displayed at the end of the string.
 * Location lines and instruction codes are always
 * normalized respectiely to three and two digits.
 * Equivalent keys are either strings containing the 
 * key to type on the calculator, or an argument for 
 * an instruction (such as for the PGM instruction, 
 *  which requires a two digit number, this function 
 *  will print in the file something like:
 *  001 36    -2nd-  Pgm
 *  002 02    [02]
 *  So that the user knows they need to type PGM and
 *  then the number 02)
 * The location return value of this function represents
 * the argument which the function was compiling while
 * encountering the error.
 * This function might return nested error codes.
 */
return_val convert_line_to_instr(input_line in, FILE* out, int* reg_pos);

/*
 * This function must run after all lines have already
 * been written into a file, this seeks through the
 * entire file and replaces all the dynamic program
 * pointer references with the relative actual instruction
 * line codes for the program pointer.
 * The location integer in the return value of this function,
 * in case of error, contains the index of the actual 
 * instruction that was being replaced when the error
 * occurred, if the instruction to replace was the fourth
 * one in the file to be replaced, the location value
 * contains the number '3'.
 *
 * Note that this must be ran BEFORE calling free_compiler_mem()
 * otherwise the references to the program pointers are not
 * present in memory.
 */
return_val final_compile(FILE* file);

/*
 * This must always be called if any line is ever
 * converted to a string, since the compiler creates
 * some dynamic memory allocations that must be free'd
 * after the program's execution to avoid memory
 * leaks.
*/
void free_compiler_mem();

#endif