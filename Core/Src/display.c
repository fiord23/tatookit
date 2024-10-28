
#include "display.h"
#include "stm32l4xx_hal_spi.h"


extern SPI_HandleTypeDef hspi1;

void command (uint8_t command)
{
    uint8_t tcommand = command;
    cs_low();
    display_command();
    HAL_SPI_Transmit(&hspi1, &tcommand, 1, 100);
    cs_high(); 

}

void command_bi (uint8_t command1, uint8_t command2)
{
    uint8_t tcommand[] = {command1, command2};
    display_command();
    cs_low();
    HAL_SPI_Transmit(&hspi1, tcommand, 2, 100);
    cs_high(); 

}

void data (uint8_t data)
{
    uint8_t tdata = data;
    cs_low();
    display_data();
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

    command(DISPLAYOFF);       // 0xAE - Display off

    command(SETDISPLAYCLOCKDIV); // 0xD5 - Clock divide ratio/osc. freq
    command(0xC2);                     // 0xC2 - osc clock=0xC divide ratio = 0x2

    command(SETMULTIPLEX); // 0xA8 - Multiplex ratio
    command(0x1F);                     // 0x1F - 31

    command(SETDISPLAYOFFSET); // 0xD3 - Display offset
    command(0x60);                          // 0x60 - 96

    command(SETSTARTLINE); // 0xA2 - Start line
    command(0x00);                      // 0x00 - Line 0

    command(SETSEGREMAP);  // 0xA0 - Segment re-map

    command(COMSCANINC); // 0xC0 - COM Output scan direction

    command(SETCOMPINS); // 0xDA - seg pins hardware config
    command(0x12);                       // 0x12 -

    command(SETCONTRAST);    // 0x81 - Contrast control
    command(0xFF);                       // 0x5A - value between 0x00 and 0xFF

    command(SETPHASELENGTH); // 0xD9 - Pre-charge period
    command(0x22);                       // 0x22

    command(SETVCOMDESELECT);   // 0xDB - VCOMH Deselect level
    command(0x30);                       // 0x30

    command(SELECTIREF);     // 0xAD - Internal IREF Enable
    command(0x10);                       // 0x10

    command(MEMORYMODE); // 0x20 - Memory addressing mode
    command(0x00);                        // 0x00 - Horizontal

  // disable internal charge pump
    command(SETCHARGEPMP1); // 0x8D - Internal charge pump
    command(0x01);                           // 0x01
    command(SETCHARGEPMP2); // 0xAC - Internal charge pump
    command(0x00);                           // 0x00

  // set entire display on/off
  //  command(RESETALLON);      // 0xA4 - Display on

  // set normal/inverse display
    command(RESETINVERT);  // 0xA6 - Normal display (not inverted)

  // display on
    command(DISPLAYON);         // 0xAF - Display on
    setColumnAddress(0);
    setRowAddress(0);

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

void setColumnAddress(uint8_t address) {
  command(SETCOLUMN); // Set column address
  command(address); //Set start address
  command(WIDTH - 1); //There are 160 pixels but each byte is 2 pixels. We want addresses 0 to 79.
  return;
}

void setRowAddress(uint8_t address) {
  command(SETROW); // Set row address
  command(address); //Set start address
  command(HEIGHT - 1); //Set end address: Display has 32 rows of pixels.
  return;
}