#include "parser.h"
#include "global-helpers.h"

#define COMMENT_IDENTIFIER ';'

#define TOKEN_ENCLOSER '\t'
#define WHITESPACE_PLACEHOLDER '\0'

#define NUMBER_TOKEN 0xe739ce739ce739c
#define INVALID_TOKEN      0xEF7BDEF7BDEF7BD
#define PROG_POINTER_TOKEN 0xF7BDEF7BDEF7BDE
#define VARIABLE_TOKEN     0xD6B5AD6B5AD6B5A

#define IND_MODIFIER "IND"

struct parser_state_str {
    map_t progpointer_map;
    map_t variable_map;
} parser_state = {
    .progpointer_map = MAP_INIT,
    .variable_map = MAP_INIT
};

/*
 * Enumerator containing all the pre-coded
 * available packed tokens to check against
 * using the switch-case.
 */
enum instruction_tokens {
    token_RETURN = 0x2249D22D,
    token_CLR = 0x971,
    token_NatLog = 0x1A09ADC6,
    token_CE = 0x44,
    token_LOG = 0x2DC6,
    token_CP = 0x4F,
    token_TAN = 0x4C0D,
    token_EXCXT = 0x4B8AF3,
    token_SQR = 0x4A11,
    token_SQRT = 0x94233,
    token_RECIP = 0x112090F,
    token_PGM = 0x3CCC,
    token_PTOR = 0x7CDD1,
    token_SIN = 0x490D,
    token_COS = 0x9D2,
    token_STO = 0x4A6E,
    token_RCL = 0x444B,
    token_SUM = 0x4A8C,
    token_POW = 0x3DD6,
    token_CMs = 0x992,
    token_EXC = 0x12E2,
    token_PROD = 0x7C5C3,
    token_ABS = 0x32,
    token_EE = 0x84,
    token_POPEN = 0xF73C8D,
    token_PCLOSE = 0x1E25BA44,
    token_DIV = 0xD15,
    token_ENG = 0x11A6,
    token_FIX = 0x1517,
    token_INTG = 0x43666,
    token_DEG = 0xC86,
    token_GTO = 0x1A6E,
    token_MUL = 0x328B,
    token_PAUSE = 0xF05244,
    token_EQ = 0x90,
    token_NOP = 0x35CF,
    token_OP = 0x1CF,
    token_RAD = 0x4403,
    token_SBR = 0x4831,
    token_SUB = 0x4A81,
    token_LBL = 0x2C2B,
    token_GE = 0xC4,
    token_STATSUM = 0x4A609CA8C,
    token_MEAN = 0x6100D,
    token_GRAD = 0x34403,
    token_RST = 0x4653,
    token_ADD = 0x63,
    token_STFLG = 0x1299566,
    token_IFFLG = 0x829566,
    token_DMS = 0xD92,
    token_PI = 0x1E8,
    token_LIST = 0x5A253,
    token_STOP = 0x94DCF,
    token_NEG = 0x3486,
    token_RESULT = 0x22495173,
    token_WRITE = 0x168A264,
    token_DSZ = 0xE59,
    token_ADV = 0x75,
    token_PRT = 0x3E33,

    token_INV = 0x21B5,
    token_DEC = 0xC82,

    token_A = 0x0,
    token_B = 0x1,
    token_C = 0x2,
    token_D = 0x3,
    token_E = 0x4,
    token_Ap = 0xF,
    token_Bp = 0x2F,
    token_Cp = 0x4F,
    token_Dp = 0x6F,
    token_Ep = 0x8F
};

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
char* f_getline(FILE* in, size_t* line_length) {
    /* Seek the length of the line to know 
     * the amount of memory to allocate for 
     * it in the heap.
     */
    char last_read_char = '\0';
    // This flag is set to true when a comment start is found
    bool comment_start_found = false;
    // This contains the seek position for the line start
    long line_start_position = ftell(in);
    if(line_start_position == -1) return NULL;
    // Count amount of bytes until EOF or newline
    for(*line_length = 0; last_read_char != '\n'; (*line_length)++) {
        int get_result = fgetc(in);
        if(get_result < 0 || get_result == EOF) break;
        last_read_char = (char)get_result;
        // If a comment is found, stop allocating
        if(last_read_char == COMMENT_IDENTIFIER) {
            comment_start_found = true;
            break;
        }
    }
    // If line length is zero, return NULL
    if(*line_length == 0) return NULL;
    
    // Seek back in the file at the start of the line
    int seek_result = fseek(in, line_start_position, SEEK_SET);
    if(seek_result != 0) return NULL;

    // Allocate the memory for the line
    char* line_buffer = (char*)malloc(*line_length);
    // Check for failed memory allocation
    if(line_buffer == NULL) return NULL;
    // Copy the contents of the file in the buffer
    if(fgets(line_buffer, *line_length, in) == NULL)
        return NULL;

    // Keep reading until a new line is found
    int get_result;
    do {
        get_result = fgetc(in);
    } while(get_result != EOF && get_result >= 0 && (char)get_result != '\n');

    return line_buffer;
}

