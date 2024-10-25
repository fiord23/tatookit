
#include "display.h"
#include "stm32l4xx_hal_spi.h"


extern SPI_HandleTypeDef hspi1;

void command (uint8_t command)
{
    uint8_t tcommand = command;
    display_command();
    cs_low();
    HAL_SPI_Transmit(&hspi1, &tcommand, 1, 100);
    cs_high(); 

}

void data (uint8_t data)
{
    uint8_t tdata = data;
    display_data();
    cs_low();
    HAL_SPI_Transmit(&hspi1, &tdata, 1, 100);
    cs_high(); 

}

void display_init (void)
{
    display_power_low();
    cs_low();
    display_reset_high();
    HAL_Delay(10);
    display_reset_low();
    HAL_Delay(10);
    display_reset_high();
    HAL_Delay(10);
    display_power_high();

    command(0xae);//Set y Off
    command(0xD5);//Set Display Clock Divide Ratio/Oscillator Frequency
    command(0x11); 
    command(0xA8);//Set Multiplex Ratio 
    command(0x47); 
    command(0xA2);//Set Display Start Line
    command(0x00); 


    command(0xD3);//Set Display Offset
    command(0x74);
    command(0xa1);//Set COM Output Scan Direction
    command(0xC0); 

    command(0xDA);//SetSEGPinsHardwareConfiguration
    command(0x32);
	

    command(0x81);//Set Contrast Control
    command(0xdf);

    command(0xD9);//SSet Pre-Charge Priod
    command(0x72);
    command(0xDB);//Set VCOMH Deselect Level 
    command(0x20);
	
	
    command(0xAD);//Set Internal IREF Enable
    command(0x00);
    command(0xBC);
    command(0x1E); 
	
    command(0xA4);//Set Entire Display On/Off
    command(0xA6);
    command(0xaf);//Display on  

}

void OLED_ColorTurn(uint8_t i)
{
	if(i==0)
		{
		command(0xA6);
		}
	if(i==1)
		{
		command(0xA7);
		}
}

void er_oled_SetWindow(uint8_t Xstart, uint8_t Ystart, uint8_t Xend, uint8_t Yend)
{ 
  command(0x21);
  command(Xstart);
  command(Xend);
  command(0x22);
  command(Ystart);
  command(Yend);

}

void er_oled_clear(uint8_t a)
{int i,row;
  er_oled_SetWindow(0, 0, WIDTH-1, HEIGHT-1);
  for (row = 0; row < HEIGHT; row++) {              
        for(i = 0; i< WIDTH; i++ ) {
          data(a);// write data       
        }        
  }
}

void er_oled_bitmap(const uint8_t * pBuf)
{   
    uint8_t row,col; 
  er_oled_SetWindow(0, 0, WIDTH-1, HEIGHT-1);
  for (row = 0; row < HEIGHT; row++) {              
        for(col = 0;col<WIDTH; col++ ) 
        {
        data(*pBuf);
        * pBuf++;
        }
  }    	
}

