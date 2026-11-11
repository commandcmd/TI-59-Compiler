#ifndef TI_ERROR_H
#define TI_ERROR_H

/*
 * Enumerator used to store all different
 * possible error codes, this can then
 * be passed to a function to convert
 * the error code into a proper printable
 * string.
 */
typedef enum return_code_enum {
    // No error
    SUCCESS = 0,

    /* General error codes */
    PROGRAM_POINTER_ALLOC_ERROR = -1,
    VARIABLE_REGISTER_ALLOC_ERROR = -2,

    /* Parser error codes */
    PARSER_NO_TOKENS = 100,
    PARSER_INSTR_NO_TOKEN = 101,
    PARSER_INSTR_NO_NUMBER = 102,
    PARSER_INSTR_NO_INV = 103,
    PARSER_INSTR_INVALID_INV = 104,
    PARSER_INSTR_VAR_NO_TOKEN = 105,
    PARSER_INSTR_VAR_NO_EQUAL = 106,
    PARSER_INSTR_VAR_NO_REG = 107,
    PARSER_INSTR_VAR_INVALID_REG = 108,
    PARSER_INSTR_VAR_REG_OUTOFRANGE = 109,
    PARSER_INSTR_INVALID_TOKEN = 110,
    PARSER_INSTR_TOO_LONG_TOKEN = 111,
    PARSER_INSTR_INVALID_INSTR = 112,
    PARSER_ARG_ALLOC_ERROR = 150,
    PARSER_ARG_NO_TOKEN_FOUND = 151,
    PARSER_ARG_IND_NO_VALUE = 152,
    PARSER_ARG_IND_NO_NUMBER = 153,
    PARSER_ARG_IND_OUTOFRANGE = 154,
    PARSER_ARG_IND_NO_VARS = 155,
    PARSER_ARG_IND_UNDECLARED_VAR = 156,
    PARSER_ARG_VARIABLE_OUTOFRANGE = 157,
    PARSER_ARG_INPUT_OUTOFRANGE = 158,
    PARSER_ARG_DIGIT_OUTOFRANGE = 159,
    PARSER_ARG_NO_NUMBER = 160,
    PARSER_ARG_LABEL_INVALID_TOKEN = 161,
    PARSER_ARG_LABEL_UNEXPECTED_PROGPOINTER = 162,
    PARSER_ARG_LABEL_INVALID_CODE = 163,

    /* Compiler error codes */
    COMPILER_FILEPRT_UNRECOGNIZED_INSTR_TYPE = 200,
    COMPILER_FILEPRT_NUMBER_OUTOFRANGE = 201,
    COMPILER_FILEPRT_DIGIT_OUTOFRANGE = 202,
    COMPILER_FILEPRT_INVALID_INSTR = 203,
    COMPILER_FILEPRT_UNRECOGNIZED_INSTR = 204,
    COMPILER_FILEPRT_CANNOT_READ_FILE_POS = 205,
    COMPILER_FILEPRT_CANNOT_PRINT_TO_FILE = 206,
    COMPILER_CANNOT_STORE_REG_MAP = 207,
    COMPILER_PRINTNUM_ERROR_PRINTING_ZERO = 208,
    COMPILER_PRINTNUM_ERROR_PRINTING_INT = 209,
    COMPILER_PRINTNUM_ERROR_PRINTING_DOT = 210,
    COMPILER_PRINTNUM_ERROR_PRINTING_DEC = 211,
    COMPILER_MERGED_IND_PRINT_ERROR = 212,
    COMPILER_IND_PRINT_ERROR = 213,
    COMPILER_INVALID_PARAMETER_TYPE = 214,
    COMPILER_INVALID_PROG_INDEX = 215,
    COMPILER_ERROR_PRINT_CODE = 216,
    COMPILER_ERROR_PRINT_DIGIT = 217,
    COMPILER_ERROR_PRINT_LABEL = 218,
    COMPILER_ERROR_PRINT_LOCATION = 219,

    /* Post-compile error codes*/
    POSTCOMP_FAILED_FILE_WRITE = 300,
    POSTCOMP_INVALID_RANGE = 301,
    POSTCOMP_INVALID_FILE = 302,
    POSTCOMP_CANNOT_SEEK = 303,
    POSTCOMP_INVALID_REGINDEX_POS = 304
} error_code;

/*
 * Structure returned by all program's functions
 * in order to store an error and where it happened
 * in the form of a number.
*/
typedef struct return_val_str {
    // The error encountered
    error_code error;

    /* In case this is an error coming from
     * an internal function being called externally
     * by another function, this cause is the error
     * code coming from the external algorithm.
     * In case this value isn't used, it's just set
     * to the enumerator value of SUCCESS.
     */
    error_code cause;

    // A number indicating the location,
    // the implementation of this is 
    // function dependent.
    int location;
} return_val;

// Helper one-liners to quickly construct a return_val struct
// Where not specified, the location will always be initialised to -1
return_val rv_from_code(error_code error);
return_val rv_nested(return_val orig, error_code reason);
return_val rv_nested_location(return_val orig, error_code reason, int location);
return_val rv_from_location(error_code error, int location);
return_val rv_from_success();
// Returns 1 on success in input, and 0 otherwise
int is_success(return_val return_code);

/*
 * This function converts the error code
 * returned by a function into a static 
 * string returned by this function.
 *
 * In case of any error encountered during this
 * conversion, NULL is returned by this function.
*/
char* error_to_str(error_code input);

#endif