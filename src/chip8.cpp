#include <iostream>
#include <exception>

using namespace std;

using Byte = unsigned char;
using Short = unsigned short;using Short = unsigned short;

struct Chip8{
    

    Short PC;
    Byte SP;
    Short I;

    Byte V0;
    Byte V1;
    Byte V2;
    Byte V3;
    Byte V4;
    Byte V5;
    Byte V6;
    Byte V7;
    Byte V8;
    Byte V9;
    Byte VA;
    Byte VB;
    Byte VC;
    Byte VD;
    Byte VE;
    Byte VF;
};

/**
 * A call stack implementation for the CHIP 8, implemented using an array.
 * 
 * 
 */
class CHIP8_Stack{

    using Byte = unsigned short;
    
    public:
    
    int max_length;
    int top;
    Byte* internal_array;
    
    CHIP8_Stack(int array_size){
        internal_array = (Byte*)malloc(array_size*sizeof(Byte));
        top = 0;
        max_length = array_size;
    };


    Byte& peek(){
        return *(internal_array+(top-1));
    };

    void push(Byte byte){
        if(!is_full()){
            *(internal_array + top) = byte;
            top++;
        }
        
    };

    Byte& pop(){
        if(!is_empty()){
             top --;
            return *(internal_array+(top));;

        }
    };

    bool is_full(){
        return top == max_length;

    };

    bool is_empty(){
        return top == 0;
    }

};


class Memory{
    public:
    using Byte = unsigned char;
    using Short = unsigned short;
    Byte* ram = new Byte[4096];

    Memory(){
        int fonts[] = {0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F}
                    };
        for(int i = 0; i < 45; i++){
            *(ram + i + 0x50) = fonts[i];
        }

}

};

// 00E0 (clear screen)
// 1NNN (jump)
// 6XNN (set register VX)
// 7XNN (add value to register VX)
// ANNN (set index register I)
// DXYN (display/draw)

class Program{

    public:
    Chip8* cpu;
    CHIP8_Stack* stack;
    Memory* ram;


    Program(){

        cpu = new Chip8;
        stack = new CHIP8_Stack(16);
        ram = new Memory;


    }
    

    /**
     * Opcode 1NNN
     * Sets PC to NNN
     * 
     * @param NNN A 12 bit memory address to jump to
     */
    void jump(Short NNN){
        cpu->PC = NNN;
    }

    /**
     * Opcode ANNN
     * Sets I register to NNN
     * 
     * @param NNN A 12 bit memory address to store
     */
    void set_index(Short NNN){
        cpu->I = NNN;

    }








};

int main(){

    Program program;
    program.set_index(0xF0);
    std::cout << program.cpu->I;



  


}