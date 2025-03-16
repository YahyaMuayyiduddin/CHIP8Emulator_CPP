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
// #include "instructions.h"

using namespace std;

using bait = uint8_t;
using Byte = unsigned char;
using Short = unsigned short;

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
        window = SDL_CreateWindow("Chip 8", 64, 32, SDL_WINDOW_OPENGL);
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
    for (int i = 0; i < 80; i++)
    {
        memory.write(fonts[i], 0x050 + i);
        // std::cout << (unsigned int)memory.read(0x050+i) << endl;
    }
};

void load_program(Memory &memory, string filename)
{
    std::fstream file("/Users/yahya/Documents/CPP/CHIP8Emulator_CPP/src/IBM Logo.ch8", ios::in | std::ios::binary | ios::ate);
    streampos size;
    std::filesystem::path path = "/Users/yahya/Documents/CPP/CHIP8Emulator_CPP/src/IBM Logo.ch8";
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

void clear_screen(Display &display)
{
    SDL_SetRenderDrawColor(display.renderer, 0, 0, 0, 255);
    SDL_RenderClear(display.renderer);
}

void jump(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
    Chip8.PC = NNN;
}

void call(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    Chip8.SP.push(Chip8.PC);
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
    Chip8.PC = NNN;

}

void ret(Byte first_byte, Byte second_byte, Chip8 &Chip8){
    Short address = Chip8.SP.pop();
    Chip8.PC = address;

}

void set_register(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int reg = extract_sec4bits(first_byte);
    Chip8.reg_tabl.at(reg) = second_byte;
}

void add_val_to_reg(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int reg = extract_sec4bits(first_byte);
    Chip8.reg_tabl.at(reg) += second_byte;
}

void set_index(Byte first_byte, Byte second_byte, Chip8 &Chip8)
{
    unsigned int first_N = extract_sec4bits(first_byte);
    Short NNN = first_N << 8 | second_byte;
    Chip8.I = NNN;
}

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

std::array<Byte, 2> fetch(Memory &memory, Chip8 &Chip)
{
    std::array<Byte, 2> instruction;
    instruction[0] = memory.read(Chip.PC);
    instruction[1] = memory.read(Chip.PC + 1);
    std::cout << "Memory: "<<Chip.PC << " " << Chip.PC + 1 << endl;
    Chip.PC += 2;
    return instruction;
}

void decode_and_execute(Byte first_byte, Byte second_byte, Display &display, Memory &memory, Chip8 &Chip8)
{
    unsigned int first_nibble = extract_first4bits(first_byte);
    switch (first_nibble)
    {
    case 0x0:
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        clear_screen(display);
        break;
    case 0x1:
        jump(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
        ;
    case 0x2:;
    case 0x3:;
    case 0x4:;
    case 0x5:;
    case 0x6:
        set_register(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
    case 0x7:
        add_val_to_reg(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
    case 0x8:;
    case 0x9:;
    case 0xA:
        set_index(first_byte, second_byte, Chip8);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
        ;
    case 0xB:;
    case 0xC:;
    case 0xD:
        display_sprite2(first_byte, second_byte, display, Chip8, memory);
        // display_sprite2(first_byte, second_byte, display, Chip8, memory);
        std::cout <<"Opcode: "<< (unsigned int)first_byte << (unsigned int)second_byte << endl;
        break;
    case 0xE:;
    case 0xF:
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

    bool running = false;

    std::array<Byte, 2> instructions;

    call(0x2F,0xFF, chip8);
    std::cout << (unsigned int)chip8.PC << endl;
    ret(0x00,0xEE, chip8);
    std::cout << (unsigned int)chip8.PC << endl;
    
    
    while (running)
    {
        instructions = fetch(memory, chip8);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        // instructions = fetch(memory, chip8);
        decode_and_execute(instructions[0], instructions[1], display, memory, chip8);
        // std::this_thread::sleep_for(std::chrono::milliseconds(250));
        // decode_and_execute(0xDF, 0xAF, display, memory, chip8, call_stack);

        // display_sprite_test(5, display, chip8, memory);
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
