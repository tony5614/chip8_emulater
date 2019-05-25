#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#define SCREEN_OFFSET 10
#define SCREEN_DEBUG_WIDTH 320
#define PROGRAM_BYTE_OFFSET (0x200)
#define DEBUG false

typedef unsigned char U8;




class CHIP8CPU
{
private:
    //16 8 bit general register
    U8 V[16];
    U8 DT;
    U8 sp;
    unsigned short stack[16];
    unsigned short pc;
    unsigned short instruction;
    //store memory addr
    unsigned short I;
    //flag register
    U8 memory[4096];
    U8 screen[32][64];
    bool keyin[16];
    int instruction_count;
    char page;
public:
    CHIP8CPU()
    {
    }
    void initialze()
    {
        //initialize hex digit sprite
        
        pc = PROGRAM_BYTE_OFFSET - 2;
        memset((void*)V ,0x00 ,sizeof(V));
        DT = 0;
        I = 0;
        sp = 0;
        
        srand(time(NULL));
        //set fontset sprite
        memory[ 0] = 0xF0; memory[ 1] = 0x90; memory[ 2] = 0x90; memory[ 3] = 0x90; memory[ 4] = 0xF0; memory[ 5] = 0x20; memory[ 6] = 0x60; memory[ 7] = 0x20; memory[ 8] = 0x20; memory[ 9] = 0x70; memory[10] = 0xF0; memory[11] = 0x10; memory[12] = 0xF0; memory[13] = 0x80; memory[14] = 0xF0; memory[15] = 0xF0; memory[16] = 0x10; memory[17] = 0xF0; memory[18] = 0x10; memory[19] = 0xF0; memory[20] = 0x90; memory[21] = 0x90; memory[22] = 0xF0; memory[23] = 0x10; memory[24] = 0x10; memory[25] = 0xF0; memory[26] = 0x80; memory[27] = 0xF0; memory[28] = 0x10; memory[29] = 0xF0; memory[30] = 0xF0; memory[31] = 0x80; memory[32] = 0xF0; memory[33] = 0x90; memory[34] = 0xF0; memory[35] = 0xF0; memory[36] = 0x10; memory[37] = 0x20; memory[38] = 0x40; memory[39] = 0x40; memory[40] = 0xF0; memory[41] = 0x90; memory[42] = 0xF0; memory[43] = 0x90; memory[44] = 0xF0; memory[45] = 0xF0; memory[46] = 0x90; memory[47] = 0xF0; memory[48] = 0x10; memory[49] = 0xF0; memory[50] = 0xF0; memory[51] = 0x90; memory[52] = 0xF0; memory[53] = 0x90; memory[54] = 0x90; memory[55] = 0xE0; memory[56] = 0x90; memory[57] = 0xE0; memory[58] = 0x90; memory[59] = 0xE0; memory[60] = 0xF0; memory[61] = 0x80; memory[62] = 0x80; memory[63] = 0x80; memory[64] = 0xF0; memory[65] = 0xE0; memory[66] = 0x90; memory[67] = 0x90; memory[68] = 0x90; memory[69] = 0xE0; memory[70] = 0xF0; memory[71] = 0x80; memory[72] = 0xF0; memory[73] = 0x80; memory[74] = 0xF0; memory[75] = 0xF0; memory[76] = 0x80; memory[77] = 0xF0; memory[78] = 0x80; memory[79] = 0x80;
    }  
    
