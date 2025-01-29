
#include "display.h"
#include <stdio.h>

#define VREF 3.3
#define ADC_RES 4095.0
#define VBAT_DIV 2.0



extern SPI_HandleTypeDef hspi1;
extern vbat_value;
extern char vbatbuf[3];
extern volatile uint16_t ADC_Data[3];
extern uint8_t speed;
/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = {0};
static uint8_t pixelBuffer[SSD1306_BUFFER_SIZE] = {0};

/* Private SSD1306 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

static SSD1306_t SSD1306;

void command (uint8_t command)
{
    uint8_t tcommand = command;
    cs_low();
    display_command();
    HAL_SPI_Transmit(&hspi1, &tcommand, 1, 100);
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
    //display_power_low();
    cs_low();
    display_reset_high();
    HAL_Delay(20);
    display_reset_low();
    HAL_Delay(20);
    display_reset_high();
    HAL_Delay(20);
    display_power_high();
    HAL_Delay(100);

    command(DISPLAYOFF); 

    command(SETDISPLAYCLOCKDIV); // 0x80 - Clock divide ratio/osc. freq
    command(0xF0);     

    command(SETMULTIPLEX); // 0xA8 - Multiplex ratio
    command(0x1F);                     // 0x1F

    command(SETDISPLAYOFFSET); // 0xD3 - Display offset
    command(0x00);                          //No offset

    command(SETCHARGEPMP1); // 0x8D - Display offset
    command(0x14);          //set(0x10) disable

    command(SETSTARTLINE); // 0x40 - set start line address

    command(RESETINVERT); // 0xA6 - set normal display

    command(RESETALLON); // 0xA4 - Disable Entire Display On

    command(SETSEGREMAP); // 0xA1 - set segment re-map 128 to 0  

    command(COMSCANDEC); // 0xC8 - Set COM Output Scan Direction 64 to 0

    command(SETCOMPINS); // 0xDA - seg pins hardware config
    command(0x42);                       // 0x12 -

    command(SETCONTRAST);    // 0x81 - set contrast control register
    command(0x80);           //

    command(SETPHASELENGTH); // 0xD9 - set pre-charge period
    command(0xF1);            

    command(SETVCOMDESELECT);   // 0xDB - set vcomh
    command(0x40);                       // 0x30


    command(DISPLAYON);         // 0xAF - Display on

      

    	/* Clear screen */
	  SSD1306_Fill(SSD1306_COLOR_WHITE);
	
	/* Update screen */
	  SSD1306_UpdateScreen();
	
	/* Set default values */
	  SSD1306.CurrentX = 0;
	  SSD1306.CurrentY = 0;

    SSD1306.Initialized = 1;

}

void SSD1306_UpdateScreen(void) {
  unsigned char i,j,num=0;
	for(i=0;i<0x04;i++)
	{
		Set_Page_Address(i);
  		Set_Column_Address(0x00);
  		for(j=0;j<0x80;j++)
		{
		  data(pixelBuffer[i*0x80+j]);
		}
	}
}

void SSD1306_ClearScreen()
{
  for (uint16_t i = 0; i < SSD1306_Buffer; i++)
  {
    pixelBuffer[i] = 0x00;
  }
  SSD1306_UpdateScreen();
}

void SSD1306_Fill(SSD1306_COLOR_t color) {
	/* Set memory */
  uint8_t i,j;
  uint8_t fill;
  if (color == SSD1306_COLOR_BLACK)
  fill = 0x00;
  else
  fill = 0xFF;
    for (uint16_t k = 0; k < SSD1306_BUFFER_SIZE; k++)
  {
    pixelBuffer[k] = fill;
  }

	for(i=0;i<0x04;i++)
	{
	Set_Page_Address(i);
  Set_Column_Address(0x00);
  for(j=0;j<0x80;j++)
		{
		  data(pixelBuffer[i*0x80+j]);
		}
	}


}

