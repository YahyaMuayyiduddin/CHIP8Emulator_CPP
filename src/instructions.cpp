#include <iostream>

using Byte = unsigned char;
using Short = unsigned short;


Byte extract_first4bits(Byte opcode){
    unsigned int first_four = (opcode >> 4) | 0;
    return first_four;
   
}

Byte extract_sec4bits(Byte opcode){
    unsigned int sec_four = opcode & 15;
    return sec_four;

}

void jump(Byte first_byte, Byte second_byte){
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
}