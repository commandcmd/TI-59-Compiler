#include "compiler.h"

/*
 * Array used to map each incremental program pointer
 * ID with the relative register index.
 * This array is dynamically allocated.
*/
int* stored_reg_index = NULL;
size_t stored_reg_index_size = 0;

/* Helper method to add a register position association 
 * to the passed identifier.
 * This register position can then be accessed at the
 * identifier's location in stored_reg_index, as follows:
 * stored_reg_index[identifier] = reg_pos;
 *
 * In case, after a call to this method, stored_reg_index
 * is still NULL, that indicates a memory allocation problem,
 * and should be addressed.
 *
 * NOTE: This method does dynamic memory allocation!
 *       remember to free the stored_reg_index array!
 */
void add_stored_register(int reg_pos, unsigned int identifier) {
    // Allocate the memory if not allocated yet
    if(stored_reg_index == NULL) {
        // Allocate the memory
        stored_reg_index = (int*)malloc((identifier + 1) * sizeof(int));
        if(stored_reg_index == NULL) return;
        stored_reg_index_size = identifier + 1;
    } else if(identifier >= stored_reg_index_size) {
        int* new_stored_reg_index = (int*)realloc(stored_reg_index, (identifier + 1) * sizeof(int));
        if(new_stored_reg_index == NULL) return;
        stored_reg_index = new_stored_reg_index;
        stored_reg_index_size = identifier + 1;
    }

    // Assign at this element's position the current register value
    stored_reg_index[identifier] = reg_pos;
}

/*
 * This array is used to store the calls 
 * to the program pointers, which then
 * themselves point to a certain stored
 * register index.
 * Storing them in an array like this avoids
 * the program running through the entire file
 * a second time after having compiled the
 * output.
*/
typedef struct stored_progpointer_str {
    /*
     * The position in the file to seek in order
     * to find the dollar sign to replace with 
     * this program pointer.
    */
    long file_position; 
    /*
     * The register index in the stored register
     * indexes map where the register to replace
     * this program pointer with can be found.
    */
    size_t reg_index;
    /*
     * This stores the part of the number
     * to write in this file position, since
     * the location pointer is sectioned
     * in two registers, this is false for the
     * high part (the first one) and true for
     * the low one.
    */
    bool part;
} stored_progpointer;
stored_progpointer* stored_progpointer_call = NULL;
size_t stored_progpointer_size = 0; 

/*
 * Stores a new progpointer association
 * with the index of an element in the
 * stored_reg_index array.
 */
void store_progpointer_call(long file_position, size_t reg_index, bool part) {
    // Create the object to store
    stored_progpointer new_store = {
        .file_position = file_position,
        .reg_index = reg_index,
        .part = part
    };

    // If not initialised, initialise the array
    if(stored_progpointer_call == NULL) {
        // Initialise the memory allocation
        stored_progpointer_call = (stored_progpointer*)malloc(sizeof(stored_progpointer));
        // Check for any errors
        if(stored_progpointer_call == NULL) return;
        stored_progpointer_size = 1;
    } else {
        // Reallocate the array and increase its size
        stored_progpointer* new_stored_progpointer_call = 
            (stored_progpointer*)realloc(stored_progpointer_call, (stored_progpointer_size + 1) * sizeof(stored_progpointer));
        if(new_stored_progpointer_call == NULL) return;
        stored_progpointer_call = new_stored_progpointer_call;
        stored_progpointer_size++;
    }

    // Assign the given element
    stored_progpointer_call[stored_progpointer_size - 1] = new_store;
}

/* Frees the memory allocations for register indexes 
 * and stored program pointers. */
void free_compiler_mem() {
    if(stored_reg_index != NULL)
        free(stored_reg_index);
    if(stored_progpointer_call != NULL)
        free(stored_progpointer_call);
    stored_progpointer_size = 0;
    stored_reg_index_size = 0;
}

