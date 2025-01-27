#ifndef DISPLAY_H
#define DISPLAY_H
#include "main.h"
#include "stm32l4xx_hal_spi.h"
#include "fonts.h"

#include "stdlib.h"
#include "string.h"

#define cs_low()                HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_RESET) //PA9 LOW
#define cs_high()               HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_SET) //PA9 HIGH
#define display_data()          HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_SET) //PA10 HIGH
#define display_command()       HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_RESET) // PA10 LOW
#define display_reset_low()     HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_RESET) // PA8 LOW
#define display_reset_high()    HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_SET) // PA8 HIGH
#define display_power_low()     HAL_GPIO_WritePin(GPIOA, POWER_ON_OFF_Pin, GPIO_PIN_RESET) // PA0 LOW
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
#define SETSEGREMAP         0xA1
#define SETSTARTLINE        0x40
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

#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            128
#endif
/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           32
#endif

#define SSD1306_X_SIZE           128
#define SSD1306_Y_SIZE           32
#define SSD1306_BUFFER_SIZE      (SSD1306_X_SIZE *  SSD1306_Y_SIZE) / 8

/**
 * @brief  SSD1306 color enumeration
 */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;




void command (uint8_t command);
void data (uint8_t data);
void display_init (void);
void Set_Page_Address(unsigned char add);
void Set_Column_Address(unsigned char add);
void Display_Picture(unsigned char pic[]);

void display_demo();

void SSD1306_UpdateScreen(void);
void SSD1306_ClearScreen();
void SSD1306_Fill(SSD1306_COLOR_t color); 
void SSD1306_ToggleInvert(void);
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);
void SSD1306_GotoXY(uint16_t x, uint16_t y);
char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);
char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color);
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c);
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);
void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);
void display_test(void);

#endif