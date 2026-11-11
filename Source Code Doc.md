# Assembly source code instructions
This doc contains a guide on how to actually write valid code that can be compiled with this software.

## Basic code structure
A program written for this compiler uses newlines as instruction separators, like standard ASM, and codes can be written by using the semicolon:

```
; Simple program which sums 5 to the number in the x register

LBL A   ; Label entry point
ADD
5       ; Add 5 to the x register value
RESULT  ; Display the result
STOP    ; Stop the program operation
RST     ; Reset the program counter back to zero
```

*Note*: The instructions are case-insensitive, and any preceeding or trailing whitespace characters are ignored (such as tabs or spaces) so the code can be freely indented.
All the text in a line after a semicolon is ignored, empty lines are ignored too.
A line containing just a comment is valid.

## Writing to x register
To write a value in the x register, that number can just be placed in the code.
Numbers can have a decimal point, it is indicated with a dot, although itself will use another instruction separately, like each digit of the number written.

```
LBL A   ; Label entry point
DIV     ; Divide operation
100.5   ; Place 100.5 in the x register to act as the divisor
RESULT  ; Display the result
STOP    ; STOP the program execution
RST     ; Reset the program counter
```

This program, for example, even if it uses 6 lines of code, it will make use of 11 instructions on the calculator, because LBL uses two program steps for the LBL command and the name of the label, then 100.5 uses five instructions, one for each number's digit and the decimal point separator.
Note that all numbers from 0-9 and even the dot used for the decimal point can all be individual instructions used to fill the x register, even `.5` is a valid line of code, because it contains the decimal point instruction and the 5 instruction.

## Changing the sign of the x register
In case a number to multiply by is negative, this compiler allows the use of the instruction tied to the +/- key to negate the contents of the x register:

```
MUL     ; Multiplication instruction
2       ; Multiplication by -2
NEG     ; Use of the NEG instruction to negate the -2 value
RESULT  ; Show the result
```

This is a separate instruction on the calculator, and it is a generic instruction used to invert whatever the x register contains, even the user input in it:

```
LBL A   ; Label entry point
NEG     ; Invert the user input
MUL     ; Multiply
5       ; By 5
RESULT  ; Show the user input result, multiplied by -5
STOP    ; Stop the program execution
RST     ; Reset
```

*Note on its function*: This operation can also be used to change the sign of the exponent if using `EE` and `Eng` or whatever exponent entry.

## Pi constant
There is an instruction to type out the number **pi** for calculations to 13 significant digits, note that the display only shows 10 and using `CE` on this number won't clear it:

```
LBL A
; Multiply the input by 2 * pi.
MUL
PI
MUL
2
; Show the result
RESULT
STOP
RST
```

## Clearing data
To clear the current program entry, there is a key on the calculator which executes the Clear Entry instruction, and it can be used in this compiler too:

```
ADD
5.56
; Show the user the result
RESULT
; Pause to show the number on screen for a bit
PAUSE
; Clear the entry and continue with the program
CE
```

There are other clearing instructions, this is a complete list:
- `CE`: Clear entry instruction, it clears only entries made with the sign, the digits and the decimal pointer, this does not clear calculated results, numbers recalled from memory, or pi.
- `CLR`: General clear instruction, clears the calculation in progress and the flashing display, it does not affect the data contents or program memory, refer to the manual for further information
- `CP`: Clear Program data, this clears the entire program memory (and protection) and resets the program counter back to zero, refer to the manual for further information
- `CMS`: Clear all memory registers as defined by the current partitioning

The manual section regarding the different kind of clear operations is placed into:
**Personal Programming** - Page V-3

## Inverse function
There is an instruction used to inverse the function exceeding it, in this compiler it's inlined to the function to inverse.
For example, if the programmer wants to compute the inverse cosine of the number in the x register they might do the following:

```
LBL A
INV COS
STOP
RST
```

Refer to the Personal Programming manual, page V-4, for additional information on inverse functions.