void SSD1306_ToggleInvert(void) {
	uint16_t i;
	
	/* Toggle invert */
	SSD1306.Inverted = !SSD1306.Inverted;
	
	/* Do memory toggle */
	for (i = 0; i < sizeof(pixelBuffer); i++) {
		pixelBuffer[i] = ~pixelBuffer[i];
	}
}

void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) {
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Error */
		return;
	}
	
	/* Check if pixels are inverted */
	if (SSD1306.Inverted) {
		color = (SSD1306_COLOR_t)!color;
	}
	
	/* Set color */
	if (color == SSD1306_COLOR_WHITE) {
		pixelBuffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		pixelBuffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

void SSD1306_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color) {
	uint32_t i, b, j;
	
	/* Check available space in LCD */
	if (
		SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}
	
	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);
			} else {
				SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);
			}
		}
	}
	
	/* Increase pointer */
	SSD1306.CurrentX += Font->FontWidth;
	
	/* Return character written */
	return ch;
}

char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (SSD1306_Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}
		
		/* Increase string pointer */
		str++;
	}
	
	/* Everything OK, zero should be returned */
	return *str;
}
 
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			SSD1306_DrawPixel(x0, i, c);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			SSD1306_DrawPixel(i, y0, c);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		SSD1306_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}

void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}
	
	/* Draw 4 lines */
	SSD1306_DrawLine(x, y, x + w, y, c);         /* Top line */
	SSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	SSD1306_DrawLine(x, y, x, y + h, c);         /* Left line */
	SSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	uint8_t i;
	
	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}
	
	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		SSD1306_DrawLine(x, y + i, x + w, y + i, c);
	}
}

void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	/* Draw lines */
	SSD1306_DrawLine(x1, y1, x2, y2, color);
	SSD1306_DrawLine(x2, y2, x3, y3, color);
	SSD1306_DrawLine(x3, y3, x1, y1, color);
}

void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		SSD1306_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawPixel(x0 + x, y0 + y, c);
        SSD1306_DrawPixel(x0 - x, y0 + y, c);
        SSD1306_DrawPixel(x0 + x, y0 - y, c);
        SSD1306_DrawPixel(x0 - x, y0 - y, c);

        SSD1306_DrawPixel(x0 + y, y0 + x, c);
        SSD1306_DrawPixel(x0 - y, y0 + x, c);
        SSD1306_DrawPixel(x0 + y, y0 - x, c);
        SSD1306_DrawPixel(x0 - y, y0 - x, c);
    }
}

void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);
    SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}

void Set_Page_Address(unsigned char add)
{
  add=0xb0|add;
  command(add);
	return;
}

void Set_Column_Address(unsigned char add)
{
  command((0x10|(add>>4)));
	command((0x0f&add));
	return;
}

void Display_Picture(unsigned char pic[])
{
  unsigned char i,j,num=0;
	for(i=0;i<0x04;i++)
	{
	Set_Page_Address(i);
  Set_Column_Address(0x00);
  for(j=0;j<0x80;j++)
		{
		  data(pic[i*0x80+j]);
		}
	}
  return;
}