/*
 * Given an instruction to type on the calculator,
 * this returns a string which contains the user
 * readable keystroke to type on the calculator
 * to type that instruction in LRN mode.
*/
const char* compile_keystroke_str(raw_instruction instr) {
    // First of all, get the instruction type used
    switch(instr.instr_type) {
        case instyp_common_instr: {
            switch(instr.instr.common_instr) {
                case instr_CLR: return "CLR";
                case instr_NatLog: return "ln x";
                case instr_CE: return "CE";
                case instr_LOG: return "-2nd-  log";
                case instr_CP: return "-2nd-  CP";
                case instr_TAN: return "-2nd-  tan";
                case instr_EXCXT: return "x<>t";
                case instr_SQR: return "x\u00B2";
                case instr_SQRT: return "\u221Ax";
                case instr_RECIP: return "1/x";
                case instr_PGM: return "-2nd-  Pgm";
                case instr_PTOR: return "-2nd-  P\u2192R";
                case instr_SIN: return "-2nd-  sin";
                case instr_COS: return "-2nd-  cos";
                case instr_STO: return "STO";
                case instr_RCL: return "RCL";
                case instr_SUM: return "SUM";
                case instr_POW: return "y\u02E3";
                case instr_CMs: return "-2nd-  CMs";
                case instr_EXC: return "-2nd-  Exc";
                case instr_PROD: return "-2nd-  Prd";
                case instr_ABS: return "-2nd-  |x|";
                case instr_EE: return "EE";
                case instr_POPEN: return "(";
                case instr_PCLOSE: return ")";
                case instr_DIV: return "\u00F7";
                case instr_ENG: return "-2nd-  Eng";
                case instr_FIX: return "-2nd-  Fix";
                case instr_INTG: return "-2nd-  Int";
                case instr_DEG: return "-2nd-  Deg";
                case instr_GTO: return "GTO";
                case instr_MUL: return "\u00D7";
                case instr_PAUSE: return "-2nd-  Pause";
                case instr_EQ: return "-2nd-  x=t";
                case instr_NOP: return "-2nd-  Nop";
                case instr_OP: return "-2nd-  Op";
                case instr_RAD: return "-2nd-  Rad";
                case instr_SBR: return "SBR";
                case instr_SUB: return "-";
                case instr_LBL: return "-2nd-  Lbl";
                case instr_GE: return "-2nd-  x\u2265t";
                case instr_STATSUM: return "-2nd-  \u2211+";
                case instr_MEAN: return "-2nd-  x\u0304";
                case instr_GRAD: return "-2nd-  Grad";
                case instr_RST: return "RST";
                case instr_ADD: return "+";
                case instr_STFLG: return "St flg";
                case instr_IFFLG: return "If flg";
                case instr_DMS: return "D.MS";
                case instr_PI: return "\u03C0";
                case instr_LIST: return "-2nd-  List";
                case instr_STOP: return "R/S";
                case instr_NEG: return "+/-";
                case instr_RESULT: return "=";
                case instr_WRITE: return "-2nd-  Write";
                case instr_DSZ: return "-2nd-  Dsz";
                case instr_ADV: return "-2nd-  Adv";
                case instr_PRT: return "-2nd-  Prt";
                // No valid instruction found
                default: return NULL;
            }
        }

        case instyp_ucommon_label: {
            switch(instr.instr.uncommon_label) {
                case ulabel_INV: return "INV";
                case ulabel_DEC: return ".";
                // No valid instruction found
                default: return NULL;
            }
        }

        case instyp_user_key: {
            switch(instr.instr.user_key) {
                case A_lbl: return "A";
                case B_lbl: return "B";
                case C_lbl: return "C";
                case D_lbl: return "D";
                case E_lbl: return "E";
                case A_first: return "A'";
                case B_first: return "B'";
                case C_first: return "C'";
                case D_first: return "D'";
                case E_first: return "E'";
                // No valid instruction found
                default: return NULL;
            }
        }

        case instyp_ind_instr: {
            switch(instr.instr.ind_instr) {
                case instr_IND: return "-2nd-  Ind";
                case instr_PGM_ind: return "-2nd-  Pgm  -2nd-  Ind";
                case instr_EXC_ind: return "-2nd-  Exc  -2nd-  Ind";
                case instr_PRD_ind: return "-2nd-  Prd  -2nd-  Ind";
                case instr_STO_ind: return "STO  -2nd-  Ind";
                case instr_RCL_ind: return "RCL  -2nd-  Ind";
                case instr_SUM_ind: return "SUM  -2nd-  Ind";
                case instr_GTO_ind: return "GTO  -2nd-  Ind";
                case instr_OP_ind: return "-2nd-  Op  -2nd-  Ind";
                // No valid instruction found
                default: return NULL;
            }
        }

        case instyp_number: {
            // If this is a number instruction
            if(instr.instr.number < 0) {
                // Check for invalid number instruction
                if(instr.instr.number < -10) return NULL;
                if(instr.instr.number == -10) return "0";
                else {
                    // Static container for the output
                    static char number_str[] = "1";
                    // Convert the one digit negative decimal to char
                    number_str[0] = '0' - instr.instr.number;
                    return number_str;
                }
            } else { // If this is a generic code
                char code = instr.instr.number;
                // Check for invalid code
                if(code < 0 || code > 99) return NULL;
                // Static container for the output
                static char code_str[] = "[00]";
                // Convert the two digit positive code into string
                code_str[2] = '0' + (code % 10);
                code_str[1] = '0' + ((int)(code / 10) % 10);
                return code_str;
            }
        }

        case instyp_digit: { // If this is a generic digit
            char code = instr.instr.number;
            // Check for a number that's not a digit
            if(code < 0 || code > 9) return NULL;
            // Static container for the output
            static char code_str[] = "[0]";
            // Convert the digit to a char
            code_str[1] = '0' + code;
            return code_str;
        }

        // If this is a program index
        case instyp_progindex: {
            /* Static container for the output
             * This is used to allocate some space
             * which will then be overwritten with
             * the actual code string in the post
             * compilation process.
             */
            static char code_str[] = "$$$ ";
            return code_str;
        }

        default: return NULL; // Unrecognized instyp
    }
}