## Display formats
There are three main display formats the calculator uses: Scientific (the default), Engineering, and Fixed decimal notation.
Three instructions are tied to these kinds of display formats, and they follow:
- `EE` is the Enter Exponent instruction, it can be used to type a number in mantissa/exponent form when in scientific notation. After this instruction is called, all subsequent numbers entered are displayed in scientific notation, until CLR is called.
- `ENG` Sets the display in ENGINEERING scale mode, refer to the manual for further instructions
- `FIX n` Sets the display in Fixed decimal control, where n is a number from 0 to 8 indicating the amount of digits to use, `FIX 9` or `INV FIX` brings the calculator back to the default mode. Refer to the manual for further information.

*Example*: To type the number 230000000 in scientific notation, it can be typed as 2.3x10^8, in this assembly code it is written as follows:

```
2.3
EE
8
```
*Which only uses up 5 instructions rather than 9 to type the whole number.*

The manual section regarding these display modes is in page V-5 to V-9 of the Personal Programming Manual.

## Basic Arithmetic functions
Here follows a list of the basic arithmetic functions the calculator supports and the relative operations supported by this compiler
- `ADD`, the addition instruction, same as the + key
- `SUB`, the subtraction instruction, same as the - key
- `MUL`, multiplication instruction, same as the x key
- `DIV`, division instruction, same as the ÷ key
- `POPEN` and `PCLOSE` are the parenthesis open and close instructions, these act exactly the same as in the final program, but they must be typed in a single line since they are separate instructions
- `RESULT`, result instruction, same as the = key

A program computing 4 x (5 + 9) / (7 - 4)^(2 + 3) will be:

```
4
MUL
POPEN
    5
    ADD
    9
PCLOSE
DIV
POPEN
    7
    SUB
    4
PCLOSE
POW
POPEN
    2
    ADD
    3
PCLOSE
```

Refer to the Personal Programming manual, page V-10 for further information.

## Algebraic functions
Here follows a list of the instructions for all basic algebraic functions supported by the calculator
- `RECIP`, computes the reciprocal of the x register, same as 1/x key on the calculator
- `NATLOG`, computes the natural log of the x register, same as ln x on the calculator
- `LOG`, computes the common log of the x register, same as log x on the calculator
*These are additional useful reverse functions listed here*
- `INV LOG`, computes the common antilog (10th power) of the x register
- `INV NATLOG`, computes the natural antilog (e^th power) of the x register

Refer to page V-15 onward of the Personal Programming Manual for further information on this kind of functions

### Angular modes
The calculator supports three angular modes, they are selectable with the following instructions:
- `DEG` For an angular mode of degrees
- `RAD` For the angular mode of radiants
- `GRAD` For the angular mode of gradiants

Note that the calculator at startup is always set in degrees mode.

### Trigonometric functions
The three main trig instructions supported by this compiler
- `SIN` Computes the sine of the x register
- `COS` Computes the cosine of the x register
- `TAN` Computes the tangent of the x register

### Integer and absolute value
- `INTG` Removes the decimal part of the x register
    - `INV INTG` Removes the integer part of the x register, only keeping the decimal part
- `ABS` Computes the absolute value of the x register

### Squares and square roots
- `POW`, the power instruction, same as y^x key on the calculator, raises whatever number typed before to whatever power typed after
    - `INV POW` Computes the nth root of the value passed
- `SQR`, The square instruction, same as x^2 key on the calculator, raises the x register to the second power
- `SQRT`, The square root instruction, computes the square root of the x register

## Memory operations
The calculator has a bunch of possible operations which make use of its register space, here follows a list of functions which make use of these registers, keep in mind that when rr is written here, that is the placeholder for the register number the function operates with.
The register numbers are always two digits, even if the instruction must operate in register 1, the instruction to store in that register is still `STO 01`
- `STO rr` Stores the current display value, in the defined register
- `RCL rr` Recalls the value from the rr register, into the display
- `SUM rr` Sums the current display value to the rr register, and saves the result in the rr register
- `PROD rr` Multiplies the current display value with the rr register, and saves the result in the rr register
*Inverse functions are also possible to obtain direct operations with the other registers*
- `INV SUM rr` Subtracts the display value from register rr, and saves the result in rr
- `INV PROD rr` Divides the contents of register rr with the display value, and stores the result in rr
- `EXC rr` Swaps the current display value with the contents of register rr

For additional information, refer to Personal Programming manual, page V-23

## Special operations
Additional special operations can be executed with the OP instruction, refer to the Personal Programming manual, page V-27 for further information.

