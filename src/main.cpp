#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <chrono>
#include <thread>
#include <string.h>


 int byteswap(unsigned int startNum) {
        unsigned int a = startNum << 8;
        unsigned int b = startNum >> 8;
        unsigned int endNum = a | b;
        unsigned int clean = endNum & 0xFFFF;
        return clean;
 }
 bool keypad[16];
 int main(int argc, char **argv) {
    if (argv[1] == NULL) {
        printf("ERROR:  please enter a file path and speed!\nUsage: './SIMON8 /path/to/rom SPEED(FAST/SLOW)' \n");
        exit(0);
    }
    if (argv[2] == NULL) {
        printf("ERROR:  please enter a file path and speed!\nUsage: './SIMON8 /path/to/rom SPEED(FAST/SLOW)' \n");
        exit(0);
    }
    using namespace std::this_thread;     // sleep_for, sleep_until
    using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
    using std::chrono::system_clock;
    srand(0);
    renderer::cldisp();
     // store current opcode
     // memory
     unsigned char memory[4096];
     // 15 general registers + carry flag
     unsigned char vReg[16];
     // index
     unsigned short I;
     // program counter
     unsigned short proCou;
     // screen
     // open rom file
     unsigned short stack[16];
     unsigned short sp = 0;
     unsigned int DelTimer = 0;
     unsigned int SoundTimer = 0;
     int proSpeed;
     const char *sSpeed = argv[2];
     const uint8_t *keyState = SDL_GetKeyboardState(NULL);
     bool nokeys = true;
    for (int i = 1; i < argc; i++) {
        if (i == 1) {
            printf("loading file: %s\n", argv[i]);
        } else if (i == 2) {
            printf("speed: %s\n", argv[i]);
        }
        
    }
    FILE *file = std::fopen(argv[1], "rb");
    if (file == NULL) {
        printf("ERROR: invalid file or path!!\nUsage: './SIMON8 /path/to/rom SPEED(FAST/SLOW)' \n");
        exit(0);
    }
    if (strcmp(sSpeed, "FAST") == 0) { proSpeed = 256;}
    else if (strcmp(sSpeed, "SLOW") == 0) { proSpeed = 10;}
    else {
        printf("ERROR:  please enter a speed (FAST = 256 instructions per frame, SLOW  = 10 instructions per frame)\n");
        exit(0);
    }
    
    // get size of rom file
    std::fseek(file, 0, SEEK_END);
    const int fileSize = std::ftell(file);
        if (fileSize >  3584) {
            printf("ERROR: file is too large (%d bytes)\nMaximum size is 3.5KB!\n", fileSize);
            exit(0);
        }
        
    std::printf("file size: %d bytes\n", fileSize);
    std::fseek(file, 0, SEEK_SET);
    // load rom file into memory
    std::fread(&memory[0x200], sizeof(char), fileSize, file);
    // load font into memory
    unsigned char font[80] = { 
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
    std::memcpy(&memory[0x50], font, sizeof(font));
    printf("font loaded into memory x50 to memory x9F\n");
    //reset variables
    proCou = 0x200; //start program counter at rom region
    I = 0;
    //byteswap instructions in memory
    renderer::init();
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        nokeys = true;
        while ( SDL_PollEvent( &e ) != 0 ) {
            switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
            nokeys = false;
            switch (e.key.keysym.sym) {
                case SDLK_1:
                keypad[0x1] = 1;
                break;
                case SDLK_2:
                keypad[0x2] = 1;
                break;
                case SDLK_3:
                keypad[0x3] = 1;
                break;
                case SDLK_4:
                keypad[0xC] = 1;
                break;
                case SDLK_q:
                keypad[0x4] = 1;
                break;
                case SDLK_w:
                keypad[0x5] = 1;
                break;
                case SDLK_e:
                keypad[0x6] = 1;
                break;
                case SDLK_r:
                keypad[0xD] = 1;
                break;
                case SDLK_a:
                keypad[0x7] = 1;
                break;
                case SDLK_s:
                keypad[0x8] = 1;
                break;
                case SDLK_d:
                keypad[0x9] = 1;
                break;
                case SDLK_f:
                keypad[0xE] = 1;
                break;
                case SDLK_z:
                keypad[0xA] = 1;
                break;
                case SDLK_x:
                keypad[0x0] = 1;
                break;
                case SDLK_c:
                keypad[0xB] = 1;
                break;
                case SDLK_v:
                keypad[0xF] = 1;
                break;
                default:
                //stuff
                break;
            }
            break;
            case SDL_KEYUP:
            switch (e.key.keysym.sym) {
                case SDLK_1:
                keypad[0x1] = 0;
                break;
                case SDLK_2:
                keypad[0x2] = 0;
                break;
                case SDLK_3:
                keypad[0x3] = 0;
                break;
                case SDLK_4:
                keypad[0xC] = 0;
                break;
                case SDLK_q:
                keypad[0x4] = 0;
                break;
                case SDLK_w:
                keypad[0x5] = 0;
                break;
                case SDLK_e:
                keypad[0x6] = 0;
                break;
                case SDLK_r:
                keypad[0xD] = 0;
                break;
                case SDLK_a:
                keypad[0x7] = 0;
                break;
                case SDLK_s:
                keypad[0x8] = 0;
                break;
                case SDLK_d:
                keypad[0x9] = 0;
                break;
                case SDLK_f:
                keypad[0xE] = 0;
                break;
                case SDLK_z:
                keypad[0xA] = 0;
                break;
                case SDLK_x:
                keypad[0x0] = 0;
                break;
                case SDLK_c:
                keypad[0xB] = 0;
                break;
                case SDLK_v:
                keypad[0xF] = 0;
                break;
                default:
                //stuff
                break;
            }
            break;
            default:
            //stuff
                break;
            }
        }
        for (int f = 0; f < proSpeed; f++) {
        int instr = 0;
        int* instr_ptr = &instr;
        std::memcpy(instr_ptr, &memory[proCou], 2);
        int opcode = byteswap(instr);
        proCou = proCou+2;
        switch (opcode & 0xF000) {
            case 0x0000:
            //check if CLS otherwise, error
            if ((opcode != 0x00E0) & (opcode != 0x00EE) & (opcode != 0x0000)) {
                printf("unimplemented instruction: %04X at %04X\n", opcode, proCou - 2);
                exit(0);
            } else if (opcode == 0x00EE) {
                //return to stack pointer
                proCou = stack[sp];
                sp--;
            } else if (opcode == 0x0000) {
                    //printf("opcode 0000\n");
                    //exit(0);
            } else {
                //clear screen
                renderer::cldisp();
                printf("clear screen\n");
            }
            break;
            case 0xA000:
            ///set index to value
            I = opcode & 0x0FFF;
            //printf("index set to %X\n", I);
            break;
            case 0xC000:
            vReg[(opcode >> 8 ) & 0x0F] = rand() & (opcode & 0xFF);
            break;
            case 0x6000:
            // set vreg[x] to nn
            vReg[(opcode >> 8 ) & 0x0F] = opcode & 0x00FF;
            //printf("set vreg %X to %X\n", (opcode >> 8 ) & 0x0F, opcode & 0x00FF);
            break;
            case 0xD000:
            // todo: display sprite of 000n bytes, starting at memory location I at V0x00 V00y0
            for (int n = 0; n < (opcode & 0xF); n++) {
                for (int s = 0; s < 8; s++){
                    int xInd = vReg[(opcode >> 8) & 0x0F]+ s;
                    int yInd = vReg[(opcode >> 4) & 0x00F]+n;
                    unsigned char bitDex = (memory[I+n] >> (7-s)) & 1;
                    //renderer::draw(xInd,yInd,bitDex);
                    if (renderer::draw(xInd,yInd,bitDex) == true) {vReg[15] = 1;}
                }
            }
            
            break;
            case 0x7000:
            // add nn to vreg[x]
            vReg[(opcode >> 8 ) & 0x0F] += opcode & 0x00FF;
            //printf("added %X to vreg %X\n", opcode & 0x00FF, (opcode >> 8 ) & 0x0F);
            break;
            case 0x4000:
            //skip instruction if vreg is not "kk"
                if (vReg[(opcode >> 8) & 0x0F] != (opcode & 0x00FF)) {
                    proCou += 2;
                }
            break;
            case 0x3000:
            //skip instruction if vreg is "kk"
                if (vReg[(opcode >> 8) & 0x0F] == (opcode & 0x00FF)) {
                    proCou += 2;
                }
            break;
            case 0x5000:
                if (vReg[(opcode >> 8) & 0x0F] == vReg[(opcode >> 4) & 0x00FF])
                {
                    proCou += 2;
                }
                
            break;
            case 0x8000:
                if ((opcode & 0x000F) == 0x0000)
                {
                    vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 4) & 0x00F];
                } else if ((opcode & 0x000F) == 0x0005) {
                    int flag = 0;
                    if (vReg[(opcode >> 8) & 0x0F] >= vReg[(opcode >> 4) & 0x00F]) {
                        flag = 1;
                    } else {
                        flag = 0;
                    }
                    vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 8) & 0x0F] - vReg[(opcode >> 4) & 0x00F];
                    vReg[15] = flag;
                } else if ((opcode & 0x000F) == 0x0007) {
                    int flag = 0;
                    if (vReg[(opcode >> 4) & 0x00F] >= vReg[(opcode >> 8) & 0x0F])  {
                        flag = 1;
                    } else {
                        flag = 0;
                    }
                    vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 4) & 0x00F] - vReg[(opcode >> 8) & 0x0F];
                    vReg[15] = flag;
                } else if ((opcode & 0x000F) == 0x0006) { 
                    //early chip 8 behavior uses this line
                    int flag = 0;
                    //vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 4) & 0x00F];
                    //VF = least significant bit of Vx
                    flag = vReg[(opcode >> 8) & 0x0F] & 1;
                    // Vx = Vx SHR 1
                    vReg[(opcode >> 8) & 0x0F] >>= 1;
                    vReg[15] = flag;
                } else if ((opcode & 0x000F) == 0x000E) {
                    //early chip 8 behavior uses this line
                    //vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 4) & 0x00F];
                    int flag = 0;
                    //VF = most significant bit of Vx
                    flag = (vReg[(opcode >> 8) & 0x0F] >> 7) & 1;
                    // Vx = Vx SHL 1
                    vReg[(opcode >> 8) & 0x0F] <<= 1;
                    vReg[15] = flag;
                } else if ((opcode & 0x000F) == 0x0004) {
                    //add vx and vy, if greater than 8 bits, set vf to 1, store lowest 8 bits in vx
                    int carry = (int) vReg[(opcode >> 8) & 0x0F] + (int) vReg[(opcode >> 4) & 0x00F];
                    int flag = 0;
                    if (carry > 0xFF){
                        flag = 1;
                    } else {
                        flag = 0;
                    }
                    vReg[(opcode >> 8) & 0x0F] = carry & 0xFF;
                    vReg[15] = flag;
                } else if ((opcode & 0x000F) == 0x0003) {
                    //XORcompare vx and vy, store in vx
                    vReg[(opcode >> 8) & 0x0F] ^= vReg[(opcode >> 4) & 0x00F];
                } else if ((opcode & 0x000F) == 0x0001) {
                    //ORcompare vx and vy, store in vx
                    vReg[(opcode >> 8) & 0x0F] |= vReg[(opcode >> 4) & 0x00F];
                } else if ((opcode & 0x000F) == 0x0002) {
                    // "&" compare vx and vy, store in vx
                    vReg[(opcode >> 8) & 0x0F] &= vReg[(opcode >> 4) & 0x00F];
                } else {
                    printf("unimplemented instruction: %04X at %04X\n", opcode, proCou - 2);
                    exit(0);        
                }
                    
            break;
            case 0x9000:
            // skip if vreg X is not vreg Y
                if (vReg[(opcode >> 8) & 0x0F] != vReg[(opcode >> 4) & 0x00F]){
                    proCou += 2;
                }
            break;
            case 0xF000:
                if ((opcode & 0x00FF) == 0x001E)
                {
                    I = I + vReg[(opcode >> 8 ) & 0x0F];
                }  else if ((opcode & 0x00FF) == 0x0055) {
                    //copy vreg 0-x into memory starting at Index
                    int vMax = ((opcode >> 8) & 0x0F);
                    for(int m = 0; m <= vMax; m++) {
                        memory[I+m] = vReg[m];
                    }
                } else if((opcode & 0x00FF) == 0x0065){
                    //read from memory into vreg 0-x starting at Index
                    int vMax = ((opcode >> 8) & 0x0F);
                    for(int m = 0; m <= vMax; m++) {
                        vReg[m] = memory[I+m];
                    }
                } else if ((opcode & 0x00FF) == 0x0033){
                    int bcd = vReg[(opcode >> 8) & 0x0F];
                    memory[I] = bcd/100;
                    memory[I+1] = (bcd/10) % 10;
                    memory[I+2] = bcd % 10;

                } else if ((opcode & 0x00FF) == 0x0029) {
                    I = 0x50 + 5 * vReg[(opcode >> 8) & 0x0F];
                } else if ((opcode & 0x00FF) == 0x0015) {
                    DelTimer = vReg[(opcode >> 8) & 0x0F];
                } else if ((opcode & 0x00FF) == 0x0018) {
                    SoundTimer = vReg[(opcode >> 8) & 0x0F];
                } else if ((opcode & 0x00FF) == 0x0007) {
                    vReg[(opcode >> 8) & 0x0F] = DelTimer;
                } else if((opcode & 0x00FF) == 0x000A){
                    //wait for key press. vX = key press
                    if(nokeys == false){
                    for(int x= 0; x > 15; x++) {
                        if (keypad[x] == 1) {vReg[(opcode >> 8) & 0x0F] = x;}
                        }
                    } else {proCou = proCou -2;}
                } else {
                    printf("unimplemented instruction: %04X at %04X\n", opcode, proCou - 2);
                    exit(0);        
                }
            break;
            case 0xE000:
                if ((opcode & 0x00FF) == 0x009E) {
                    for(int x= 0; x <= 15; x++) {
                        if (vReg[(opcode >> 8) & 0x0F] == x) {
                            if (keypad[x] == 1) {
                                proCou += 2;
                                }
                        }
                    }
                } else if ((opcode & 0x00FF) == 0x00A1) {
                    for(int x= 0; x <= 15; x++) {
                        if (vReg[(opcode >> 8) & 0x0F] == x) {
                            if (keypad[x] == 0) {
                                proCou += 2;
                                }
                        }
                    }
                } else {
                    printf("unimplemented instruction: %04X at %04X\n", opcode, proCou - 2);
                    exit(0);        
                }
            break;
            case 0x2000:
            //Call subroutine at nnn
            sp++;
            stack[sp]= proCou;
            proCou = opcode & 0x0FFF;
            break;
            case 0x1000:
            //jump to address
            proCou = opcode & 0x0FFF;
            //printf ("jump to %X \n", opcode & 0x0FFF);
            break;
            case 0xB000:
            //jump to address
            proCou = (opcode & 0x0FFF) + vReg[0];
            break;
            default:
            printf("unimplemented instruction: %04X at %04X\n", opcode, proCou - 2);
            renderer::close();
            exit(0);
        }
        }


        
        renderer::refresh();
        if (DelTimer != 0) {
            DelTimer--;
            sleep_for(5ms);
        }
        if (SoundTimer != 0) {
            SoundTimer--;
        
        }
    }
    renderer::close();
    printf("SIMON8 has finished running\n");
    return 0;
 }