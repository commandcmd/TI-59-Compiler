#include "../src/parser.h"
#include "../src/compiler.h"
#include <stdio.h>

void print_parameter(input_parameter param) {
    // Print The main information
    printf("\t\tType: %d\n\t\tIndirect: %d\n\t\tIsValue: %d\n\t\t", param.type, param.indirect, param.is_value);

    // Check the value
    if(param.is_value) {
        printf("Value: %d\n", param.data.value);
    } else {
        signed char ltype = param.data.label.label_type;
        if(ltype == 0) printf("Common Label: %d\n", param.data.label.label.common_label);
        else if(ltype < 0) printf("Uncommon Label: %d\n", param.data.label.label.uncommon_label);
        else if(ltype > 0) printf("User defined key: %d\n", param.data.label.label.user_defined_key);
    }
}

void print_instr_data(instr_data data) {
    printf("Data:\n\t\tprog_pointer: %u\n\t\tnumber_instr:\n\t\t\tnumber: %f\n\t\t\tsize: %u\n", 
        data.prog_pointer,
        data.number_instr.number,
        data.number_instr.size
    );
}

void print_instr_line(input_line input) {
    printf("[LINE OF CODE]");
    printf("\tInstruction Code: %d\n\tInverse: %d\n\t",
        input.instruction,
        input.inverse
    );

    print_instr_data(input.data);

    printf("[PARAMETERS] - Amount:%d\n", input.parameters_amount);
    for(int i = 0; i < input.parameters_amount; i++)
        print_parameter(input.parameters[i]);
}

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Invalid arguments!\nUsage: main-test \"[STRING-TO-PARSE]\"\n");
        return -1;
    }

    // Parse the line in argv[1] and print it
    input_line output;
    int result = parse_line(&output, argv[1], strlen(argv[1]));
    printf("String parsed - Return code %d:\n", result);
    int reg_pos = 0;
    // If the parse succeeded
    if(result == 0) {
        print_instr_line(output);
        printf("=== OUTPUT ===\n");
        result = convert_line_to_instr(output, stdout, &reg_pos);
        printf("Convert line to instr returned %d\n", result);
    }
    clear_parser_memory();
    free_compiler_mem();

    return 0;
}