- `OP nn` Execute special operation with code nn
- `NOP` No-operation instruction. Placeholder for an empty line of code.

## Conversions
Here follows a list of the conversion instructions available in this compiler:

- `EXCXT` Exchanges the display (x) register with the T register, refer to the manual for further information
- `DMS` Converts from degrees, minutes and seconds to decimal degrees
- `PTOR` Converts from polar coordinates to rectangular coordinates, it uses the t register, refer to the manual for further information

Inverse operations are always supported through the `INV` modifier.
The related manual data is present in section V-30 of the Personal Programming manual.

## Statistics functions
Additional statistics functions are listed here, refer to the Personal Programming manual page V-32 for further information

- `STATSUM`: The statistics sum function
- `MEAN`: Calculates and displays the mean of the dependent (y) values
*Additional operations computable through special modifiers*
- `INV MEAN`: Computes the standard deviation of the dependent data in the display register, and the T register contains the standard deviation of the x data
- `OP 11`: Computes the variance of the dependent data in the display register, and the variance of the independent data in the T register

Remember that these functions make use of memory registers 01-06, refer to Personal Programming manual page V-32/40 for additional information on how to make use of these instructions.

# General programming instructions
The main operations that allow the programmer to control the flow of execution of the written program follow here:
- `STOP`: Stops the running program, equivalent to the R/S button on the calculator
- `RST`: Resets the program counter, makes the processor start executing instructions starting from the 0th one
- `PAUSE`: When encountered during program execution, causes the current value in the display register to be displayed to the user for a portion of a second. This instruction can be used whenever needed and can be concatenated to make the display visible for more time

