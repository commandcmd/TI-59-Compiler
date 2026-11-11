#include "ti-error.h"


// Helper one-liners to quickly construct a return_val struct
// rv_from_code will always set the location to -1
return_val rv_from_code(error_code error) {
    return_val output = {.error = error, .location = -1, .cause = SUCCESS};
    return output;
}
return_val rv_nested(return_val orig, error_code reason) {
    orig.cause = reason;
    return orig;
}
return_val rv_nested_location(return_val orig, error_code reason, int location) {
    orig.location = location;
    return rv_nested(orig, reason);
}
return_val rv_from_location(error_code error, int location) {
    return_val output = {.error = error, .location = location, .cause = SUCCESS};
    return output;
}
return_val rv_from_success() {
    return_val output = {.error = SUCCESS, .location = -1, .cause = SUCCESS};
    return output;
}

int is_success(return_val return_code) {
    if(return_code.error == SUCCESS) return 1;
    return 0;
}

/*
 * This function converts the error code
 * returned by a function into a static 
 * string returned by this function.
 *
 * In case of any error encountered during this
 * conversion, NULL is returned by this function.
*/
char* error_to_str(error_code input) {
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

        /* Compiler error codes */
        case COMPILER_FILEPRT_UNRECOGNIZED_INSTR_TYPE: return "[Output File Printer] Couldn't recognize the instruction type to print";
        case COMPILER_FILEPRT_NUMBER_OUTOFRANGE: return "[Output File Printer] Provided out of range number, must be between 0-99";
        case COMPILER_FILEPRT_DIGIT_OUTOFRANGE: return "[Output File Printer] Provided out of range digit, must be between 0-9";
        case COMPILER_FILEPRT_UNRECOGNIZED_INSTR: return "[Output File Printer] Unrecognized output instruction";
        case COMPILER_FILEPRT_CANNOT_READ_FILE_POS: return "[Output File Printer] Unable to read current file position";
        case COMPILER_FILEPRT_CANNOT_PRINT_TO_FILE: return "[Output File Printer] Couldn't print to output file";
        case COMPILER_CANNOT_STORE_REG_MAP: return "[Compiler] Cannot store a new register map in memory";
        case COMPILER_PRINTNUM_ERROR_PRINTING_ZERO: return "[Compiler Number Print] Cannot print number '0' to file";
        case COMPILER_PRINTNUM_ERROR_PRINTING_INT: return "[Compiler Number Print] Cannot print integer number part to file";
        case COMPILER_PRINTNUM_ERROR_PRINTING_DOT: return "[Compiler Number Print] Cannot print decimal dot to file";
        case COMPILER_PRINTNUM_ERROR_PRINTING_DEC: return "[Compiler Number Print] Cannot print decimal number part to file";
        case COMPILER_MERGED_IND_PRINT_ERROR: return "[Compiler] Error printing the merged indirect instruction";
        case COMPILER_IND_PRINT_ERROR: return "[Compiler] Error printing the indirect instruction";
        case COMPILER_INVALID_PARAMETER_TYPE: return "[Compiler] Invalid parameter type found in memory";
        case COMPILER_INVALID_PROG_INDEX: return "[Compiler] Invalid program index found in memory";
        case COMPILER_ERROR_PRINT_CODE: return "[Compiler] Error while printing 2-digit code instruction";
        case COMPILER_ERROR_PRINT_DIGIT: return "[Compiler] Error while printing single-digit code instruction";
        case COMPILER_ERROR_PRINT_LABEL: return "[Compiler] Error while printing label";
        case COMPILER_ERROR_PRINT_LOCATION: return "[Compiler] Error while printing program pointer location";

        /* Post-compiler error codes */
        case POSTCOMP_FAILED_FILE_WRITE: return "[Post-Compiler] Couldn't write to output file";
        case POSTCOMP_INVALID_RANGE: return "[Post-Compiler] Invalid register provided - out of range";
        case POSTCOMP_INVALID_FILE: return "[Post-Compiler] Invalid output file provided by compiler";
        case POSTCOMP_CANNOT_SEEK: return "[Post-Compiler] Cannot seek into the provided output file";
        case POSTCOMP_INVALID_REGINDEX_POS: return "[Post-Compiler] Invalid register index position in memory";

        default: return "[Unspecified error code]";
    }
}