/*
 * Given an instruction, this function writes it to file,
 * at the register position provided.
 * It includes the register position, the raw opcode and 
 * the keystroke to press for that line.
 */
return_val print_instr_to_file(FILE* out, int reg_pos, raw_instruction instr) {
    // Save the number to print, which is the data to type at the current register
    int reg_data;
    
    // First of all, get the instruction type used
    switch(instr.instr_type) {
        case instyp_common_instr: {
            input_instructions instruction = instr.instr.common_instr;
            reg_data = (int)instruction;
            if(reg_data < 10) return rv_from_code(COMPILER_FILEPRT_INVALID_INSTR); // Invalid instruction passed
            break;
        }

        case instyp_ucommon_label: {
            uncommon_labels instruction = instr.instr.uncommon_label;
            reg_data = (int)instruction;
            break;
        }

        case instyp_user_key: {
            user_defined_keys instruction = instr.instr.user_key;
            reg_data = (int)instruction;
            break;
        }

        case instyp_ind_instr: {
            ind_instructions instruction = instr.instr.ind_instr;
            reg_data = (int)instruction;
            break;
        }

        case instyp_digit:
        case instyp_number: 
        {
            char data = instr.instr.number;
            reg_data = (int)data;
            if(reg_data < 0) reg_data *= -1;
            // If this is a digit and a bigger number was inserted, error out
            if(instr.instr_type == instyp_digit && reg_data > 9) return rv_from_code(COMPILER_FILEPRT_DIGIT_OUTOFRANGE);
            if(reg_data > 99) return rv_from_code(COMPILER_FILEPRT_NUMBER_OUTOFRANGE); // Invalid number saved
            break;
        }

        case instyp_progindex: {
            reg_data = instr.instr.number >= 0 ? instr.instr.number : -instr.instr.number - 1;
            break;
        }

        default: return rv_from_code(COMPILER_FILEPRT_UNRECOGNIZED_INSTR_TYPE); // Unrecognized instyp
    }
    // Print the register data to file
    const char* keystrokes = compile_keystroke_str(instr);
    if(keystrokes == NULL) return rv_from_code(COMPILER_FILEPRT_UNRECOGNIZED_INSTR);
    int written_bytes = fprintf(out, "%03u\t", reg_pos);
    // If this is a program pointer index
    if(instr.instr_type == instyp_progindex) {
        long current_file_pos = ftell(out);
        if(current_file_pos == -1) return rv_from_code(COMPILER_FILEPRT_CANNOT_READ_FILE_POS);
        store_progpointer_call(current_file_pos, reg_data, instr.instr.number >= 0);
    }
    if(written_bytes >= 0) written_bytes += fprintf(out, "%02u\t%s\n", reg_data, keystrokes);
    // Check for file printf errors
    if(written_bytes <= 0) return rv_from_code(COMPILER_FILEPRT_CANNOT_PRINT_TO_FILE);
    return rv_from_success();
}