/*
 * Helper which returns whether a passed
 * character is considered a whitespace.
*/
bool is_whitespace(char in) {
    // Commas are also considered token separators so whitespaces
    return in <= ' ' || in == (char)127 || in == ','; 
}

/*
 * Checks if the passed character is a letter.
 */
bool is_letter(char in) {
    return (in >= 'A' && in <= 'Z') 
        || (in >= 'a' && in <= 'z');
}

/*
 * Given a character, this function 
 * converts it to uppercase, because
 * this language is case-insensitive,
 * this conversion must be done.
*/
void to_uppercase(char* ch) {
    if(*ch >= 'a' && *ch <= 'z')
        *ch = *ch - 32;
}

/*
 * Given the string in the buffer, this function
 * separates each token by enclosing it in TAB
 * ascii characters and by replacing all additional
 * whitespace characters (other spaces, tabs, etc.)
 * with NULL characters.
 * This also counts the amount of tokens (words separated
 * by whitespace characters) and returns it.
 */
unsigned int tokenize_line(char* line_buffer, size_t bufsize) {
    unsigned int token_amount = 0;
    bool found_comment = false;

    // In case the size of the string is too short, ignore
    if(bufsize < 1) return 0;

    /*
     * Handle the edge case in which a comment
     * character sits at the start of the line
    */
    if(bufsize != 0 && line_buffer[0] == COMMENT_IDENTIFIER) return 0;

    /* Uppercase manually the first and last letters
     * since they are not looped through.
     */
    if(is_letter(line_buffer[0])) to_uppercase(&line_buffer[0]);
    if(is_letter(line_buffer[bufsize - 1])) to_uppercase(&line_buffer[bufsize - 1]);

    /* Iterate through all the characters 
     * which are contained between other
     * characters.
     */
    for(int i = 1; i < bufsize - 1; i++) {
        // Once a comment is read, stop reading everything
        if(line_buffer[i] == COMMENT_IDENTIFIER) {
            found_comment = true;
            break;
        }

        if(is_whitespace(line_buffer[i])) {
            if(!is_whitespace(line_buffer[i - 1])) {
                line_buffer[i] = TOKEN_ENCLOSER;
                token_amount++;
            } else if(!is_whitespace(line_buffer[i + 1]))
                line_buffer[i] = TOKEN_ENCLOSER;
            else line_buffer[i] = WHITESPACE_PLACEHOLDER;
        } else if(is_letter(line_buffer[i])) { // Convert all text to uppercase
            to_uppercase(&line_buffer[i]);
        }
    }

    // Handle the last character of the string 
    // separately since it creates some edge cases
    if(!found_comment) {
        // Increase the token amount if the last character is a token
        if(!is_whitespace(line_buffer[bufsize - 1]))
            token_amount++;
        else if(bufsize > 1) { // If there is a character before the last
            // If the previous character was part of a token
            if(!is_whitespace(line_buffer[bufsize - 2])) {
                // Write a token encloser
                line_buffer[bufsize - 1] = TOKEN_ENCLOSER;
                // Add one to the token amount
                token_amount++;
            }
            // Otherwise write a whitespace placeholder in case it was a whitespace itself
            else line_buffer[bufsize - 1] = WHITESPACE_PLACEHOLDER;
        }
    }
    
    return token_amount;
}

/*
 * Given a token to pack, this function returns a
 * 64bit integer value containing all characters
 * that the function can fit in there through optimization.
 * Note that if the token is longer than 12 characters, the
 * function will cut it short.
 *
 * In case the token is too long, then UINT64_MAX is returned.
*/
uint64_t pack_token(const char* token, size_t bufsize) {
    uint64_t output = 0;

    if(bufsize == 0) return INVALID_TOKEN;

    // Check if the current token is a program pointer before checking the size
    if(token[0] == ':') return PROG_POINTER_TOKEN;
    // The dollar sign is the variable identifier and must be at the start
    if(token[0] == '$') return VARIABLE_TOKEN;

    // Check the length of the token passed, if too long return all f's
    if(bufsize > 12) return UINT64_MAX;

    /* 
     * Iterate through each character and pack it in 
     * the output.
     * A maximum of 12 characters can fit in the 64bit
     * space.
     */
    for(int i = 0; i < bufsize; i++) {
        // Pack the current character
        char current_char = token[i];
        if(current_char >= 'A' && current_char <= 'Z')
            current_char -= 'A'; // A-Z are the numbers 0-25
        else if(current_char >= '0' && current_char <= '9' || current_char == '.')
            return NUMBER_TOKEN; // In case of any number return the number token
        else return INVALID_TOKEN;

        // Cast the last 5 bits of the char into the output
        output = output << 5;
        output |= (current_char & 31);
    }

    return output;
}

