#include <iostream>
#include <exception>
#include <SDL3/SDL.h>
#include <thread>
#include <chrono>
#include <stdint.h>
#include <fstream>
#include <istream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <exception>
#include <chrono>
#include <thread>
#include <random>
#include <time.h>
// #include "instructions.h"


using namespace std;

using bait = uint8_t;
using Byte = unsigned char;
using Short = unsigned short;

/**
 * 
 * Chip-8 Keyboard
 * 1234     123C
 * QWER --->456D
 * ASDF     789E
 * ZXCV     A0BF
 * 
 */
std::unordered_map<SDL_Keycode, Byte > keyboard = {
    {SDLK_1, 0x01}, {SDLK_2, 0x02}, {SDLK_3, 0x03}, {SDLK_4, 0x0C},
    {SDLK_Q, 0x04}, {SDLK_W, 0x05}, {SDLK_E, 0x06}, {SDLK_R, 0x0D},
    {SDLK_A, 0x07}, {SDLK_S, 0x08}, {SDLK_D, 0x09}, {SDLK_F, 0x0E},
    {SDLK_Z, 0x0A}, {SDLK_X, 0x00}, {SDLK_C, 0x0B}, {SDLK_V, 0x0F}
};
std::unordered_map<SDL_Keycode, Byte > font_address = {

};


/**
 * A call stack implementation for the CHIP 8, implemented using an array.
 *
 *
 */
class Call_Stack
{

public:
    int max_length;
    int top;
    Short *internal_array = nullptr;

    Call_Stack(int array_size)
    {
        internal_array = (Short *)malloc(array_size * sizeof(Short));
        top = 0;
        max_length = array_size;
    };

    Short &peek()
    {
        return *(internal_array + (top - 1));
    };

    void push(Short item)
    {
        *(internal_array + top) = item;
        top++;
    };

    Short &pop()
    {
        top--;
        return *(internal_array + (top));
    };

    bool is_full()
    {
        return top == max_length;
    };

    bool is_empty()
    {
        return top == 0;
    }

    ~Call_Stack()
    {
        free(internal_array);
    }
};

struct Chip8
{
    Short PC;
    Call_Stack SP{16};
    Short I;
    Byte delay_t;
    Byte sound_t;

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
    unordered_map<unsigned int, Byte &> reg_tabl = {
        {0x0, V0}, {0x1, V1}, {0x2, V2}, {0x3, V3}, {0x4, V4}, {0x5, V5}, {0x6, V6}, {0x7, V7}, {0x8, V8}, {0x9, V9}, {0xA, VA}, {0xB, VB}, {0xC, VC}, {0xD, VD}, {0xE, VE}, {0xF, VF}};
};



class Memory
{
public:
    Byte *internal_array = nullptr;
    int max_size;

    Memory(int size)
    {
        internal_array = (Byte *)malloc(size * sizeof(Byte));
        max_size = size;
    };

    ~Memory()
    {
        free(internal_array);
    }

    void write(Byte data, Short address)
    {
        *(internal_array + address) = data;
    }

    Byte read(Short address)
    {
        if ((unsigned int)address >= max_size)
        {
            throw std::out_of_range("Received address bigger than memory");
        }
        return *(internal_array + address);
    }
};

class Display
{
public:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    std::array<std::array<int, 64>, 32> display_buffer{};

    Display()
    {
        window = SDL_CreateWindow("Chip 8", 640, 320, SDL_WINDOW_OPENGL);
        renderer = SDL_CreateRenderer(window, NULL);
        // std::cout << renderer << "renderer" << endl;
        // std::cout << window << "window" << endl;

        if (!renderer)
        {
            std::cout << "Renderer failed " << SDL_GetError() << endl;
        }
        for (int i = 0; i < display_buffer.size(); i++)
        {
            for (int j = 0; j < display_buffer[i].size(); j++)
            {
                display_buffer[i][j] = 0;
            }
        }
    }