/*===== ONE-LINE HELPERS FOR RAW INSTRUCTION OBJECTS =====*/
raw_instruction ri_from_common(input_instructions input) {
    raw_instruction output = {.instr_type = instyp_common_instr};
    output.instr.common_instr = input;
    return output;
}
raw_instruction ri_from_ucommon(uncommon_labels input) {
    raw_instruction output = {.instr_type = instyp_ucommon_label};
    output.instr.uncommon_label = input;
    return output;
}
raw_instruction ri_from_ukey(user_defined_keys input) {
    raw_instruction output = {.instr_type = instyp_user_key};
    output.instr.user_key = input;
    return output;
}
raw_instruction ri_from_indirect(input_instructions input) {
    ind_instructions input_ind;
    // Bundled switch case to convert a normal
    // instruction to the indirect variant
    switch(input) {
        case instr_PGM:  input_ind = instr_PGM_ind; break;
        case instr_EXC:  input_ind = instr_EXC_ind; break;
        case instr_PROD: input_ind = instr_PRD_ind; break;
        case instr_STO:  input_ind = instr_STO_ind; break;
        case instr_RCL:  input_ind = instr_RCL_ind; break;
        case instr_SUM:  input_ind = instr_SUM_ind; break;
        case instr_GTO:  input_ind = instr_GTO_ind; break;
        case instr_OP:   input_ind = instr_OP_ind;  break;
        // Create an invalid instruction to make the print 
        // instr to file function return an error.
        default: {
            raw_instruction error = {.instr_type = instyp_invalid};
            return error;
        }
    }
    // Now create the output instruction
    raw_instruction output = {.instr_type = instyp_ind_instr};
    output.instr.ind_instr = input_ind;
    return output;
}
/*
 * Custom function to output the indirect instruction
 * instr_IND cannot be passed to ri_from_indirect because
 * that converts a regular instruction to its indirect
 * counterpart, but this is the exception for the IND
 * instruction which mustn't be converted and thus doesn't
 * come from the input_instructions enumerator.
 */
raw_instruction raw_ind_instr() {
    raw_instruction output = {.instr_type = instyp_ind_instr};
    output.instr.ind_instr = instr_IND;
    return output;
}
raw_instruction ri_from_code(unsigned char code) {
    raw_instruction output = {.instr_type = instyp_number};
    output.instr.number = code;
    return output;
}
raw_instruction ri_from_digit(unsigned char single_digit) {
    raw_instruction output = {.instr_type = instyp_digit};
    output.instr.number = single_digit;
    return output;
}
raw_instruction ri_from_number(unsigned char number) {
    raw_instruction output = {.instr_type = instyp_number};
    if(number == 0) output.instr.number = -10;
    else output.instr.number = (char)number * -1;
    return output;
}
raw_instruction ri_from_label(label_code label) {
    // Check if the label passed is invalid
    if(label.label_type == 0 && label.label.common_label < 10)
        return (raw_instruction){.instr_type = instyp_invalid};

    if(label.label_type > 0) {
        return ri_from_ucommon(label.label.uncommon_label);
    } else if(label.label_type < 0) {
        return ri_from_ukey(label.label.user_defined_key);
    } else {
        return ri_from_common(label.label.common_label);
    }
}
raw_instruction ri_from_program_index(int reg_index) {
    raw_instruction output = {.instr_type = instyp_progindex};
    output.instr.number = reg_index;
    return output;
}