/*
 * Given the line buffer and its size,
 * This function parses the next token after the
 * starting index, returning its index start location,
 * and writing the size to the passed pointer.
 * The index returned is relative to the string.
 * This function returns zero on success, or non-zero 
 * on no token found.
*/
int parse_token(unsigned int index_start, char* line_buffer, size_t bufsize, size_t* token_index, size_t* token_size) {
    // Find the token
    size_t token_start, token_end = bufsize - 1;
    bool found_token_start = false;
    if(index_start >= bufsize) return 20; // Start index out of bounds
    /* If the first character already isn't a whitespace
     * then it is the start of the first token.
     */
    if(!is_whitespace(line_buffer[index_start])) {
        found_token_start = true;
        token_start = index_start;
    } else { // Otherwise find the first token
        for(int i = index_start; i < bufsize; i++) {
            if(line_buffer[i] == TOKEN_ENCLOSER && i < bufsize - 1) {
                token_start = i + 1;
                found_token_start = true;
                break;
            }
        }
    }
    if(!found_token_start) return 10;
    // Find the end of the first token
    for(int i = token_start + 1; i < bufsize; i++) {
        if(line_buffer[i] == TOKEN_ENCLOSER) {
            token_end = i - 1;
            break;
        }
    }
    // Now save the token size and index in the string
    *token_size = token_end - token_start + 1;
    *token_index = token_start;
    return 0;
}

/*
 * This enumerator maps all the packed tokens to the
 * input instructions available, then returns true
 * if a map was found, false otherwise.
*/
bool parse_instruction_token(uint64_t packed_token, input_instructions* output) {
    switch(packed_token) {
        case NUMBER_TOKEN: {
            *output = instr_number;
            break;
        }
        case token_RETURN: {
            *output = ret_alias;
            break;
        }
        case token_CLR: {
            *output = instr_CLR;
            break;
        }
        case token_NatLog: {
            *output = instr_NatLog;
            break;
        }
        case token_CE: {
            *output = instr_CE;
            break;
        }
        case token_LOG: {
            *output = instr_LOG;
            break;
        }
        case token_CP: {
            *output = instr_CP;
            break;
        }
        case token_TAN: {
            *output = instr_TAN;
            break;
        }
        case token_EXCXT: {
            *output = instr_EXCXT;
            break;
        }
        case token_SQR: {
            *output = instr_SQR;
            break;
        }
        case token_SQRT: {
            *output = instr_SQRT;
            break;
        }
        case token_RECIP: {
            *output = instr_RECIP;
            break;
        }
        case token_PGM: {
            *output = instr_PGM;
            break;
        }
        case token_PTOR: {
            *output = instr_PTOR;
            break;
        }
        case token_SIN: {
            *output = instr_SIN;
            break;
        }
        case token_COS: {
            *output = instr_COS;
            break;
        }
        case token_STO: {
            *output = instr_STO;
            break;
        }
        case token_RCL: {
            *output = instr_RCL;
            break;
        }
        case token_SUM: {
            *output = instr_SUM;
            break;
        }
        case token_POW: {
            *output = instr_POW;
            break;
        }
        case token_CMs: {
            *output = instr_CMs;
            break;
        }
        case token_EXC: {
            *output = instr_EXC;
            break;
        }
        case token_PROD: {
            *output = instr_PROD;
            break;
        }
        case token_ABS: {
            *output = instr_ABS;
            break;
        }
        case token_EE: {
            *output = instr_EE;
            break;
        }
        case token_POPEN: {
            *output = instr_POPEN;
            break;
        }
        case token_PCLOSE: {
            *output = instr_PCLOSE;
            break;
        }
        case token_DIV: {
            *output = instr_DIV;
            break;
        }
        case token_ENG: {
            *output = instr_ENG;
            break;
        }
        case token_FIX: {
            *output = instr_FIX;
            break;
        }
        case token_INTG: {
            *output = instr_INTG;
            break;
        }
        case token_DEG: {
            *output = instr_DEG;
            break;
        }
        case token_GTO: {
            *output = instr_GTO;
            break;
        }
        case token_MUL: {
            *output = instr_MUL;
            break;
        }
        case token_PAUSE: {
            *output = instr_PAUSE;
            break;
        }
        case token_EQ: {
            *output = instr_EQ;
            break;
        }
        case token_NOP: {
            *output = instr_NOP;
            break;
        }
        case token_OP: {
            *output = instr_OP;
            break;
        }
        case token_RAD: {
            *output = instr_RAD;
            break;
        }
        case token_SBR: {
            *output = instr_SBR;
            break;
        }
        case token_SUB: {
            *output = instr_SUB;
            break;
        }
        case token_LBL: {
            *output = instr_LBL;
            break;
        }
        case token_GE: {
            *output = instr_GE;
            break;
        }
        case token_STATSUM: {
            *output = instr_STATSUM;
            break;
        }
        case token_MEAN: {
            *output = instr_MEAN;
            break;
        }
        case token_GRAD: {
            *output = instr_GRAD;
            break;
        }
        case token_RST: {
            *output = instr_RST;
            break;
        }
        case token_ADD: {
            *output = instr_ADD;
            break;
        }
        case token_STFLG: {
            *output = instr_STFLG;
            break;
        }
        case token_IFFLG: {
            *output = instr_IFFLG;
            break;
        }
        case token_DMS: {
            *output = instr_DMS;
            break;
        }
        case token_PI: {
            *output = instr_PI;
            break;
        }
        case token_LIST: {
            *output = instr_LIST;
            break;
        }
        case token_STOP: {
            *output = instr_STOP;
            break;
        }
        case token_NEG: {
            *output = instr_NEG;
            break;
        }
        case token_RESULT: {
            *output = instr_RESULT;
            break;
        }
        case token_WRITE: {
            *output = instr_WRITE;
            break;
        }
        case token_DSZ: {
            *output = instr_DSZ;
            break;
        }
        case token_ADV: {
            *output = instr_ADV;
            break;
        }
        case token_PRT: {
            *output = instr_PRT;
            break;
        }
        default: return false;
    }
    return true;
}