    char getKeyhit()
	{
		if(GetKeyState(0x31) < 0)
		{
			//return '1';
			return 0x1;
		}
		else if(GetKeyState(0x32) < 0)
		{              
			//return '2';  
			return 0x2;    
		}
		else if(GetKeyState(0x33) < 0)
		{                  
			//return '3';  
			return 0x3;
		}
		else if(GetKeyState(0x34) < 0)
		{
			//return '4'; 
			return 0xC; 
		}
		else if(GetKeyState(0x51) < 0)
		{ 
			//return 'q';  
			return 0x4; 
		}
		else if(GetKeyState(0x57) < 0)
		{
			//return 'w'; 
			return 0x5;
		}
		else if(GetKeyState(0x45) < 0)
		{
			//return 'e'; 
			return 0x6;
		}
		else if(GetKeyState(0x52) < 0)
		{
			//return 'r'; 
			return 0xD;
		}
		else if(GetKeyState(0x41) < 0)
		{
			//return 'a';  
			return 0x9;  
		}
		else if(GetKeyState(0x53) < 0)
		{
			//return 's';  
			return 0x8;
		}
		else if(GetKeyState(0x44) < 0)
		{
			//return 'd';  
			return 0x9;  
		}
		else if(GetKeyState(0x46) < 0)
		{ 
			//return 'f'; 
			return 0xE;
		}
		else if(GetKeyState(0x5A) < 0)
		{
			//return 'z';  
			return 0xA;  
		}
		else if(GetKeyState(0x58) < 0)
		{
			//return 'x';  
			return 0x0;
		}
		else if(GetKeyState(0x43) < 0)
		{
			//return 'c';  
			return 0xB; 
		}
		else if(GetKeyState(0x56) < 0)
		{ 
			//return 'v';
			return 0xF;  
		}
		return -1;
	}
    
    void readROM(std::string filename)
    {
        std::ifstream fin(filename.c_str(), std::ios::in | std::ios::binary);
        if(!fin)
        {
            if(DEBUG)printf("cannot open file\n");
            return;
        }
        int program_byte_index = PROGRAM_BYTE_OFFSET;
        char one_byte;
        while(!fin.eof())
        {
            fin.get(one_byte);
            memory[program_byte_index] = (U8)one_byte;
            if(DEBUG)printf("%02X ",(U8)one_byte);
            program_byte_index ++;
            if((program_byte_index % 16) == 0)
                if(DEBUG)printf("| %d\n",program_byte_index);
        }
        instruction_count = (program_byte_index - PROGRAM_BYTE_OFFSET);
        if(DEBUG)printf("instruction_count = %d\n",instruction_count);
    }
    void showSpriteXY(U8 x, U8 y, U8 *sprite , U8 height)
    {
        //setactivepage(page);
        //cleardevice(); 
        U8 idx_x,idx_y;
        U8 left_shift = 0;
        //rectangle(x * 10 + SCREEN_OFFSET, y * 10 + SCREEN_OFFSET, (x + 8) * 10 + SCREEN_OFFSET, (y + height) * 10 + SCREEN_OFFSET);
        
        V[0xF] = 0;
        for(idx_y = 0; idx_y < height; idx_y++)
        {
            if(DEBUG)printf("%02X \n",sprite[idx_y]);
            for(idx_x = 0; idx_x < 8 ; idx_x++)
            {
                if((sprite[idx_y] << idx_x) & 0x80)
                {
                    //if(DEBUG)printf("%04d %04d to ",(idx_x + x) * 10 + SCREEN_OFFSET, (idx_y + y) * 10 + SCREEN_OFFSET);
                    //if(DEBUG)printf("%04d %04d\n",(idx_x + x) * 10, (idx_y + y) * 10);   
                    if(getpixel((idx_x + x) * 10 + SCREEN_OFFSET + 3, (idx_y + y) * 10 + SCREEN_OFFSET + 3) == 0)
                    {                
                        bar((idx_x + x) * 10 + SCREEN_OFFSET,(idx_y + y) * 10 + SCREEN_OFFSET,(idx_x + x + 1) * 10 + SCREEN_OFFSET,(idx_y + y + 1) * 10 + SCREEN_OFFSET);
                    }  
                    else if(getpixel((idx_x + x) * 10 + SCREEN_OFFSET + 3, (idx_y + y) * 10 + SCREEN_OFFSET + 3) > 0)
                    {
                        //return true;
                        setfillstyle(1,BLACK);
                        bar((idx_x + x) * 10 + SCREEN_OFFSET,(idx_y + y) * 10 + SCREEN_OFFSET,(idx_x + x + 1) * 10 + SCREEN_OFFSET,(idx_y + y + 1) * 10 + SCREEN_OFFSET);
                        setfillstyle(1,WHITE);
                        V[0xF] = 1;
                    }                
                }
            }   
        }
        
        //page = !page;
        //setvisualpage(page);
    }
    