## Labels
Labels are definitions of program entry points, on the TI-58C and the TI-59 labels can be named as any key on the keyboard is, so in this compiler a label name can simply be an instruction, every instruction name in this programming language can be used as label, without parameters, although numbers, program pointers and the RETURN instruction (which is an alias to INV SBR), which are handled differently cannot be used as a label name.
Labels also allow the usage of certain modifiers or user-defined keys such as "A-E" and "A'-E'" as names, to be easily recalled from the user (*Note: The IND modifier can NOT be used as a label name, and the manual explicitly discourages the usage of the STOP operation as label, because it's tied to the key to start/stop the program execution*):

```
LBL CLR
INV SBR ; Return call

LBL INV
; The INV modifier can be used as a label name
SBR CLR ; Call the CLR subroutine
RETURN ; Alias to INV SBR to make reading the program easier

LBL A ; Also B, C, D, E are accepted
; This code can be recalled by just pressing the A key without using SBR before
RETURN

LBL AP ; Also BP, CP, DP, EP are accepted
; Code which can be executed by pressing "2nd A" on the calculator
RETURN
```

For additional information on labels, refer to the Personal Programming manual V-55/56

## Transfer instructions
These are instructions that can disrupt the program flow by changing the program counter:

### Unconditional transfers
GoTo operation: `GTO lbl` or `GTO progPointer`.
The unconditional transfer, or GOTO operation, is an operation that makes the program jump to another line.
On the calculator, the parameter to a GOTO operation can be either a label, or a line number. Since on this compiler line numbers are abstracted, there is another way to jump to a certain section of the program, here the two ways to make an unconditional transfer are shown:

```
LBL A
; Program tied to user-defined key A
RETURN

LBL B
; Other program which jumps to A after its execution
:halfB ; Program pointer for the section where C should jump to
; Rest of the B code
GTO A

LBL C
; Another program which jumps to the middle of B after execution
GTO halfB ; This will be replaced to the line position of halfB at compile time
```

Keep in mind that a program pointer can only contain uppercase or lowercase letters and no symbols.
Refer to page V-56 of the Personal Programming manual for further information

### Subroutines
To call a subroutine: `SBR lbl`
A subroutine acts similarly to the GOTO operation, but it returns back to the place where it gets called after the RETURN instruction:

```
LBL A ; This is the subroutine to execute
; Some code...
RETURN ; Returns after having finished execution

LBL B ; Another program
SBR A ; This will call A, and on the RETURN instruction will jump back here
; Some other instructions
STOP ; Stop program execution without return
```

Subroutines only support labels, thus cannot be called towards program pointers.
To call the return instruction there are two methods:
- `INV SBR`, which is the default calculator method
- `RETURN`, which is a simple alias to the former method, introduced in this compiler to make programming easier, it just executes `INV SBR` under the hood.

Subroutines can also be called inside library programs, to read a library program the following operation can be called:
- `PGM nn`, followed by `SBR A`, for example, to recall the program associated with user-defined key **A**:

```
PGM 15
150
SBR A
SBR B
STO 10
```

This simple program calls the ML-15 program to generate a random number using a set seed and saves it to register 10.
Refer to page V-58 of the Personal Programming manual for further information

### Conditional transfers
Conditional transfers work like the GOTO instruction, but instead they operate the transfer only if the condition tied to them is satisfied.
The comparisons are always made between the display register and the internal T register. (*Reminder, the T register is only accessible by passing data through the display register first and then calling `EXCXT`*)
There are four possible conditional transfers, requiring two kinds of instructions:
- `EQ lbl` or `EQ progPointer`, checks if x and t are equal, if so, jumps to the label or the specified program pointer
- `GE lbl` or `GE progPointer`, checks if x is grater or equal than t, if so, jumps to the label or the specified program pointer
- `INV EQ lbl` or `INV EQ progPointer`, checks if x is not equal to t, if so, jumps to the label or specified program pointer
- `INV GE lbl` or `INV GE progPointer`, checks if x is lower than t, if so, jumps to the label or specified program pointer

Refer to the Personal Programming manual, page V-62 for more information

### Decrement and skip on zero
The calculator supports a simple way to make a cycle, which is called *Decrement and skip on zero*, which is just another conditional transfer.
The instruction tied to this operation is the following:
`DSZ r, lbl` or `DSZ r, progPointer`, where rr is the two digit register and lbl/progPointer is the position to jump to. Only one r is written because this instruction only supports the data registers 0-9.

Refer to page V-64 of the manual for further instructions on how to use this operation, but generally:
- It checks the contents of the r register passed
- If negative, it increments it by one, if positive it decrements it by one (it basically just decrements its absolute value by one)
- It checks if it's equal to zero:
    - If it is, then it jumps to the desired destination
    - If not equal to zero, it skips and keeps processing the following instructions

Here follows a loop of an operation executed 10 times thanks to the DSZ instruction:

```
; Register 01 contains 10 and 02 contains 20
10
STO 01
20
STO 02
; Program pointer for the start loop location
:startLoop
; Decrement 02 by one
1
INV SUM 02
; Execute the loop as many times as the number contained in 01, which is 10, so 10 times.
DSZ 01, startLoop
GTO startLoop
```

## Flags
There are 10 flags in the calculator numbered 0-9 available for use in programs. Refer to page V-65/66 of the Personal Programming manual for further information.
There are four operations that can be executed on these flags, derived from two instructions:
- `STFLG f`, Set the flag, sets the value in the flag f to 1, or true
- `INV STFLG f`, Reset the flag, sets the value in flag f to 0, or false
- `IFFLG f, lbl` or `IFFLG f, progPointer`, Checks the value contained in flag f, if true, skips to the location specified
- `INV IFFLG f, lbl` or `INV IFFLG f, progPointer`, Checks the value contained in flag f, if false, skips to the location specified

Flags can also be used for error conditions, refer to page V-67 of the manual for further information

## Indirect addressing
There is an `IND` modifier which allows the use of certain registers as pointers for other parameters.
For example, the actual register's location to use can be stored in another register, such as register 01, which contains the number 30, and we want to write the current display value in the register whose index is contained in register 01, this is the instruction to run:
`STO IND 01`

Note that IND sometimes is merged to the instruction which is called from, so it doesn't always use an extra instruction, but this depends on the actual instruction it's used on.
As said before, this can be used wherever a parameter is used, even as the amount of digits in the FIX display or as the flag to check, valid lines can be:
- `INV DSZ IND 02, IND 05`
- `GTO IND 04`
- `PGM IND 10`
- `OP IND 15`
- `SBR IND 5`
- `FIX IND 10`
- `IFFLG IND 20, IND 25`

Refer to page V-68/69 of the Personal Programming guide for further information.

