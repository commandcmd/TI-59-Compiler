#ifndef TI_ERROR_H
#define TI_ERROR_H

/*
 * Structure returned by all program's functions
 * in order to store an error and where it happened
 * in the form of a number.
*/
typedef struct return_val_str {
    // The error encountered
    error_code error;

    /* If this error was encountered 
     * due to another error, the nested
     * error code of the more internal
     * algorithm is stored here and can 
     * be separately printed.
     */
    error_code nested_error;

    // A number indicating the location,
    // the implementation of this is 
    // function dependent.
    int location;
} return_val;

// Helper one-liners to quickly construct a return_val struct
// rv_from_code will always set the location to -1
inline return_val rv_from_code(error_code error) {
    return_val output = {.error = error, .location = -1};
    return output;
}
inline return_val rv_from_code_nested(error_code error, error_code nested) {
    return_val output = {.error = error, .nested_error = nested, .location = -1};
}
inline return_val rv_from_location(error_code error, int location) {
    return_val output = {.error = error, .location = location};
    return output;
}
inline return_val rv_from_success() {
    return_val output = {.error = SUCCESS, .location = 0};
    return output;
}

inline int is_success(return_val return_code) {
    if(return_code.error == SUCCESS) return 1;
    return 0;
}

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
    COMPILER_FILEPRT_UNRECOGNIZED_INSTR = 203,
    COMPILER_FILEPRT_CANNOT_READ_FILE_POS = 204,
    COMPILER_FILEPRT_CANNOT_PRINT_TO_FILE = 205

} error_code;

/*
 * This function converts the error code
 * returned by a function into a static 
 * string returned by this function.
 *
 * In case of any error encountered during this
 * conversion, NULL is returned by this function.
*/
inline char* error_to_str(error_code input) {
    switch(input) {
        case SUCCESS: return "No Error Encountered";

        case PROGRAM_POINTER_ALLOC_ERROR: return "Cannot allocate a new program pointer";
        case VARIABLE_REGISTER_ALLOC_ERROR: return "Cannot allocate a new variable map";

        /* Parser error codes */
        case PARSER_NO_TOKENS: return "No tokens found";
        // Instruction parser
        case PARSER_INSTR_NO_TOKEN: return "[Parse Instruction] No Instruction token found";
        case PARSER_INSTR_NO_NUMBER: return "[Parse Instruction] Number Instruction - No valid number found";
        case PARSER_INSTR_NO_INV: return "[Parse Instruction] No second token found after INV";
        case PARSER_INSTR_INVALID_INV: return "[Parse Instruction] Invalid token found after INV";
        case PARSER_INSTR_VAR_NO_TOKEN: return "[Parse Instruction] No second token found after '$'";
        case PARSER_INSTR_VAR_NO_EQUAL: return "[Parse Instruction] Expected '=' token for variable map";
        case PARSER_INSTR_VAR_NO_REG: return "[Parse Instruction] No valid register token found";
        case PARSER_INSTR_VAR_INVALID_REG: return "[Parse Instruction] No valid register number found";
        case PARSER_INSTR_VAR_REG_OUTOFRANGE: return "[Parse Instruction] Register number provided is out of range, must be between 0-99";
        case PARSER_INSTR_INVALID_TOKEN: return "[Parse Instruction] Invalid token found";
        case PARSER_INSTR_TOO_LONG_TOKEN: return "[Parse Instruction] Instruction token too long";
        case PARSER_INSTR_INVALID_INSTR: return "[Parse Instruction] Couldn't parse the instruction token";
        case PARSER_ARG_ALLOC_ERROR: return "[Argument Allocation] Cannot create an argument allocation for the instruction";
        case PARSER_ARG_NO_TOKEN_FOUND: return "[Argument Parser] No token found for parameter";
        case PARSER_ARG_IND_NO_VALUE: return "[Argument Parser] No second token found after IND";
        case PARSER_ARG_IND_NO_NUMBER: return "[Argument Parser] No valid register found after IND";
        case PARSER_ARG_IND_OUTOFRANGE: return "[Argument Parser] Register specified after IND is out of range, must be between 0-99";
        case PARSER_ARG_IND_NO_VARS: return "[Argument Parser] No declared variable registers available";
        case PARSER_ARG_IND_UNDECLARED_VAR: return "[Argument Parser] Specified variable register couldn't be found";
        case PARSER_ARG_VARIABLE_OUTOFRANGE: return "[Argument Parser] Variable contains a multiple digit number, but parameter only accepts digits 0-9";
        case PARSER_ARG_INPUT_OUTOFRANGE: return "[Argument Parser] Specified number is out of range, must be between 0-99";
        case PARSER_ARG_DIGIT_OUTOFRANGE: return "[Argument Parser] Value contains a multiple digit number, but parameter only accepts digits 0-9";
        case PARSER_ARG_NO_NUMBER: return "[Argument Parser] Expected number token, none found";
        case PARSER_ARG_LABEL_INVALID_TOKEN: return "[Argument Parser] Invalid token found on label";
        case PARSER_ARG_LABEL_UNEXPECTED_PROGPOINTER: return "[Argument Parser] Expected label but found program pointer instead";
        case PARSER_ARG_LABEL_INVALID_CODE: return "[Argument Parser] Invalid label type passed";


        default: return NULL;
    }
}

#endif