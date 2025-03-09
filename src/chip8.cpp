#include <iostream>

using namespace std;



struct Chip8{
    using Byte = unsigned char;
    using Short = unsigned short;

    Short PC;
    Byte SP;
    Byte I;

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
        *(internal_array + top) = byte;
        top++;
    };

    Byte& pop(){
        top --;
        return *(internal_array+(top));;
        
    };

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

int main(){

  


}