/*
 * Given a number, this function prints
 * the instructions tied to each digit
 * to the file, but this is only done
 * for the decimal part of the number
 * itself. The function will stop printing
 * once size decimal digits have been
 * counted.
 * This function returns 1 on success, 0 otherwise.
*/
return_val print_dec_to_file(FILE* out, int* reg_pos, double number, unsigned int size) {
    // Get the decimal part of the number
    double number_dec = number - (long)number;
    // Keep going until digits run out
    do {
        number_dec *= 10.0;
        // Round current digit if this is the last instruction
        char current_digit;
        if(size <= 1) current_digit = (char)(number_dec + 0.5);
        else current_digit = (char)(number_dec);

        // Print this digit to file
        return_val result = print_instr_to_file(out, (*reg_pos)++, ri_from_number(current_digit));
        if(!is_success(result)) return result;

        // Remove the last digit and decrement the size
        number_dec -= (double)current_digit;
        size--;
    } while(size > 0);

    // Success
    return rv_from_success();
}

/*
 * Given a number, even with a decimal point, this function
 * prints is to the output file and advances the register
 * position index passed.
 * To save a number in the calculator, this function must 
 * separate each digit into a distinct instruction, so
 * this function does just that.
 */
return_val print_number_instr(FILE* out, int* reg_pos, double number, unsigned int size) {
    // If the number is zero, print just the zero instruction and exit
    if(number == 0) {
        // Edge case: log10 of zero is -inf
        return_val result = print_instr_to_file(out, (*reg_pos)++, ri_from_number(0));
        if(!is_success(result)) return rv_nested(result, COMPILER_PRINTNUM_ERROR_PRINTING_ZERO);
    }
    // If the number is negative, invert
    if(number < 0) number *= -1;

    // Strip out the decimal part from the number
    long number_int = (long)number;
    // Get the amount of digits of the number
    int number_digits = (int)(log10(number_int) + 1);
    // Now convert the number to decimal and print the integer part
    return_val result = print_dec_to_file(out, reg_pos, number_int / pow(10, number_digits), number_digits);
    if(!is_success(result)) return rv_nested(result, COMPILER_PRINTNUM_ERROR_PRINTING_INT);
    // Decrement the size
    size -= number_digits;

    // If no more digits are remaining
    if(size <= 1) return rv_from_success();
    
    // Decrement it once for the decimal point
    size--;
    result = print_instr_to_file(out, (*reg_pos)++, ri_from_ucommon(ulabel_DEC));
    if(!is_success(result)) return rv_nested(result, COMPILER_PRINTNUM_ERROR_PRINTING_DOT);
    // Keep decrementing until size runs out
    result = print_dec_to_file(out, reg_pos, number, size);
    if(!is_success(result)) return rv_nested(result, COMPILER_PRINTNUM_ERROR_PRINTING_DEC);

    return rv_from_success();
}