/*
 * This function, given a packed token, parses
 * either an uncommon label or a user defined
 * key from it, filling whatever was parsed in
 * the relative argument.
 * This function returns a positive value in case
 * an uncommon label was parsed, a negative value
 * in case of a user defined key, and 0 in case
 * no valid token was parsed.
*/
signed char parse_uncommon_token(uint64_t packed_token, uncommon_labels* ulabel, user_defined_keys* key) {
    switch(packed_token) {
        case token_INV:
            *ulabel = ulabel_INV;
            return 1;
        case token_DEC:
            *ulabel = ulabel_DEC;
            return 1;
        
        case token_A:
            *key = A_lbl;
            return -1;
        
        case token_B:
            *key = B_lbl;
            return -1;
        
        case token_C:
            *key = C_lbl;
            return -1;
        
        case token_D:
            *key = D_lbl;
            return -1;
        
        case token_E:
            *key = E_lbl;
            return -1;
        
        case token_Ap:
            *key = A_first;
            return -1;
        
        case token_Bp:
            *key = B_first;
            return -1;
        
        case token_Cp:
            *key = C_first;
            return -1;
        
        case token_Dp:
            *key = D_first;
            return -1;
        
        case token_Ep:
            *key = E_first;
            return -1;

        default: return 0;
    }
}