void display_demo (void)
{

    unsigned char pic[]= 

/*--  ������һ��ͼ��E:\��ʾ��ͼƬ\ER-OLED091-3.bmp  --*/
/*--  ����x�߶�=128x32  --*/
{ 0xFF,0x01,0x01,0xFD,0x25,0x25,0x25,0x25,0x01,0xE1,0x59,0x45,0x59,0xE1,0x01,0x99,
0x25,0x25,0xC9,0x01,0x05,0x05,0xFD,0x05,0x05,0x01,0xFD,0x25,0x25,0x65,0x99,0x01,
0x01,0xFD,0x01,0x01,0x99,0x25,0x25,0xC9,0x01,0x01,0xFD,0x01,0x01,0xFD,0x19,0x21,
0xC1,0xFD,0x01,0x01,0xF9,0x05,0x05,0x25,0xE9,0x01,0x01,0x01,0x01,0x05,0x05,0xFD,
0x05,0x05,0x01,0xFD,0x25,0x25,0x25,0x25,0x01,0xF9,0x05,0x05,0x05,0x89,0x01,0x01,
0xFD,0x21,0x21,0x21,0xFD,0x01,0x01,0x01,0x01,0x01,0xF9,0x05,0x05,0x05,0x89,0x01,
0x01,0xF9,0x05,0x05,0x05,0xF9,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFD,0x01,
0x01,0x01,0x05,0x05,0xFD,0x05,0x05,0x01,0xFD,0x05,0x05,0x05,0xF9,0x01,0x01,0xFF,
0xFF,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xF1,0xF0,0x90,0x90,0x91,0x00,
0xF0,0xF1,0x10,0x10,0xF0,0xE1,0x01,0x00,0x00,0x00,0x01,0xC0,0xE0,0x31,0x10,0x30,
0xE0,0xC1,0x00,0xF0,0xF0,0x01,0x01,0x01,0x00,0x00,0xF0,0xF0,0x90,0x90,0x90,0x01,
0xF0,0xF0,0x10,0x11,0xF1,0xE1,0x01,0xE1,0xF0,0x10,0xF1,0xE1,0x01,0xE0,0xF0,0x10,
0xF1,0xE0,0x00,0xC0,0x61,0xF0,0xF0,0x01,0x00,0x00,0x00,0x01,0x01,0x21,0x30,0x90,
0xF0,0x60,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x03,0x00,0x00,0x01,0x01,
0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x80,0xC0,0xC0,0x00,0x00,0x80,0xC0,0x40,0xC0,0x80,0x00,0x80,0xC0,
0x40,0xC0,0x80,0x00,0x80,0xC0,0x80,0x00,0x80,0xC0,0x4F,0xCF,0x88,0x08,0x88,0xC0,
0x4F,0xCF,0x81,0x03,0x07,0x0C,0x08,0x02,0x02,0x02,0x00,0xC3,0xC7,0x0C,0x08,0x0C,
0x07,0x03,0x00,0x0F,0x0F,0x88,0xC8,0x08,0x08,0x00,0x0F,0x0F,0x08,0x08,0x08,0x00,
0x0F,0x0F,0x08,0x08,0x0F,0x07,0x80,0xC7,0x4F,0xC8,0x8F,0x07,0x00,0x04,0x0D,0x89,
0xCF,0x47,0xC0,0x80,0x00,0x0F,0x8F,0xC0,0xC0,0x02,0x42,0x42,0x00,0x04,0x0C,0x08,
0x0F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x80,0x83,0x81,0xBF,0xBF,0x80,0x80,0xB0,0xB8,0xAC,0xA7,0xA3,0x80,0x9D,0xBF,
0xA2,0xBF,0x9D,0x80,0x82,0x81,0x82,0x80,0x90,0xB0,0xA2,0xBF,0x9D,0x80,0xB0,0xB8,
0xAC,0xA7,0xA3,0x80,0x80,0x80,0x80,0x9E,0xBF,0xA1,0xA1,0xBF,0xBF,0x80,0x9E,0xBF,
0xA1,0xA1,0xBF,0x9E,0x81,0x9F,0xBF,0xA1,0x80,0x96,0xB7,0xAD,0xAD,0xBB,0x9A,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x9F,0xBF,0xA0,0xBF,0x9F,0x80,0xB0,0xB0,0x80,0x93,
0xB7,0xA4,0xBF,0x9F,0x80,0x83,0x81,0xBF,0xBF,0x80,0xBF,0xBF,0x80,0xBF,0xBF,0x81,
0x81,0xBF,0xBE,0x80,0x9E,0xBF,0xA1,0xB3,0x92,0x80,0xBF,0xBF,0x81,0x81,0xBF,0xBE,
0x80,0x9E,0xBF,0xA5,0xA5,0xB7,0x96,0x80,0x96,0xB7,0xAD,0xAD,0xBB,0x9A,0x80,0xFF
};
    Display_Picture(pic);
    HAL_Delay(1000);
    command(0xa7);
    HAL_Delay(1000);
    command(0xa6);

}