    bool calcSpriteXOR(U8 x, U8 y, U8 *sprite , int height) 
    {/*
        U8 idx_x,idx_y;
        for(idx_y = 0; idx_y < height; idx_y++)
        {
            for(idx_x = 0; idx_x < 8 ; idx_x++)
            {
                if((((sprite[idx_y] << idx_x) & 0x80) > 0) && (getpixel((idx_x + x) * 10 + SCREEN_OFFSET + 3, (idx_y + y) * 10 + SCREEN_OFFSET + 3) > 0))
                {
                    return false;
                }  
                else if((((sprite[idx_y] << idx_x) & 0x80) > 0) && (getpixel((idx_x + x) * 10 + SCREEN_OFFSET + 3, (idx_y + y) * 10 + SCREEN_OFFSET + 3) == 0))
                {
                    return true;
                }
                else if((((sprite[idx_y] << idx_x) & 0x80) == 0) && (getpixel((idx_x + x) * 10 + SCREEN_OFFSET + 3, (idx_y + y) * 10 + SCREEN_OFFSET + 3) > 0))
                {
                    return true;
                }
                else if((((sprite[idx_y] << idx_x) & 0x80) == 0) && (getpixel((idx_x + x) * 10 + SCREEN_OFFSET + 3, (idx_y + y) * 10 + SCREEN_OFFSET + 3) == 0))
                {
                    return false;
                }  
            }         
        }*/
    }
    void run()
    {
        if(DEBUG)printf("\n");
        U8 x,y,n,kk;
        unsigned short nnn;
        U8 sprite[16];
        char keyin;
        char tmp_string[40] = "\0";

        
        
        while(true)
        //for (int i = PROGRAM_BYTE_OFFSET ; i < (PROGRAM_BYTE_OFFSET + instruction_count) ; i += 2)
        {
             //getch() ;
            
           
        if(DEBUG)getch();    
        
        if(DT > 0)
            DT--;
            
            
            Sleep(3);
            //chip8 is big endien
            //instruction = (memory[i] << 8) | (memory[i + 1]);
            instruction = (memory[pc] << 8) | (memory[pc + 1]);
            
            
            //process instruction
            x   = (instruction & 0xF00) >> 8;
            y   = (instruction & 0x0F0) >> 4;
            n   = (instruction & 0x00F);
            nnn = (instruction & 0xFFF);
            kk  = (instruction & 0x0FF);
            
            if(DEBUG)printf("%04X: %04X , x = %X, y = %X, nnn = %03X, n = %X, kk = %02X\n\t" , pc, instruction, x ,y,nnn ,n ,kk);
            
            
            
            
            
                   
        // debug window
        sprintf(tmp_string , "V[0] = %02X  " , V[0]);
        outtextxy(640 + 2 * SCREEN_OFFSET,SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[1] = %02X  " , V[1]);
        outtextxy(640 + 2 * SCREEN_OFFSET,20 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[2] = %02X  " , V[2]);
        outtextxy(640 + 2 * SCREEN_OFFSET,40 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[3] = %02X  " , V[3]);
        outtextxy(640 + 2 * SCREEN_OFFSET,60 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[4] = %02X  " , V[4]);
        outtextxy(640 + 2 * SCREEN_OFFSET,80 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[5] = %02X  " , V[5]);
        outtextxy(640 + 2 * SCREEN_OFFSET,100 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[6] = %02X  " , V[6]);
        outtextxy(640 + 2 * SCREEN_OFFSET,120 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[7] = %02X  " , V[7]);
        outtextxy(640 + 2 * SCREEN_OFFSET,140 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[8] = %02X  " , V[8]);
        outtextxy(640 + 2 * SCREEN_OFFSET,160 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[9] = %02X  " , V[9]);
        outtextxy(640 + 2 * SCREEN_OFFSET,180 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[A] = %02X  " , V[0xA]);
        outtextxy(640 + 2 * SCREEN_OFFSET,200 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[B] = %02X  " , V[0xB]);
        outtextxy(640 + 2 * SCREEN_OFFSET,220 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[C] = %02X  " , V[0xC]);
        outtextxy(640 + 2 * SCREEN_OFFSET,240 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[D] = %02X  " , V[0xD]);
        outtextxy(640 + 2 * SCREEN_OFFSET,260 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[E] = %02X  " , V[0xE]);
        outtextxy(640 + 2 * SCREEN_OFFSET,280 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "V[F] = %02X  " , V[0xF]);
        outtextxy(640 + 2 * SCREEN_OFFSET,300 + SCREEN_OFFSET,tmp_string);
        
        sprintf(tmp_string , "PC = %04X " , pc);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,0 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "SP = %02X " , sp);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,20 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "I = %04X " , I);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,40 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "DT = %04X " , DT);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,60 + SCREEN_OFFSET,tmp_string);
        
        
        sprintf(tmp_string , "memory[%04X] = %02X  " , I, memory[I]);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,80 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "memory[%04X] = %02X  " , I + 1, memory[I + 1]);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,100 + SCREEN_OFFSET,tmp_string);
        sprintf(tmp_string , "memory[%04X] = %02X  " , I + 2, memory[I + 2]);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,120 + SCREEN_OFFSET,tmp_string);
            
            
        pc += 2;
            
            
            
            switch(instruction & 0xF000)
            {
                case 0x0000:
                    if(instruction == 0x00E0)
                    {
                        if(DEBUG)printf("%04X CLS\n",instruction);
                        cleardevice(); 
                    }
                    else if(instruction == 0x00EE)
                    {
                        if(DEBUG)printf("%04X RET\n",instruction);
                        pc = stack[sp];
                        sp--;
                    }
                    else
                    {                    
                        if(DEBUG)printf("unknown instruction : %04X \n", instruction);   
                    }
                    break;
                    
                case 0x1000:
                    if(DEBUG)printf("%04X JP\n",instruction);
                    pc = nnn;
                    break;
                    
                case 0x2000:
                    sp++;
                    stack[sp] = pc;
                    pc = nnn;
                    if(DEBUG)printf("%04X CALL %04X\n",instruction,pc);
                    break;
                    
                case 0x3000:
                    if(DEBUG)printf("%04X skip next if V[x] = kk\n",instruction);
                    if(DEBUG)printf("V[%X] = %X\n", x, V[x]);
                    if(V[x] == kk)
                        pc += 2;
                    break;
                    
                case 0x4000:
                    if(DEBUG)printf("%04X skip next if V[x] != kk\n",instruction);
                    if(DEBUG)printf("V[%X] = %X\n", x, V[x]);
                    if(V[x] != kk)
                        pc += 2;
                    break;
                    
                case 0x5000:
                    if(DEBUG)printf("%04X skip next if V[x] = V[y]\n",instruction);
                    if(V[x] == V[y])
                        pc += 2;                    
                    break;
                    
                case 0x6000:
                    if(DEBUG)printf("%04X set V[x] = kk\n",instruction);
                    V[x] = kk;
                    break;
                    
                case 0x7000:
                    if(DEBUG)printf("%04X Set V[x] = V[x] + kk\n",instruction);
                    V[x] += kk;
                    break;
                    
                case 0x8000:
                    if((instruction & 0xF) == 0x0)
                    {
                        if(DEBUG)printf("Set V[x] = V[y].\n");
                        V[x] = V[y];
                    }
                    else if((instruction & 0xF) == 0x1)
                    {
                        if(DEBUG)printf("Set V[x] or V[y].\n");
                        V[x] |= V[y];
                    }
                    else if((instruction & 0xF) == 0x2)
                    {
                        if(DEBUG)printf("Set V[x] and V[y].\n");
                        V[x] &= V[y];
                    }
                    else if((instruction & 0xF) == 0x3)
                    {
                        if(DEBUG)printf("Set V[x] xor V[y].\n");
                        V[x] ^= V[y];
                    }
                    else if((instruction & 0xF) == 0x4)
                    {
                        if(DEBUG)printf("Set V[x] add V[y].\n");
                        if(((unsigned short)V[x] + (unsigned short)V[y]) > 0xFF)
                            V[0xF] = 1;
                        else
                            V[0xF] = 0;
                        if(DEBUG)printf("before V[%X] = %X ,V[%X] = %X \n", x ,V[x],y,V[y]);
                        V[x] += V[y];
                        if(DEBUG)printf("after V[%X] = %X ,V[%X] = %X \n", x ,V[x],y,V[y]);
                        
                    }
                    else if((instruction & 0xF) == 0x5)
                    {
                        if(DEBUG)printf("Set V[x] sub V[y].\n");
                        if(V[x] < V[y])
                            V[0xF] = 0;  
                        else                        
                            V[0xF] = 1;  
                        if(DEBUG)printf("before V[%X] = %X ,V[%X] = %X \n", x ,V[x],y,V[y]);
                        V[x] -= V[y];
                        if(DEBUG)printf("after V[%X] = %X ,V[%X] = %X \n", x ,V[x],y,V[y]);
                                              
                    }
                    else if((instruction & 0xF) == 0x6)
                    {
                        if(DEBUG)printf("Set V[x] shf R V[y].\n");
                        if(V[x] & 0x1)
                            V[0xF] = 1;  
                        V[x] >>= 1;
                    }
                    else if((instruction & 0xF) == 0x7)
                    {
                        if(DEBUG)printf("Set V[y] - V[x] , no borrow\n");                        
                        if(V[x] < V[y])
                            V[0xF] = 1;  
                        V[x] = V[y] - V[x];
                    }
                    else if((instruction & 0xF) == 0xE)
                    {
                        if(DEBUG)printf("Set V[x] = V[x] SHL 1.\n");
                        if(V[x] & 0x80)
                            V[0xF] = 1;  
                        V[x] <<= 1;
                    }
                    else
                    {
                        if(DEBUG)printf("%04X unknown instruction\n",instruction);                                           
                    }
                    break;
                    
                case 0x9000:
                    if(DEBUG)printf("%04X Skip next instruction if V[x] != V[y].\n",instruction);
                    if(V[x] != V[y])
                        pc += 2;
                    break;
                    
                case 0xA000:
                    if(DEBUG)printf("%04X Set I = nnn.\n",instruction);
                    I = instruction & 0xFFF;
                    break;
                    
                case 0xB000:
                    if(DEBUG)printf("%04X Jump to location nnn + V0.\n",instruction);
                    pc = nnn + V[0];
                    break;
                    
                case 0xC000:
                    if(DEBUG)printf("%04X Set V[x] = random byte AND kk.\n",instruction);
                    V[x] = (rand() & 0xFF) & kk;
                    break;
                    
                case 0xD000:
                    if(DEBUG)printf("%04X Display n-byte sprite\n",instruction);
                    
                    for(int i = 0 ; i < n ; i++)
                    {
                        sprite[i] = memory[I + i];
                    }               
                    //calcSpriteXOR(V[x],V[y],sprite, n);                         
                    showSpriteXY(V[x],V[y],sprite, n);
                    //bar(10,10,20,20);
                    
                    break;
                    
                case 0xE000:
                    if((instruction & 0xFF) == 0x9E)
                    {
                        if(DEBUG)printf("%04X Skip next instruction if key with the value of V[x] is pressed\n",instruction);
                        keyin = getKeyhit();
                        if(keyin == V[x])
                        {
                            pc += 2;
                            if(DEBUG)printf("%X == %X \n",keyin , V[x]);
                        }
                    }
                    else if((instruction & 0xFF) == 0xA1)
                    {
                        if(DEBUG)printf("%04X Skip next instruction if key with the value of V[x] is not pressed\n",instruction);
                        keyin = getKeyhit();
                        if(keyin != V[x])
                        {
                            pc += 2;
                            if(DEBUG)printf("%X != %X \n",keyin , V[x]);
                        }
                    }
                    break;
                    
                case 0xF000:
                    if((instruction & 0xFF) == 0x07)
                    {
                        if(DEBUG)printf("%04X Set Vx = delay timer value.\n",instruction);
                        V[x] = DT;
                    }
                    else if((instruction & 0xFF) == 0x0A)
                    {
                        if(DEBUG)printf("%04X Wait for a key press, store the value of the key in Vx\n",instruction);
                        keyin = -1;
                        while(keyin == -1)
                        {   
                            keyin = getKeyhit();
                            if(DEBUG)printf("wait for key : %x\n" , keyin);
                            V[x] = keyin;                            
                        }
                    }
                    else if((instruction & 0xFF) == 0x15)
                    {
                        if(DEBUG)printf("%04X Set delay timer = Vx\n",instruction);
                        DT = V[x];
                    }
                    else if((instruction & 0xFF) == 0x18)
                    {
                        if(DEBUG)printf("%04X Set sound timer = Vx.\n",instruction);
                    }
                    else if((instruction & 0xFF) == 0x1E)
                    {
                        if(DEBUG)printf("%04X Set I = I + Vx.\n",instruction);
                        I += V[x];
                    }
                    else if((instruction & 0xFF) == 0x29)
                    {
                        if(DEBUG)printf("%04X Set I = location of sprite for digit Vx.\n",instruction);
                        I = 5 * V[x];
                    }
                    else if((instruction & 0xFF) == 0x33)
                    {
                        if(DEBUG)printf("%04X Store BCD representation of Vx in memory locations I, I+1, and I+2.\n",instruction);
                        memory[I + 0] = (V[x] / 100) % 100 ;
                        memory[I + 1] = (V[x] / 10)  % 10 ;
                        memory[I + 2] = V[x] % 10 ;
                    }
                    else if((instruction & 0xFF) == 0x55)
                    {
                        if(DEBUG)printf("%04X Store registers V0 through Vx in memory starting at location I.\n",instruction);
                        for(int i = 0 ; i <= x ; i++)
                        {
                            memory[I + i] = V[i];
                        }
                    }
                    else if((instruction & 0xFF) == 0x65)
                    {
                        if(DEBUG)printf("%04X Read registers V0 through Vx from memory starting at location I.\n",instruction);
                        for(int i = 0 ; i <= x ; i++)
                        {
                            V[i] = memory[I + i];
                        }
                    }
                    else 
                    {
                        if(DEBUG)printf("%04X unknow instruction\n",instruction);
                    }
                    break;
                    
                default:
                    if(DEBUG)printf("unknown instruction : %04X \n", instruction);
                    break;
            }
            
            
            
                   
        // debug window
        /*
        sprintf(tmp_string , "V[0] = %02X  " , V[0]);
        outtextxy(640 + 2 * SCREEN_OFFSET,SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[1] = %02X  " , V[1]);
        outtextxy(640 + 2 * SCREEN_OFFSET,20 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[2] = %02X  " , V[2]);
        outtextxy(640 + 2 * SCREEN_OFFSET,40 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[3] = %02X  " , V[3]);
        outtextxy(640 + 2 * SCREEN_OFFSET,60 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[4] = %02X  " , V[4]);
        outtextxy(640 + 2 * SCREEN_OFFSET,80 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[5] = %02X  " , V[5]);
        outtextxy(640 + 2 * SCREEN_OFFSET,100 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[6] = %02X  " , V[6]);
        outtextxy(640 + 2 * SCREEN_OFFSET,120 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[7] = %02X  " , V[7]);
        outtextxy(640 + 2 * SCREEN_OFFSET,140 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[8] = %02X  " , V[8]);
        outtextxy(640 + 2 * SCREEN_OFFSET,160 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[9] = %02X  " , V[9]);
        outtextxy(640 + 2 * SCREEN_OFFSET,180 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[A] = %02X  " , V[0xA]);
        outtextxy(640 + 2 * SCREEN_OFFSET,200 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[B] = %02X  " , V[0xB]);
        outtextxy(640 + 2 * SCREEN_OFFSET,220 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[C] = %02X  " , V[0xC]);
        outtextxy(640 + 2 * SCREEN_OFFSET,240 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[D] = %02X  " , V[0xD]);
        outtextxy(640 + 2 * SCREEN_OFFSET,260 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[E] = %02X  " , V[0xE]);
        outtextxy(640 + 2 * SCREEN_OFFSET,280 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "V[F] = %02X  " , V[0xF]);
        outtextxy(640 + 2 * SCREEN_OFFSET,300 + SCREEN_OFFSET + 320,tmp_string);
        
        sprintf(tmp_string , "PC = %04X " , pc);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,0 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "SP = %02X " , sp);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,20 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "I = %04X " , I);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,40 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "DT = %04X " , DT);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,60 + SCREEN_OFFSET + 320,tmp_string);
        
        
        sprintf(tmp_string , "memory[%04X] = %02X  " , I, memory[I]);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,80 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "memory[%04X] = %02X  " , I + 1, memory[I + 1]);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,100 + SCREEN_OFFSET + 320,tmp_string);
        sprintf(tmp_string , "memory[%04X] = %02X  " , I + 2, memory[I + 2]);
        outtextxy(640 + 100 + 2 * SCREEN_OFFSET,120 + SCREEN_OFFSET + 320,tmp_string);
      */
            
        }
    }
};