return_val convert_line_to_instr(input_line line, FILE* out, int* reg_pos) {
    // Saves the status of file call operations
    return_val status = rv_from_success();
    // In case this is one of those merged indirect instructions
    bool merged_ind = false;

    // Check for an inverse instruction and print the inverse
    // In case of the return alias, don't print an inversion already
    if(line.inverse && !line.instruction == ret_alias) 
        status = print_instr_to_file(out, (*reg_pos)++, ri_from_ucommon(ulabel_INV));

    // Check for special instructions
    switch(line.instruction) {
        case instr_number: {
            status = print_number_instr(out, reg_pos, line.data.number_instr.number, line.data.number_instr.size);
            if(!is_success(status)) return status;
            break;
        }

        case prog_pointer:
        {
            // Create a new association for this register position
            add_stored_register(*reg_pos, line.data.prog_pointer);
            // Failed memory allocation
            if(stored_reg_index == NULL) return rv_from_code(COMPILER_CANNOT_STORE_REG_MAP);
            return rv_from_success();
        }

        // In case of a program pointer or a variable just return invalid
        case reg_variable:
            return rv_from_success();

        case ret_alias: {
            // Handle the return alias to INV SBR, using SBR if RETURN has been itself inverted
            if(!line.inverse) status = print_instr_to_file(out, (*reg_pos)++, ri_from_ucommon(ulabel_INV));
            if(!is_success(status)) return status;
            status = print_instr_to_file(out, (*reg_pos)++, ri_from_common(instr_SBR));
            if(!is_success(status)) return status;
            return rv_from_success();
        }

        // Cases for indirect instructions
        case instr_PGM:
        case instr_EXC:
        case instr_PROD:
        case instr_STO:
        case instr_RCL:
        case instr_SUM:
        case instr_GTO:
        case instr_OP:
        {
            // If this instruction has one argument and it's indirect
            if(line.parameters_amount > 0 && line.parameters[0].indirect) {
                // This is a merged indirect instruction
                merged_ind = true;
                status = print_instr_to_file(out, (*reg_pos)++, ri_from_indirect(line.instruction));
            // Otherwise just print the instruction normally
            } else status = print_instr_to_file(out, (*reg_pos)++, ri_from_common(line.instruction));
            // Exit from the condition
            break;
        }

        default: status = print_instr_to_file(out, (*reg_pos)++, ri_from_common(line.instruction));
    }

    if(!is_success(status)) return status;

    // Iterate through all parameters
    for(int param = 0; param < line.parameters_amount; param++) {
        // Get the current parameter
        input_parameter current_parameter = line.parameters[param];

        // If this instruction has merged indirect
        if(merged_ind && param == 0) {
            // Just print the code of the indirect register and continue
            return_val result = print_instr_to_file(out, (*reg_pos)++, ri_from_code(current_parameter.data.value));
            if(!is_success(result)) return rv_nested_location(result, COMPILER_MERGED_IND_PRINT_ERROR, param);
            continue;
        } else if(current_parameter.indirect) { // If it isn't merged but still indirect
            // Print the IND code
            return_val result = print_instr_to_file(out, (*reg_pos)++, raw_ind_instr());
            // Print the value tied to this code
            if(is_success(result)) result = print_instr_to_file(out, (*reg_pos)++, ri_from_code(current_parameter.data.value));
            // In case of any error
            if(!is_success(result)) return rv_nested_location(result, COMPILER_IND_PRINT_ERROR, param);
            continue;
        }

        switch(current_parameter.type) {
            case program_code:
            case register_index:
            case operation:
                status = print_instr_to_file(out, (*reg_pos)++, ri_from_code(current_parameter.data.value));
                if(!is_success(status)) return rv_nested_location(status, COMPILER_ERROR_PRINT_CODE, param);

            case digit:
            case decimal_amount:
                status = print_instr_to_file(out, (*reg_pos)++, ri_from_digit(current_parameter.data.value));
                if(!is_success(status)) return rv_nested_location(status, COMPILER_ERROR_PRINT_DIGIT, param);

            case label:
                status = print_instr_to_file(out, (*reg_pos)++, ri_from_label(current_parameter.data.label));
                if(!is_success(status)) return rv_nested_location(status, COMPILER_ERROR_PRINT_LABEL, param);

            case location:
                // Check if it's a location or a progpointer
                if(current_parameter.is_value) { // Program pointer
                    // Invalid program index
                    if(current_parameter.data.value >= stored_reg_index_size && stored_reg_index == NULL) return rv_from_location(COMPILER_INVALID_PROG_INDEX, param);
                    // Print the reg index instruction to file
                    else {
                        status = print_instr_to_file(out, (*reg_pos)++, ri_from_program_index(current_parameter.data.value));
                        status = is_success(status) ? print_instr_to_file(out, (*reg_pos)++, ri_from_program_index(-current_parameter.data.value - 1)) : status;
                    }
                } else status = print_instr_to_file(out, (*reg_pos)++, ri_from_label(current_parameter.data.label));
                if(!is_success(status)) return rv_nested_location(status, COMPILER_ERROR_PRINT_LOCATION, param);

            default: return rv_from_location(COMPILER_INVALID_PARAMETER_TYPE, param); // Invalid argument
        }
    }

    // Success
    return rv_from_success();
}