void display_test (void)
{
	SSD1306_Fill(SSD1306_COLOR_BLACK);
	SSD1306_DrawFilledRectangle(118, 28, 10, 3, SSD1306_COLOR_WHITE);
	SSD1306_DrawFilledRectangle(118, 22, 10, 3, SSD1306_COLOR_WHITE);
	SSD1306_DrawFilledRectangle(118, 16, 10, 3, SSD1306_COLOR_WHITE);
	SSD1306_DrawFilledRectangle(118, 10, 10, 3, SSD1306_COLOR_WHITE);
	SSD1306_DrawFilledRectangle(120, 7, 5, 2, SSD1306_COLOR_WHITE);
	SSD1306_GotoXY(70,5);
	SSD1306_Puts("120Hz", &Font_7x10, SSD1306_COLOR_WHITE);
	SSD1306_GotoXY(70,21);
	SSD1306_Puts("00:00h", &Font_7x10, SSD1306_COLOR_WHITE);
	SSD1306_GotoXY(0,5);
	SSD1306_Puts("6.2v", &Font_16x26, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
}

void show_vbat(void)
{
	uint8_t data = 0;
	    vbat_value = ((float)ADC_Data[0] * VREF * VBAT_DIV) / ADC_RES + 0.14;
    gcvt(vbat_value, 2, vbatbuf);
    if (vbat_value > 4.0)
      data = 4;
    else if ( (vbat_value > 3.7) && (vbat_value < 4.0) )
      data = 3;
    else if ( (vbat_value > 3.2) && (vbat_value < 3.7) )
     data = 2;
    else if (vbat_value < 3.2)  
       data = 1;
	
	switch (data)
	{
	case 1:
    	SSD1306_DrawFilledRectangle(120, 7,  5,  2, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 10, 10, 3, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 16, 10, 3, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 22, 10, 3, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 28, 10, 3, SSD1306_COLOR_WHITE);
		break;
	case 2:
    	SSD1306_DrawFilledRectangle(120, 7,  5,  2, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 10, 10, 3, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 16, 10, 3, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 22, 10, 3, SSD1306_COLOR_WHITE);
    	SSD1306_DrawFilledRectangle(118, 28, 10, 3, SSD1306_COLOR_WHITE);
		break;
	case 3:
    	SSD1306_DrawFilledRectangle(120, 7,  5,  2, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 10, 10, 3, SSD1306_COLOR_BLACK);
    	SSD1306_DrawFilledRectangle(118, 16, 10, 3, SSD1306_COLOR_WHITE);
    	SSD1306_DrawFilledRectangle(118, 22, 10, 3, SSD1306_COLOR_WHITE);
    	SSD1306_DrawFilledRectangle(118, 28, 10, 3, SSD1306_COLOR_WHITE);
		break;
	case 4:
    	SSD1306_DrawFilledRectangle(120, 7,  5,  2, SSD1306_COLOR_WHITE);
    	SSD1306_DrawFilledRectangle(118, 10, 10, 3, SSD1306_COLOR_WHITE);
    	SSD1306_DrawFilledRectangle(118, 16, 10, 3, SSD1306_COLOR_WHITE);
    	SSD1306_DrawFilledRectangle(118, 22, 10, 3, SSD1306_COLOR_WHITE);
    	SSD1306_DrawFilledRectangle(118, 28, 10, 3, SSD1306_COLOR_WHITE);
		break;
	default:
		break;
	}
}

void show_motor_duty (void)
{
	float data = ((float)speed / 63.0 ) * 9.0 * 10.0;
	char duty_data[4] = {'0', '.', '0', 'v'};
	duty_data[0] = (uint8_t)data / 10 + '0';
	duty_data[2] = (uint8_t)data % 10 + '0';
	SSD1306_GotoXY(0,5);
	SSD1306_Puts(duty_data, &Font_16x26, SSD1306_COLOR_WHITE);

}