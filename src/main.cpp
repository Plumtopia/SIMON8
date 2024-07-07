#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>


 int byteswap(unsigned int startNum) {
        unsigned int a = startNum << 8;
        unsigned int b = startNum >> 8;
        unsigned int endNum = a | b;
        unsigned int clean = endNum & 0xFFFF;
        return clean;
 }
 int main(int argc, char **argv) {
    srand(0);
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
     unsigned char disp[64 * 32];
     std::memset(disp, 0, sizeof(disp));
     // open rom file
     unsigned short stack[16];
     unsigned short sp = 0;
    for (int i = 1; i < argc; i++) {
        printf("loading file: %s\n", argv[i]);
    }
    FILE *file = std::fopen(argv[1], "rb");
    // get size of rom file
    std::fseek(file, 0, SEEK_END);
    const int fileSize = std::ftell(file);
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
    while (true) {
        int instr = 0;
        int* instr_ptr = &instr;
        std::memcpy(instr_ptr, &memory[proCou], 2);
        int opcode = byteswap(instr);
        //printf("opcode 0x%X\n", opcode);
        //printf("PrCo %X\n", proCou);
        /*if (proCou == 0x33D) {
        for (int y = 0; y < 32; y++) {
                    for (int x = 0; x < 64; x++) {
                        if (disp[64 * y + x ] == 0){
                        std::printf("  ");
                        } else {
                            std::printf("[]");
                        }
                    }
                    std::printf("\n");
                }
                exit(0);
        }*/
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
                std::memset(disp, 0, sizeof(disp));
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
                    int yInd = (vReg[(opcode >> 4) & 0x00F]+n)*64;
                    unsigned char bitDex = (memory[I+n] >> (7-s)) & 1;
                    disp[xInd + yInd] ^= bitDex;
                    //printf("%u", bitDex);
                }
                //printf("\n");
                /*for (int y = 0; y < 32; y++) {
                    for (int x = 0; x < 64; x++) {
                        if (disp[64 * y + x ] == 0){
                        std::printf("  ");
                        } else {
                            std::printf("[]");
                        }
                    }
                    std::printf("\n");
                }*/
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
                    if (vReg[(opcode >> 8) & 0x0F] >= vReg[(opcode >> 4) & 0x00F]) {
                        vReg[15] = 1;
                    } else {
                        vReg[15] = 0;
                    }
                    vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 8) & 0x0F] - vReg[(opcode >> 4) & 0x00F];
                } else if ((opcode & 0x000F) == 0x0006) { 
                    //early chip 8 behavior uses this line
                    //vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 4) & 0x00F];
                    //VF = least significant bit of Vx
                    vReg[15] = vReg[(opcode >> 8) & 0x0F] & 1;
                    // Vx = Vx SHR 1
                    vReg[(opcode >> 8) & 0x0F] >>= 1;
                } else if ((opcode & 0x000F) == 0x000E) {
                    //early chip 8 behavior uses this line
                    //vReg[(opcode >> 8) & 0x0F] = vReg[(opcode >> 4) & 0x00F];
                    //VF = most significant bit of Vx
                    vReg[15] = (vReg[(opcode >> 8) & 0x0F] >> 7) & 1;
                    // Vx = Vx SHL 1
                    vReg[(opcode >> 8) & 0x0F] <<= 1;
                } else if ((opcode & 0x000F) == 0x0004) {
                    //add vx and vy, if greater than 8 bits, set vf to 1, store lowest 8 bits in vx
                    int carry = (int) vReg[(opcode >> 8) & 0x0F] + (int) vReg[(opcode >> 4) & 0x00F];
                    if (carry > 0xFF){
                        vReg[15] = 1;
                    } else {
                        vReg[15] = 0;
                    }
                    vReg[(opcode >> 8) & 0x0F] = carry & 0xFF;
                } else if ((opcode & 0x000F) == 0x0003) {
                    //compare vx and vy, store in vx
                    vReg[(opcode >> 8) & 0x0F] ^= vReg[(opcode >> 4) & 0x00F];
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
            if (((proCou - 2) & 0x0FFF) == (opcode & 0x0FFF) ) {
                printf("end of rom\n");

                for (int y = 0; y < 32; y++) {
                    for (int x = 0; x < 64; x++) {
                        if (disp[64 * y + x ] == 0){
                        std::printf("  ");
                        } else {
                            std::printf("[]");
                        }
                    }
                    std::printf("\n");
                }

                exit(0);
            }
            proCou = opcode & 0x0FFF;
            //printf ("jump to %X \n", opcode & 0x0FFF);
            break;
            default:
            printf("unimplemented instruction: %04X at %04X\n", opcode, proCou - 2);
            exit(0);
        }
    }
    printf("done");
    return 0;
 }