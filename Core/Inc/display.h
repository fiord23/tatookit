#ifndef DISPLAY_H
#define DISPLAY_H
#include "main.h"
#include "stm32l4xx_hal_spi.h"
#define cs_low()                HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_RESET) //PA9 LOW
#define cs_high()               HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_SET) //PA9 HIGH
#define display_data()          HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_SET) //PA10 HIGH
#define display_command()       HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_RESET) // PA10 LOW
#define display_reset_low()     HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_RESET) // PA8 LOW
#define display_reset_high()    HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_SET) // PA8 HIGH
#define display_power_low()     HAL_GPIO_WritePin(GPIOA, POWER_ON_OFF_Pin, GPIO_PIN_RESET) // P0 LOW
#define display_power_high()    HAL_GPIO_WritePin(GPIOA, POWER_ON_OFF_Pin, GPIO_PIN_SET) // PA0 HIGH
#define OLED_X_MAXPIXEL 128  //OLED width maximum memory 
#define OLED_Y_MAXPIXEL  72 //OLED height maximum memory


#define BLACK 0
#define WHITE 1

#define FONTHEADERSIZE    6

#define NORM        0
#define XOR         1

#define CLEAR_ALL         0
#define CLEAR_DISPLAY     1
#define CLEAR_BUFFER      2

#define MEMORYMODE          0x20
#define SETCOLUMN           0x21
#define SETROW              0x22
#define SETPORTRAIT         0x25
#define SETCONTRAST         0x81
#define SETCHARGEPMP1       0x8D
#define SETSEGREMAP         0xA0
#define SETSTARTLINE        0xA2
#define RESETALLON          0xA4
#define DISPLAYALLON        0xA5
#define RESETINVERT         0xA6
#define INVERTDISPLAY       0xA7
#define SETMULTIPLEX        0xA8
#define SETCHARGEPMP2       0xAC
#define SELECTIREF          0xAD
#define DISPLAYOFF          0xAE
#define DISPLAYON           0xAF
#define SETPRECHARGE        0xBC
#define SETGSTABLE          0xBE
#define SETDEFAULTTABLE     0xBF
#define COMSCANINC          0xC0
#define COMSCANDEC          0xC8
#define SETDISPLAYOFFSET    0xD3
#define SETDISPLAYCLOCKDIV  0xD5
#define SETPHASELENGTH      0xD9
#define SETCOMPINS          0xDA
#define SETVCOMDESELECT     0xDB
#define SETCOMMANDLOCK      0xFD

// Scroll - It's not documented in the SSD1320 doc but we
// guessed at it from the SSD1306 doc (see MicroOLED product).
#define ACTIVATESCROLL                0x2F
#define DEACTIVATESCROLL              0x2E
#define SETVERTICALSCROLLAREA         0xA3
#define RIGHTHORIZONTALSCROLL         0x26
#define LEFTHORIZONTALSCROLL          0x27
#define VERTICALRIGHTHORIZONTALSCROLL 0x29
#define VERTICALLEFTHORIZONTALSCROLL  0x2A


#define WIDTH 128/2
#define HEIGHT 72

void command (uint8_t command);
void command_bi (uint8_t command1, uint8_t command2);
void data (uint8_t data);
void display_init (void);
void OLED_ColorTurn(uint8_t i);
void er_oled_SetWindow(uint8_t Xstart, uint8_t Ystart, uint8_t Xend, uint8_t Yend);
void er_oled_clear(uint8_t a);
void er_oled_bitmap(const uint8_t * pBuf);
void setColumnAddress(uint8_t address);
void setRowAddress(uint8_t address) ;
void display_demo (void);



#endif