/*
 * This function, given a tokenized line of code,
 * parses the main instruction contained in it,
 * parsing any number in case of a number instruction,
 * and checks if it has an INV modifier to invert
 * the function. In case the instruction is a program
 * pointer, the number output value will contain the
 * assigned program pointer ID for that pointer.
 * It then converts the instruction string to
 * the input instructions enumerator in an
 * optimized and efficient way.
 * Any error occurred during parsing is returned
 * as the function return value
*/
return_val parse_instruction(char* line_buffer, size_t bufsize, input_instructions* output, bool* is_inverted, instr_data* data, size_t* instruction_end_index) {
    // Find the pointer (in the line buffer) of the first token
    size_t first_token_size;
    size_t first_token_start;
    int result = parse_token(0, line_buffer, bufsize, &first_token_start, &first_token_size);
    if(result != 0) return rv_from_code(PARSER_INSTR_NO_TOKEN); // No tokens found
    char* first_token = &line_buffer[first_token_start];

    /* Now, pack this token into a 64bit variable for the 
     * fast parse operation of switch-case.
     */
    uint64_t packed_token = pack_token(first_token, first_token_size);

    // Now parse the packed token
    switch(packed_token) {
        // In case a number is found
        case NUMBER_TOKEN: {
            // Parse the number, save it and return
            char* parse_end;
            double parse = strtod(first_token, &parse_end);
            if(parse_end == first_token) return rv_from_code(PARSER_INSTR_NO_NUMBER); // Parse error, no number
            data->number_instr.size = (unsigned int)(parse_end - first_token);
            data->number_instr.number = parse;
            *output = instr_number;
            *is_inverted = false;
            *instruction_end_index = first_token_start + first_token_size;

            return rv_from_success(); // Success
        }

        case token_INV: {
            // Find the location where the current token ends
            size_t second_token_start, second_token_size;
            int result = parse_token(first_token_start + first_token_size, line_buffer, bufsize, &second_token_start, &second_token_size);
            if(result != 0) return rv_from_code(PARSER_INSTR_NO_INV); // No second token found after INV
            char* second_token = &line_buffer[second_token_start];

            // Now parse that value as the instruction
            uint64_t packed_second_token = pack_token(second_token, second_token_size);
            input_instructions converted_instruction;
            bool parse_result = parse_instruction_token(packed_second_token, &converted_instruction);
            if(!parse_result) return rv_from_code(PARSER_INSTR_INVALID_INV); // Invalid token found after INV

            *output = converted_instruction;
            *is_inverted = true;
            *instruction_end_index = second_token_start + second_token_size;
            return rv_from_success(); // Success
        }

        case PROG_POINTER_TOKEN: { // Associate the program pointer with its ID
            // Add a new program pointer map but ignore the colon at the beginning
            int assigned_value;
            int result = add_element_map_noid(&(parser_state.progpointer_map), first_token + 1, first_token_size - 1, &assigned_value);
            if(result != 0) return rv_from_code(PROGRAM_POINTER_ALLOC_ERROR); // Cannot allocate a new program pointer

            // Construct the output
            *output = prog_pointer;
            *is_inverted = 0;
            data->prog_pointer = assigned_value;
            return rv_from_success(); // Success
        }

        case VARIABLE_TOKEN: { // Associate the given name to a register
            // Parse the next two tokens which should contain the equals sign and the register name
            size_t reg_token_start, reg_token_size;
            int result = parse_token(first_token_start + first_token_size, line_buffer, bufsize, &reg_token_start, &reg_token_size);
            if(result != 0) return rv_from_code(PARSER_INSTR_VAR_NO_TOKEN); // No second token found after $
            if(line_buffer[reg_token_start] != '=') return rv_from_code(PARSER_INSTR_VAR_NO_EQUAL); // Expected '=' token
            // Now parse the actual register value
            result = parse_token(reg_token_start + reg_token_size, line_buffer, bufsize, &reg_token_start, &reg_token_size);
            if(result != 0) return rv_from_code(PARSER_INSTR_VAR_NO_REG); // No valid register token found
            // Now parse the actual register number
            char* parse_end;
            int reg_parsed = (int)strtol(&line_buffer[reg_token_start], &parse_end, 10);
            if(parse_end == &line_buffer[reg_token_start]) return rv_from_code(PARSER_INSTR_VAR_INVALID_REG); // No valid number found
            if(reg_parsed < 0 || reg_parsed > 99) return rv_from_code(PARSER_INSTR_VAR_REG_OUTOFRANGE); // Register number out of range

            // Now add a new element map
            result = add_element_map(&(parser_state.variable_map), first_token + 1, first_token_size - 1, reg_parsed);
            if(result != 0) return rv_from_code(VARIABLE_REGISTER_ALLOC_ERROR); // Cannot allocate a new variable map

            // Construct the output
            *output = reg_variable;
            *is_inverted = 0;
            data->number_instr.number = reg_parsed;
            return rv_from_success();
        }

        case INVALID_TOKEN:
            return rv_from_code(PARSER_INSTR_INVALID_TOKEN); // Invalid token found
        case UINT64_MAX:
            return rv_from_code(PARSER_INSTR_TOO_LONG_TOKEN); // Token too long

        default: { // Just parse the instruction
            input_instructions parsed_instruction;
            if(!parse_instruction_token(packed_token, &parsed_instruction))
                return rv_from_code(PARSER_INSTR_INVALID_INSTR); // Invalid instruction token found
            // In case of no error, save the parse output and return success
            *output = parsed_instruction;
            *is_inverted = false;
            *instruction_end_index = first_token_start + first_token_size;
            return rv_from_success(); // Success
        }
    }
}

/*
 * Given an instruction, this function checks
 * against the type of arguments it accepts
 * and prepares the contents of the input line
 * argument passed in such a way that they are
 * statically allocated to make space for the
 * arguments that specific instruction accepts.
 * This function returns true on success.
 */
