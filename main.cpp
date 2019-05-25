#include <graphics.h>
#include "chip8.hpp"

void showSpriteXY(U8 x, U8 y, U8 *sprite , U8 n)
{
    U8 idx_x,idx_y;
    for(idx_y = 0; idx_y < n; idx_y++)
    {
        for(idx_x = 0; idx_x < 8 ; idx_x++)
        {
            if((sprite[idx_y] << idx_x) & 0x80)
            {
                bar((idx_x + x) * 10,(idx_y + y) * 10,(idx_x + x + 1) * 10,(idx_y + y + 1) * 10);
            }
        }   
    }
}
bool calcSpriteXOR(U8 x, U8 y, U8 *sprite , int height) 
{
    U8 idx_x,idx_y;
    for(idx_y = 0; idx_y < height; idx_y++)
    {
        for(idx_x = 0; idx_x < 8 ; idx_x++)
        {
            //printf("(%d,%d) : %d %d : ", x + idx_x, y + idx_y, ((sprite[idx_y] << idx_x) & 0x80) , getpixel((idx_x + x) * 10 + 3, (idx_y + y) * 10 + 3));
            if((((sprite[idx_y] << idx_x) & 0x80) > 0) && (getpixel((idx_x + x) * 10 + 3, (idx_y + y) * 10 + 3) > 0))
            {
                //printf("0\n");
                return false;
            }  
            else if((((sprite[idx_y] << idx_x) & 0x80) > 0) && (getpixel((idx_x + x) * 10 + 3, (idx_y + y) * 10 + 3) == 0))
            {
                //printf("1\n");
                return true;
            }
            else if((((sprite[idx_y] << idx_x) & 0x80) == 0) && (getpixel((idx_x + x) * 10 + 3, (idx_y + y) * 10 + 3) > 0))
            {
                //printf("1\n");
                return true;
            }
            else if((((sprite[idx_y] << idx_x) & 0x80) == 0) && (getpixel((idx_x + x) * 10 + 3, (idx_y + y) * 10 + 3) == 0))
            {
                //printf("0\n");
                return false;
            }           
            
            
            
            /*
            if(((sprite[idx_y] << idx_x) & 0x80) || getpixel((idx_x + x) * 10 + 3, (idx_y + y) * 10 + 3))
            {
                bar((idx_x + x) * 10,(idx_y + y) * 10,(idx_x + x + 1) * 10,(idx_y + y + 1) * 10);
            }*/
        }   
        //printf("\n--------------\n");
    }
}


int main(int argc, char *argv[] )
{

    initwindow(640 + 2 * SCREEN_OFFSET + SCREEN_DEBUG_WIDTH,  320 + 2 * SCREEN_OFFSET, "chip8_emulator",40,40);
    
    rectangle(SCREEN_OFFSET,SCREEN_OFFSET,640 + SCREEN_OFFSET, 320 + SCREEN_OFFSET);
    /*
    bar(0, 0, 10, 10);
    printf("color = %d\n" ,getpixel(5,5));
    printf("color = %d\n" ,getpixel(15,15));
    getchar();
    
    char keyin;
    while(true)
    {    
        keyin = getKeyhit();
        if(keyin != -1)
        {
            printf("%c\n",keyin);
        }
    }
    */
      
    CHIP8CPU chip8_cpu;
    chip8_cpu.readROM("Trip8.rom");
    chip8_cpu.initialze();    
    chip8_cpu.run();
    

    
    /*
    circle(100, 50, 40);
    
    U8 sprite[5] = {0x80,0x80,0x80,0x80,0x80};
    
    showSpriteXY(10, 10, sprite, 5);
    */
    

    return 0;
}