    ~Display()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void buffer_pixel(unsigned int x, unsigned int y, unsigned int pixel)
    {
        display_buffer[y][x] = display_buffer[y][x] ^ pixel;
        if (display_buffer[y][x])
        {
            // SDL_RenderPoint(renderer, x, y);
            render_screen();
        }
    }

    void render_screen()
    {
        for (int i = 0; i < display_buffer.size(); i++)
        {
            for (int j = 0; j < display_buffer[i].size(); j++)
            {
                if (display_buffer[i][j])
                {
                    SDL_RenderPoint(renderer, j, i);
                }
            }
        }
    }
};

void load_fonts(Memory &memory)
{
    Byte fonts[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    int font_count = 0;
    for (int i = 0; i < 80; i++)
    {
        memory.write(fonts[i], 0x050 + i);
        if (i % 5 == 0){
            font_address[0x00 + font_count] = 0x050 + i;
            font_count++;
        }
        // std::cout << (unsigned int)memory.read(0x050+i) << endl;
    }
};

void load_program(Memory &memory, string filename)
{
    // IBM Logo.ch8
    std::fstream file("/Users/yahya/Documents/CPP/CHIP8Emulator_CPP/src/corax.ch8", ios::in | std::ios::binary | ios::ate);
    streampos size;
    std::filesystem::path path = "/Users/yahya/Documents/CPP/CHIP8Emulator_CPP/src/corax.ch8";
    int test;
    test = std::filesystem::file_size(path);

    unsigned char *arr;
    if (file.is_open())
    {
        size = file.tellg();
        test = std::filesystem::file_size(path);
        arr = new unsigned char[size];
        file.seekg(0, std::ios::beg);
        file.read((char *)(arr), size);
        std::cout << size << endl;
        for (int i = 0; i < (int)size; i++)
        {
            memory.write(arr[i], 0x200 + i);
        }
    };

    delete[] arr;
    std::cout << "File opened" << endl;
    std::cout << "Using file_size: " << test << endl;
    std::cout << "Using tellg(): " << size << endl;

    file.close();
}

Byte extract_first4bits(Byte opcode)
{
    unsigned int first_four = (opcode >> 4) | 0;
    return first_four;
}

Byte extract_sec4bits(Byte opcode)
{
    unsigned int sec_four = opcode & 15;
    return sec_four;
}

Byte extract_bit(Byte byte, int position)
{
    unsigned int result = (byte >> (position - 1)) & 1;
    return result;
}

/**
 *
 * Program instructions
 *
 */

 /**
  * 
  * 00E0
  * Clears the screen
  * @param display 
  * 
  */
void clear_screen(Display &display)
{
    SDL_SetRenderDrawColor(display.renderer, 0, 0, 0, 255);
    SDL_RenderClear(display.renderer);
}

/**
 * 1NNN
 * Sets PC to NNN
 * 
 */
void jump(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
    Chip8.PC = NNN;
}

/**
 * 
 * 2NNN
 * Pushes current PC to the stack, and sets PC to NNN
 * 
 */
void call(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    Chip8.SP.push(Chip8.PC);
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
    Chip8.PC = NNN;

}

/**
 * 
 * 00EE
 * Pops the memory address stored on the stack, and sets the PC to that address
 * 
 * 
 */
void ret(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    Short address = Chip8.SP.pop();
    Chip8.PC = address;

}

/**
 * 
 * 3XNN
 * Skips the next instruction if the value at VX == NN
 * 
 */
void skip_equal_vx_b(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int reg = extract_sec4bits(first_byte);
    if(Chip8.reg_tabl.at(reg) == second_byte){
        Chip8.PC += 2;
    }
}

/**
 * 
 * 4XNN
 * Skips the next instruction if the value at VX != NN
 * 
 */
void skip_nequal_vx_b(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int reg = extract_sec4bits(first_byte);
    if(Chip8.reg_tabl.at(reg) != second_byte){
        Chip8.PC += 2;
    }
}

/**
 * 
 * 5XY0
 * Skips the next instruction if the value at VX == value at VY
 * 
 */
void skip_equal_vx_vy(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int reg1 = extract_sec4bits(first_byte);
    unsigned int reg2 = extract_first4bits(second_byte);
    if(Chip8.reg_tabl.at(reg1) == Chip8.reg_tabl.at(reg2)){
        Chip8.PC += 2;
    }

}

/**
 * 
 * 9XY0
 * Skips the next instruction if the value at VX != value at VY
 * 
 */
void skip_nequal_vx_vy(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int reg1 = extract_sec4bits(first_byte);
    unsigned int reg2 = extract_first4bits(second_byte);
    if(Chip8.reg_tabl.at(reg1) != Chip8.reg_tabl.at(reg2)){
        Chip8.PC += 2;
    }
}

/**
 * 
 * 6XNN
 * Sets register VX to NN
 * 
 */
void set_register(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int reg = extract_sec4bits(first_byte);
    Chip8.reg_tabl.at(reg) = second_byte;
}

/**
 * 
 * 7XNN
 * Adds NN to register VX
 * 
 */
void add_val_to_reg(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int reg = extract_sec4bits(first_byte);
    Chip8.reg_tabl.at(reg) += second_byte;
}


/**
 * 
 * ANNN
 * Sets I register to NNN
 * 
 */
void set_index(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
    Chip8.I = NNN;
}

void set(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(y_reg);

}

void bitw_or(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(y_reg) | Chip8.reg_tabl.at(x_reg);
}

void bitw_and(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(y_reg) & Chip8.reg_tabl.at(x_reg);
}

void bitw_xor(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(y_reg) ^ Chip8.reg_tabl.at(x_reg);
}
void bit_add(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    int overflowed_result = Chip8.reg_tabl.at(y_reg) + Chip8.reg_tabl.at(x_reg);
    Byte result = Chip8.reg_tabl.at(y_reg) + Chip8.reg_tabl.at(x_reg);
    if (overflowed_result > 0xFF){
        Chip8.VF = 0x01;
    }else{
        Chip8.VF = 0x00;
    }
    Chip8.reg_tabl.at(x_reg) = result;
}

void bit_sub(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    if (Chip8.reg_tabl.at(x_reg) > Chip8.reg_tabl.at(y_reg)){
        Chip8.VF = 0x01;
    }else{
        Chip8.VF = 0x00;
    }
    Byte result = Chip8.reg_tabl.at(x_reg) - Chip8.reg_tabl.at(y_reg);
    
    Chip8.reg_tabl.at(x_reg) = result;
}

void bit_subn(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    if (Chip8.reg_tabl.at(y_reg) > Chip8.reg_tabl.at(x_reg)){
        Chip8.VF = 0x01;
    }else{
        Chip8.VF = 0x00;
    }
    Byte result = Chip8.reg_tabl.at(y_reg) - Chip8.reg_tabl.at(x_reg);
    
    Chip8.reg_tabl.at(x_reg) = result;
}

void bit_shiftr(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(y_reg);
    if (Chip8.reg_tabl.at(x_reg) & 1){
        Chip8.VF = 1;
    }else{
        Chip8.VF = 0;
    }    
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(x_reg)>> 1;
}

void bit_shiftl(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(y_reg);
    if (extract_bit(Chip8.reg_tabl.at(x_reg), 8)){
        Chip8.VF = 1;
    }else{
        Chip8.VF = 0;
    }    
    Chip8.reg_tabl.at(x_reg) = Chip8.reg_tabl.at(x_reg) << 1;
}

void random_nn(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int x_reg = extract_sec4bits(first_byte);
    Chip8.reg_tabl.at(x_reg) = ((rand() % 255) & second_byte) ;
    std::cout << "random" << endl;

}

void jump_offset(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
    Chip8.PC = NNN + Chip8.reg_tabl.at(0x00);
}

void skip_key(Byte first_byte, Chip8 &chip){
    SDL_Event event;
    SDL_PollEvent(&event);
    if(event.type == SDL_EVENT_KEY_DOWN){
        if(keyboard.find(event.key.key) != keyboard.end()){
            unsigned int vx = extract_sec4bits(first_byte);
            if (keyboard.at(event.key.key) == chip.reg_tabl.at(vx)){
                chip.PC += 2;
            }

        }

    }
  
};

void nskip_key(Byte first_byte, Chip8 &chip){
    SDL_Event event;
    SDL_PollEvent(&event);
    if(event.type == SDL_EVENT_KEY_DOWN){
        if(keyboard.find(event.key.key) != keyboard.end()){
            unsigned int vx = extract_sec4bits(first_byte);
            if (keyboard.at(event.key.key) == chip.reg_tabl.at(vx)){
                return;
            }

        }

    }
  chip.PC += 2;
};

/**
 * 
 * FX07
 * 
 */
void set_vx_delay(Byte first_byte, Chip8 &chip){
    unsigned int vx = extract_sec4bits(first_byte);
    chip.reg_tabl.at(vx) = chip.delay_t;

}

/**
 * 
 * FX15
 * 
 */
void set_delay_vx(Byte first_byte, Chip8 &chip){
    unsigned int vx = extract_sec4bits(first_byte);
    chip.delay_t = chip.reg_tabl.at(vx);

}


/**
 * 
 * FX18
 * 
 */
void set_sound_vx(Byte first_byte, Chip8 &chip){
    unsigned int vx = extract_sec4bits(first_byte);
    chip.sound_t = chip.reg_tabl.at(vx);

}

/**
 * 
 * FX1E
 * 
 */
void add_index(Byte first_byte, Chip8 &chip8){
    chip8.I += chip8.reg_tabl.at(extract_sec4bits(first_byte));
}

/**
 * 
 * FX0A
 * 
 */
void get_key(Byte first_byte, Chip8 &chip8){
    SDL_Event event;
    SDL_PollEvent(&event);
    unsigned int vx = extract_sec4bits(first_byte);
    if (event.type == SDL_EVENT_KEY_DOWN){
        if(keyboard.find(event.key.key) != keyboard.end()){
            chip8.reg_tabl.at(vx) = keyboard.at(event.key.key);
            return;
        }
    }
    chip8.PC -=2;
}


/**
 * 
 * FX29
 * 
 */
void set_font(Byte first_byte, Chip8 &chip8){
    unsigned int vx = extract_sec4bits(first_byte);
    chip8.I = font_address[chip8.reg_tabl.at(vx)];
}


/**
 * 
 * FX55
 * 
 */
void store_memory(Byte first_byte, Memory& memory, Chip8 &chip8){
    unsigned int reg_range = extract_sec4bits(first_byte);
    for(int i = 0; i <= reg_range; i++){
        memory.write(chip8.reg_tabl.at(i) ,chip8.I + i);
    }

}

/**
 * 
 * FX65
 * 
 */
void load_memory(Byte first_byte, Memory& memory, Chip8 &chip8){
    unsigned int reg_range = extract_sec4bits(first_byte);
    for(int i = 0; i <= reg_range; i++){
        chip8.reg_tabl.at(i) = memory.read(chip8.I + i);
    }

}

/**
 * 
 * FX33
 * 
 */
void decimal_conversion(Byte first_byte, Memory& memory, Chip8 &chip8){
    unsigned int vx = extract_sec4bits(first_byte);
    unsigned int number = chip8.reg_tabl.at(vx);
    for(int i = 0; i < 3; i++){
        int power = i+1;
        memory.write( (number % (int)pow(10,power))/int(pow(10,power-1)),chip8.I + 2-i);
        number = number - (number % (int)pow(10,power));
        
    }
    

}



/**
 * 
 * DXYN 
 * Displays an N pixel tall sprite stored at the I register. Paints the sprite at position X, Y stored at register VX, VY respectively.
 * 
 */
void display_sprite2(Byte first_byte, Byte second_byte, Display &display, Chip8 &chip, Memory &memory)
{
    unsigned int x_reg = extract_sec4bits(first_byte);
    unsigned int y_reg = extract_first4bits(second_byte);

    Byte x = chip.reg_tabl.at(x_reg) % 64;
    Byte y = chip.reg_tabl.at(y_reg) % 32;
    int height = extract_sec4bits(second_byte);
    SDL_SetRenderDrawColor(display.renderer, 255, 255, 255, 255);
    for (int i = 0; i < height; i++)
    {
        Byte row = memory.read(chip.I + i);
        for (int j = 8, k = 0; j >= 1 && k < 8; j--, k++)
        {
            unsigned int bit = extract_bit(row, j);
            display.buffer_pixel((unsigned int)x + k, (unsigned int)y + i, bit);
        }
    }
    // display.render_screen();
    SDL_RenderPresent(display.renderer);
}

/**
 * 
 * Fetches the instruction stored at PC register. Instructions come in 2 bytes.
 * 
 */
std::array<Byte, 2> fetch(Memory &memory, Chip8 &Chip)
{
    std::array<Byte, 2> instruction;
    instruction[0] = memory.read(Chip.PC);
    instruction[1] = memory.read(Chip.PC + 1);
    std::cout << "Memory: "<<Chip.PC << " " << Chip.PC + 1 << endl;
    Chip.PC += 2;
    return instruction;
}

void decode_8(Byte first_byte, Byte second_byte, Display &display, Memory &memory, Chip8 &Chip8){
    unsigned int fourth_nibble = extract_sec4bits(second_byte);
    switch (fourth_nibble){
        case 0x00:
            set(first_byte, second_byte, Chip8);
            break;
        case 0x01:
            bitw_or(first_byte,second_byte, Chip8);
            break;
        case 0x02:
            bitw_and(first_byte,second_byte, Chip8);
            break;
        case 0x03:
            bitw_xor(first_byte, second_byte, Chip8);
            break;
        case 0x04:
            bit_add(first_byte, second_byte, Chip8);
            break;
        case 0x05:
            bit_sub(first_byte,second_byte, Chip8);
            break;
        case 0x06:
            bit_shiftr(first_byte,second_byte, Chip8);
            break;
        case 0x07:
            bit_subn(first_byte, second_byte, Chip8);
            break;
        case 0x0E:
            bit_shiftl(first_byte, second_byte, Chip8);
            break;
        
    }

}

void decode_F(Byte first_byte, Byte second_byte, Display &display, Memory &memory, Chip8 &chip8){
    switch((unsigned int)second_byte){
        case 0x07:
            set_vx_delay(first_byte, chip8);
            break;
        case 0x15:
            set_delay_vx(first_byte,chip8);
            break;
        case 0x18:
            set_sound_vx(first_byte, chip8);
            break;
        case 0x1E:
            add_index(first_byte, chip8);
            break;
        case 0x0A:
            get_key(first_byte, chip8);
            break;
        case 0x29:
            set_font(first_byte, chip8);
            break;
        case 0x33:
            decimal_conversion(first_byte, memory, chip8);
            break;
        case 0x55:
            store_memory(first_byte, memory, chip8);
            break;
        case 0x65:
            load_memory(first_byte, memory, chip8);
            break;
    }


}

void decode_and_execute(Byte first_byte, Byte second_byte, Display &display, Memory &memory, Chip8 &Chip8, SDL_Event event)
{
    unsigned int first_nibble = extract_first4bits(first_byte);
    switch (first_nibble)
    {
    case 0x0:
        if(extract_sec4bits(second_byte) == 0x0){
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        clear_screen(display);
        }else{
            ret(first_byte, second_byte, Chip8);
        }
        break;
    case 0x1:
        jump(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
        ;
    case 0x2:
        call(first_byte,second_byte,Chip8);
        break;
    case 0x3:
        skip_equal_vx_b(first_byte, second_byte, Chip8);
        break;
    case 0x4:
        skip_nequal_vx_b(first_byte, second_byte, Chip8);
        break;
    case 0x5:
        skip_equal_vx_vy(first_byte, second_byte, Chip8);
        break;
    case 0x6:
        set_register(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
    case 0x7:
        add_val_to_reg(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
    case 0x8:
        decode_8(first_byte,second_byte,display, memory, Chip8);
        break;
    case 0x9:
        skip_nequal_vx_vy(first_byte, second_byte, Chip8);
        break;
    case 0xA:
        set_index(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
        ;
    case 0xB:
        jump_offset(first_byte,second_byte, Chip8);
        break;
    case 0xC:
        random_nn(first_byte, second_byte, Chip8);
        break;
    case 0xD:
        display_sprite2(first_byte, second_byte, display, Chip8, memory);
        // display_sprite2(first_byte, second_byte, display, Chip8, memory);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
    case 0xE:
        if ((unsigned int)second_byte == 0x9E){
            skip_key(first_byte, Chip8);
        }else{
            nskip_key(first_byte, Chip8);
        }
        break;
    case 0xF:
        decode_F(first_byte, second_byte, display, memory, Chip8);
        break;
    }
}



int main(int argc, char *argv[])
{
    Memory memory(4096);
    Chip8 chip8;
    Display display;

    load_fonts(memory);
    load_program(memory, "test"); // sumth wrong with loading program and loading fonts


    SDL_Init(SDL_INIT_VIDEO);
    chip8.PC = 0x200;

    bool running = true;
    
    
    std::array<Byte, 2> instructions;
    std::clock_t start = clock();

//    test area
    // chip8.delay_t = 0xff;

    // chip8.V0 = 0xF0;
    // chip8.V1 = 0x20;
    // bit_shiftr(0x80, 0x10, chip8);
    // std::cout << (unsigned int)chip8.V1 << endl;
    // decode_and_execute(0xC1, 0xFF, display, memory, chip8);
    // std::cout << (unsigned int)chip8.V1 << endl;
    // std::cout << (unsigned int)chip8.VF << endl;

    // chip8.VA = 0x0F;
    // set_font(0x0A, chip8);
    // std::cout << (unsigned int)chip8.I << endl;
    // std::cout << (unsigned int)memory.read(chip8.I) << endl;
// ----------
    // chip8.V0 = 0xF0;
    // chip8.V1 = 0xF0;
    // chip8.V2 = 0xF0;
    // chip8.V3 = 0xF0;
    // chip8.V4 = 0xF0;
    // chip8.V5 = 0xF0;
    // chip8.V6 = 0xF0;
    // chip8.V7 = 0xF0;
    // chip8.V8 = 0xF0;
    // chip8.V9 = 0xF0;
    // chip8.VA = 0xF0;
    // chip8.VB = 0xF0;
    // chip8.VC = 0xF0;
    // chip8.VD = 0xF0;
    // chip8.VE = 0xF0;
    // chip8.VF = 0x0A;
    // chip8.I = 0x200;
    // store_memory(0x00, memory, chip8);
    // for(int i = 0; i <= 0x0f;i++){
    //     std::cout << (unsigned int)memory.read(chip8.I + i) << endl;
    // }
    // chip8.V0 = 0x00;
    // chip8.V1 = 0x00;
    // chip8.V2 = 0x00;
    // chip8.V3 = 0x00;
    // chip8.V4 = 0x00;
    // chip8.V5 = 0x00;
    // chip8.V6 = 0x00;
    // chip8.V7 = 0x00;
    // chip8.V8 = 0x00;
    // chip8.V9 = 0x00;
    // chip8.VA = 0x00;
    // chip8.VB = 0x00;
    // chip8.VC = 0x00;
    // chip8.VD = 0x00;
    // chip8.VE = 0x00;
    // chip8.VF = 0x00;
    // load_memory(0x00, memory, chip8);
    // for(int i = 0; i <= 0x0f;i++){
    //     std::cout << (unsigned int)chip8.reg_tabl.at(i) << endl;
    // }
// ---
    // chip8.I = 0x200;
    // chip8.V2 = 0xAA;
    // decimal_conversion(0x02, memory, chip8);
    // std::cout << (unsigned int)memory.read(chip8.I) << endl;
    // std::cout << (unsigned int)memory.read(chip8.I + 1)  << endl;
    // std::cout << (unsigned int)memory.read(chip8.I + 2)  << endl;

//    --------
    while (running)
    {
        // instructions = fetch(memory, chip8);
        SDL_Event event;
        
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {   
                running = false;
            }
            
        
            
        }
        
        // Delay timer decrement
        if(chip8.delay_t > 0x00){
            if((float)(clock()-start)/CLOCKS_PER_SEC >= 0.001){
                chip8.delay_t -= (Byte)0x01;
                std::cout << (unsigned int)chip8.delay_t << endl;
                start = clock();
            }
        }
        //----------
        // chip8.PC += 2;
        // std::cout << (unsigned int)chip8.PC << endl;
        // std::cout << (unsigned int)chip8.VA << endl;
        // get_key(0x0a,chip8);
        // ---------
        // set_vx_delay(0x0A, chip8);
        instructions = fetch(memory, chip8);
        decode_and_execute(instructions[0], instructions[1], display, memory, chip8, event);
      
    }

    return 0;
};





// void display_sprite_test(int height, Display &display, Chip8 &chip, Memory &memory)
// {

//     Byte x = 5;
//     Byte y = 5;
//     SDL_SetRenderDrawColor(display.renderer, 255, 255, 255, 255);
//     for (int i = 0; i < height; i++)
//     {
//         Byte row = memory.read(chip.I + i);
//         for (int j = 8, k = 0; j >= 1 && k < 8; j--, k++)
//         {
//             unsigned int bit = extract_bit(row, j);
//             if (bit)
//             {
//                 SDL_RenderPoint(display.renderer, x + k, y + i);
//             }

//             // std::cout << bit;
//         }
//         // std::cout << endl;
//     }
//     SDL_RenderPresent(display.renderer);
// }
// FF,00,FF,00,3C,00,FF,FF,00,FF,00,38,,00,3F,80
//
//
//
//

// void display_sprite(Byte first_byte, Byte second_byte, Display &display, Chip8 &chip, Memory &memory)
// {
//     unsigned int x_reg = extract_sec4bits(first_byte);
//     unsigned int y_reg = extract_first4bits(second_byte);

//     Byte x = chip.reg_tabl.at(x_reg) % 64;
//     Byte y = chip.reg_tabl.at(y_reg) % 32;
//     int height = extract_sec4bits(second_byte);
//     SDL_SetRenderDrawColor(display.renderer, 255, 255, 255, 255);
//     for (int i = 0; i < height; i++)
//     {
//         Byte row = memory.read(chip.I + i);
//         for (int j = 8, k = 0; j >= 1 && k < 8; j--, k++)
//         {
//             unsigned int bit = extract_bit(row, j);
//             if (bit)
//             {
//                 SDL_RenderPoint(display.renderer, (unsigned int)x + k, (unsigned int)y + i);
//             }

//             // std::cout << bit;
//         }
//         // std::cout << endl;
//     }
//     SDL_RenderPresent(display.renderer);
// }
//
//
//
//
//