bool create_arguments_allocation(input_instructions instruction, bool is_inverted, input_line* output) {
    // Save the instruction and the inverse
    output->inverse = is_inverted;
    output->instruction = instruction;

    // Switch between all instructions and handle them accordingly
    switch(instruction) {
        // If it's a number or a program pointer also initialise the number variable
        // and save it as no parameters
        case prog_pointer:
        case instr_number:
        case reg_variable:
            output->data.number_instr.number = 0.00;
            output->data.number_instr.size = 0;
        // All instruction that require no parameters
        case ret_alias:
        case instr_CLR:
        case instr_NatLog:
        case instr_CE:
        case instr_LOG:
        case instr_CP:
        case instr_TAN:
        case instr_EXCXT:
        case instr_SQR:
        case instr_SQRT:
        case instr_RECIP:
        case instr_PTOR:
        case instr_SIN:
        case instr_COS:
        case instr_POW:
        case instr_CMs:
        case instr_ABS:
        case instr_EE:
        case instr_POPEN:
        case instr_PCLOSE:
        case instr_DIV:
        case instr_ENG:
        case instr_INTG:
        case instr_DEG:
        case instr_MUL:
        case instr_PAUSE:
        case instr_NOP:
        case instr_RAD:
        case instr_SUB:
        case instr_STATSUM:
        case instr_MEAN:
        case instr_GRAD:
        case instr_RST:
        case instr_ADD:
        case instr_DMS:
        case instr_PI:
        case instr_LIST:
        case instr_STOP:
        case instr_NEG:
        case instr_RESULT:
        case instr_WRITE:
        case instr_ADV:
        case instr_PRT:
            output->parameters_amount = 0;
            output->parameters = NULL;
            return true;
 
        case instr_PGM: {
            // 1 Parameter for the progarm instruction
            output->parameters_amount = 1;
            // Create the statically allocated array
            static input_parameter program_parameter[] = {
                {.type = program_code}
            };
            output->parameters = program_parameter;

            return true;
        }

        // All instruction that take in a register as parameter
        case instr_STO:
        case instr_RCL:
        case instr_SUM:
        case instr_EXC:
        case instr_PROD:
        {
            // 1 Parameter for the register (indirect or not)
            output->parameters_amount = 1;
            // Create a statically allocated array
            static input_parameter register_parameter[] = {
                {.type = register_index}
            };
            output->parameters = register_parameter;

            return true;
        }

        case instr_FIX: {
            // 1 Parameter for the decimal units
            output->parameters_amount = 1;
            // Create a statically allocated array
            static input_parameter decimal_units[] = {
                {.type = decimal_amount}
            };
            output->parameters = decimal_units;

            return true;
        }

        // Instructions which accept a location
        case instr_GTO:
        case instr_SBR:
        case instr_EQ:
        case instr_GE:
        {
            // 1 parameter for the location
            output->parameters_amount = 1;
            // Create a statically allocated array
            static input_parameter destination_param[] = {
                {.type = location}
            };
            output->parameters = destination_param;

            return true;
        }

        case instr_LBL: {
            // 1 parameter for the label name
            output->parameters_amount = 1;
            // Create a statically allocated array
            static input_parameter label_name[] = {
                {.type = label}
            };
            output->parameters = label_name;

            return true;
        }

        case instr_OP: {
            // 1 parameter for the operation code
            output->parameters_amount = 1;
            // Create a statically allocated array
            static input_parameter operation_code[] = {
                {.type = operation}
            };
            output->parameters = operation_code;

            return true;
        }

        case instr_STFLG: {
            // 1 parameter for the flag index
            output->parameters_amount = 1;
            // Create a statically allocated array
            static input_parameter flag_index[] = {
                {.type = digit}
            };
            output->parameters = flag_index;

            return true;
        }

        case instr_IFFLG: 
        case instr_DSZ:
        {
            // 2 Parameters for these instructions
            output->parameters_amount = 2;
            // Create the static array for the parameters
            static input_parameter instr_parameters[] = {
                // The first parameter is a digit for the IFFLG,
                // and a register 0-9 for DSZ
                {.type = digit},
                {.type = location}
            };
            output->parameters = instr_parameters;

            return true;
        }

        default: return false; // Unknown instruction
    }
}