/*
 * This function performs a replace on the 
 * register data and keystroke string assuming
 * the file pointer is in the correct position,
 * This prints the given value formatted into
 * register data over the previous register data,
 * and then prints it as a keystroke string in the
 * following format:
 * [value]
 * For numbers 0-9 the register data will just
 * contain a number with a starting zero and
 * the keystroke string will only be one digit,
 * for values between 10 and 99, keystroke string
 * will be two digits and register data will use
 * both digits.
 * The digit amount isn't actually decided by the
 * number, rather it's decided by a separate boolean
 * value passed which if true forces a single digit
 * to be used. In which case only the last digit of
 * the value is considered.
 * If the value passed is negative or greater
 * than 99, then -1 is returned.
 * This function returns 0 on success.
*/
return_val execute_replace(FILE* out, int value, bool single_digit) {
    // Check the input value
    if(value < 0 || single_digit && value > 9 || value > 99) return rv_from_code(POSTCOMP_INVALID_RANGE);
    if(out == NULL) return rv_from_code(POSTCOMP_INVALID_FILE);

    /* 
     * Static value string conversion since we know 
     * the possible amounts of digits can either be
     * 1 or 2 and no negative numbers are possible.
     */
    static char value_str[] = "00";
    // Amount of digits used in value_str (either 1 or 2)
    int value_str_size;

    if(single_digit) {
        // Single digit value
        value_str_size = 1;
        // Fill the single digit
        value_str[0] = '0' + value % 10;
    } else {
        // Two digits
        value_str_size = 2;
        // Fill the two digits
        value_str[1] = '0' + (value % 10);
        value_str[0] = '0' + (int)(value / 10);
    }

    // Print out to file
    if(fprintf(out, "%02d\t[%.*s]", value, value_str_size, value_str) < 0)
        return rv_from_code(POSTCOMP_FAILED_FILE_WRITE);

    // Success
    return rv_from_success();
}

return_val final_compile(FILE* file) {
    // Iterate through every single progpointer call
    for(int current_call = 0; current_call < stored_progpointer_size; current_call++) {
        stored_progpointer current_progpointer = stored_progpointer_call[current_call];

        // Seek the file to the current call's position
        if(fseek(file, current_progpointer.file_position, SEEK_SET) != 0)
            return rv_from_code(POSTCOMP_CANNOT_SEEK);

        // Check if the stored reg index isn't out of bounds
        if(current_progpointer.reg_index < 0 
            || current_progpointer.reg_index >= stored_reg_index_size
        ) return rv_from_code(POSTCOMP_INVALID_REGINDEX_POS);
        // Retrieve the stored register
        int stored_register = stored_reg_index[current_progpointer.reg_index];

        // Now check the current state
        return_val replace_status;
        if(current_progpointer.part) {
            // Read only the first digit of the three digit number
            // Which is the number divided by 100
            replace_status = execute_replace(file, (int)(stored_register / 100), true);
            if(!is_success(replace_status)) {
                replace_status.location = current_call;
                return replace_status;
            }
        } else {
            // Read the last two digits of the number
            replace_status = execute_replace(file, stored_register % 100, false);
            if(!is_success(replace_status)) {
                replace_status.location = current_call;
                return replace_status;
            }
        }
    }

    return rv_from_success();
}