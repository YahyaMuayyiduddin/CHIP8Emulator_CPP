#pragma once

using Byte = unsigned char;
using Short = unsigned short;


Byte extract_first4bits(Byte opcode);

Byte extract_sec4bits(Byte opcode);

void jump(Byte first_byte, Byte second_byte);

// std::cout << (unsigned int)memory.read(0x200+1);
    // get_first_nibble(memory.read(0x200+2));
    // get_sec_nibble(memory.read(0x200+2));


    // Program program;
    // // program.set_index(0xff);
    // program.cpu->V0 = 0xff;
    // std::cout <<(int)program.cpu->V0 << endl;
    // Memory memory(4096);
    // load_fonts(memory);
    // Define a buffer 
    
    


    // CHIP8_Stack stack(16);
    // stack.push(0xABCD);
    // std::cout<<stack.peek()<<endl;
    // stack.push(0xDEAD);
    // std::cout << stack.pop() <<endl;
    // std::cout << stack.pop() << endl;

    

    // program.set_index(0xF0);
    // std::cout << program.cpu->I;
    // SDL_Init(SDL_INIT_VIDEO);
    // Display display;

    // int i = 0;
    // while(i < 60){
    //     SDL_RenderLine(display.renderer,3,3,20,3);
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    //     i--;

    // };
    // SDL_DestroyRenderer(display.renderer);
    // SDL_DestroyWindow(display.window);
    // SDL_Quit();

    // std::cout << "Hello world" <<endl;
    // SDL_Init(SDL_INIT_VIDEO);
    // SDL_FRect rect;
    // rect.h = 100;
    // rect.w = 50;
    // rect.x = 20;
    // rect.y = 20;
    // SDL_FRect* rectptr = &rect;
    // bool finished = false;
    
    // Display display;
    // SDL_SetRenderLogicalPresentation(display.renderer,640,320,SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    // while(!finished){
    //     SDL_Event event;
    //     while(SDL_PollEvent(&event)){
    //         if(event.type == SDL_EVENT_QUIT){
    //             finished = true;
    //         }

    //     }
        
    //     SDL_SetRenderDrawColor(display.renderer, 0,0,0, 255);
    //     SDL_RenderClear(display.renderer);
    //     SDL_SetRenderDrawColor(display.renderer, 255, 255, 255, 255);
    //     SDL_RenderRect(display.renderer,rectptr);
    //     SDL_RenderPresent(display.renderer);

    // }

    // SDL_DestroyWindow(display.window);
    // SDL_DestroyRenderer(display.renderer);
    // SDL_Quit();
    // Memory memory;
    // std::cout<< "Hello world" << endl;
    // std::cout << *(memory.ram)