/*
 * This code, given an already packed token 
 * containing a common or uncommon label, it
 * parses its contents and saved a valid label 
 * code object in the passed pointer.
 * If the parse completes succesfully, true is returned.
*/
bool parse_label_code(uint64_t packed_token, label_code* output) {
    input_instructions clabel;
    uncommon_labels uclabel;
    user_defined_keys udkey;

    // First, try to parse an instruction out of this token
    if(parse_instruction_token(packed_token, &clabel)) {
        // Check for invalid common labels
        if(clabel < 10) return false;

        output->label_type = 0;
        output->label.common_label = clabel;
        return true;
    } 
    
    int result = parse_uncommon_token(packed_token, &uclabel, &udkey);
    if(result == 0) return false; // Not even an uncommon label was found

    if(result > 0) output->label.uncommon_label = uclabel;
    else if(result < 0) output->label.user_defined_key = udkey;
    output->label_type = (signed char)result;
    return true;
}

/*
 * Given a tokenized line and the index of the
 * end of the instruction token or tokens, this
 * function fills the input_line's input_parameter
 * array (which must have already been initialised
 * with create_arguments_allocation) and parses
 * all the values read in the tokens.
 * This function returns any error encountered with
 * the location being set to the index of
 * which argument caused the error, starting from zero.
 */
return_val parse_arguments(char* line_buffer, size_t bufsize, size_t instruction_end_index, input_line* parse_data) {
    // Where to start the search for the next token
    size_t next_token_search = instruction_end_index + 1;

    // Iterate through the arguments in the passed data
    for(int param = 0; param < parse_data->parameters_amount; param++) {
        // Get a pointer to the current parameter
        input_parameter* current_param = &parse_data->parameters[param];
        // Parse the next token
        size_t param_token, param_token_size;
        int result = parse_token(next_token_search, line_buffer, bufsize, &param_token, &param_token_size);
        if(result != 0)
            // No token found on [param]
            return rv_from_location(PARSER_ARG_NO_TOKEN_FOUND, param);
        // Flag saved in case the token is indirect
        next_token_search = param_token + param_token_size + 1;
        // Save the current parameter type
        parameter_type current_param_type = current_param->type;
        // Check if this token is the IND modifier
        if(nnt_strcmp(IND_MODIFIER, &line_buffer[param_token], param_token_size)) {
            // Set the inverse instruction flag
            current_param->indirect = true;
            // Parse the actual parameter
            int result = parse_token(next_token_search, line_buffer, bufsize, &param_token, &param_token_size);
            if(result != 0) {
                // No second token found on [param]
                return rv_from_location(PARSER_ARG_IND_NO_VALUE, param);
            }
            // Move the next token pointer forward
            next_token_search = param_token + param_token_size + 1;

            int reg_index;
            /* In case the parameter token starts with a dollar sign then use
             * the variable map for the register index.
             */ 
            if(line_buffer[param_token] == '$') {
                // Get the register id associated with this token
                if(param_token_size < 1) return rv_from_location(PARSER_ARG_IND_NO_VARS, param);
                int found_id = get_element_map(&(parser_state.variable_map), &line_buffer[param_token + 1], param_token_size - 1, &reg_index);
                if(!found_id) return rv_from_location(PARSER_ARG_IND_UNDECLARED_VAR, param);
            } else {
                // Now parse the actual register value this indirect parameter refers to
                char* parse_end;
                reg_index = (int)strtod(&line_buffer[param_token], &parse_end);
                // Check for no valid register found
                if(parse_end == &line_buffer[param_token]) return rv_from_location(PARSER_ARG_IND_NO_NUMBER, param);
                // Check for invalid register range
                if(reg_index < 0 || reg_index >= 100) return rv_from_location(PARSER_ARG_IND_OUTOFRANGE, param);
            }
            
            // Now save the register index and skip
            current_param->is_value = true;
            current_param->data.value = reg_index;
            continue;
        } else current_param -> indirect = false; // This is not an indirect parameter
            
        // Parse the parameter based on the type
        switch(current_param_type) {
            // If an integer numeric value is required:
            case program_code:
            case operation:
            case register_index:
            case decimal_amount:
            case digit:
                // In case this is a register index and we're parsing a variable
                if(
                    current_param->type == register_index || current_param->type == digit
                    && line_buffer[param_token] == '$'
                ) {
                    // Get the register id associated with this token
                    unsigned int reg_id;
                    if(param_token_size < 1) return rv_from_location(PARSER_ARG_IND_NO_VARS, param);
                    int found_id = get_element_map(&(parser_state.variable_map), &line_buffer[param_token + 1], param_token_size - 1, &reg_id);
                    if(!found_id) return rv_from_location(PARSER_ARG_IND_UNDECLARED_VAR, param);

                    /* If the parameter is a digit and the
                     * variable contains a multiple digit number
                     * then error out.
                     */
                    if(current_param->type == digit && reg_id > 9)
                        return rv_from_location(PARSER_ARG_VARIABLE_OUTOFRANGE, param);

                    // Save the retrieved register ID
                    current_param->is_value = true;
                    current_param->data.value = reg_id;
                    break;
                }

                // Parse the number in the parameter token
                char* parse_end;
                int value = (int) strtod(&line_buffer[param_token], &parse_end);
                // Check if the token is an actual number
                if(parse_end == &line_buffer[param_token]) return rv_from_location(PARSER_ARG_NO_NUMBER, param);

                // Check the token range
                if(current_param->type == digit) {
                    // Number 0-9
                    if(value < 0 || value > 9) return rv_from_location(PARSER_ARG_DIGIT_OUTOFRANGE, param);
                } else if(current_param->type == decimal_amount) {
                    // Number 0-8
                    if(value < 0 || value > 9) return rv_from_location(PARSER_ARG_DIGIT_OUTOFRANGE, param);
                } else {
                    // Generic 2 digit number
                    if(value < 0 || value > 99) return rv_from_location(PARSER_ARG_INPUT_OUTOFRANGE, param);
                }

                // Save the value and return
                current_param->is_value = true;
                current_param->data.value = value;
                break;

            case location:
            case label:
                // Pack the parameter token
                uint64_t packed_arg = pack_token(&line_buffer[param_token], param_token_size);
                // If this token is invalid then return
                if(packed_arg == UINT64_MAX || packed_arg == INVALID_TOKEN || packed_arg == NUMBER_TOKEN)
                    return rv_from_location(PARSER_ARG_LABEL_INVALID_TOKEN, param);
                // If this is a program pointer
                if(packed_arg == PROG_POINTER_TOKEN) {
                    // If this isn't a location destination then error out
                    if(current_param->type != location) return rv_from_location(PARSER_ARG_LABEL_UNEXPECTED_PROGPOINTER, param);
                    // Retrieve this program pointer's ID
                    int parsed_progpointer;
                    if(!get_element_map(&(parser_state.progpointer_map), &line_buffer[param_token + 1], param_token_size - 1, &parsed_progpointer)) {
                        // If the element wasn't found, just add it
                        int result = add_element_map_noid(&(parser_state.progpointer_map), &line_buffer[param_token + 1], param_token_size - 1, &parsed_progpointer);
                        if(result != 0) return rv_from_location(PROGRAM_POINTER_ALLOC_ERROR, param); // Cannot allocate a new program pointer
                    }
                    // Save this parsed program pointer ID to the value
                    current_param->data.value = parsed_progpointer;
                    current_param->is_value = true;
                    break;
                }

                // Now parse the label code tied to this parameter
                label_code location_label;
                if(!parse_label_code(packed_arg, &location_label))
                    return rv_from_location(PARSER_ARG_LABEL_INVALID_CODE, param);

                // At this point save the label code and exit
                current_param->data.label = location_label;
                current_param->is_value = false;
                break;
        }
    }

    return rv_from_success();
}

/*
 * This instruction parses a full line of code
 * into the output structure.
 * In case of any error during the parsing this
 * returns an error code and nothing is written
 * to the output.
 * Note that the line buffer passed is modified.
 */
return_val parse_line(input_line* output, char* line_buffer, size_t bufsize) {
    // First thing to do is tokenizing the line for all other functions to work
    unsigned int token_amount = tokenize_line(line_buffer, bufsize);
    if(token_amount < 1) return rv_from_code(PARSER_NO_TOKENS);

    // Now parse the initial instruction
    input_instructions instruction;
    bool inverted;
    instr_data instruction_data;
    size_t instruction_end_index;
    return_val result = parse_instruction(line_buffer, bufsize, &instruction, &inverted, &instruction_data, &instruction_end_index);
    if(!is_success(result)) return result;
    // Now allocate the space for the instruction parameters
    if(!create_arguments_allocation(instruction, inverted, output))
        return rv_from_code(PARSER_ARG_ALLOC_ERROR);

    // If the instruction is a number, then save it in the structure
    if(instruction == instr_number || instruction == prog_pointer) {
        // Success, now exit
        output->data = instruction_data;
        return rv_from_success();
    }

    // Parse the arguments into the output
    result = parse_arguments(line_buffer, bufsize, instruction_end_index, output);
    // If it failed to parse the arguments then return an error value
    if(!is_success(result)) return result;
    // Success
    return rv_from_success();
}

void clear_parser_memory() {
    clear_element_map(&(parser_state.progpointer_map));
    clear_element_map(&(parser_state.variable_map));
}