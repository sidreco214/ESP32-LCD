#ifndef LCD_H
#define LCD_H

#include <inttypes.h>
#include <stdbool.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

//Modes for send_byte
#define LCD_CHARACTER  1
#define LCD_COMMAND    0

#define LCD_MAX_LOWS 4

#ifdef __cplusplus
extern "C" {
#endif

//구조체에 기본값을 할당해서 리턴하는 함수를 만들고, 핀을 설정하는 함수들 만들기

typedef struct {
    uint8_t rows; //number of rows
    uint8_t cols; //numver of columns

    uint8_t rs_pin; // LOW: command.  HIGH: character.
    uint8_t rw_pin; // LOW: write to LCD.  HIGH: read from LCD. 기본값 255
    uint8_t en_pin; // activated by a HIGH pulse.
    uint8_t data_pins[8];

    uint8_t _displaycontrol;
    uint8_t _displayfunction;
    uint8_t _displaymode;

    uint8_t _row_offsets[4];
} lcd_handle_t;

void lcd_send_byte(lcd_handle_t* lcd_handle, uint8_t value, uint8_t mode);

/*!
* @brief lcd_handle을 초기화 하는 함수
* @param cols number of columns
* @param rows number of rows
* @param fourBitsMode true of false
* @return lcd_handle, 반드시 init 함수로 초기화한 handle을 사용할 것
*/
lcd_handle_t lcd_init(uint8_t cols, uint8_t rows, bool fourBitsMode);
void lcd_setDataPin(lcd_handle_t* lcd_handle, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
bool lcd_begin(lcd_handle_t* lcd_handle);

void lcd_clear(lcd_handle_t* lcd_handle);
void lcd_home(lcd_handle_t* lcd_handle);
void lcd_setCuror(lcd_handle_t* lcd_handle, uint8_t row, uint8_t col);
void lcd_printf(lcd_handle_t* lcd_handle, const char* Format, ...);
void lcd_print_char(lcd_handle_t* lcd_handle, const char ch);

void lcd_noDisplay(lcd_handle_t* lcd_handle);
void lcd_display(lcd_handle_t* lcd_handle);
void lcd_noBlink(lcd_handle_t* lcd_handle);
void lcd_blink(lcd_handle_t* lcd_handle);
void lcd_noCursor(lcd_handle_t* lcd_handle);
void lcd_cursor(lcd_handle_t* lcd_handle);

void lcd_create_char(lcd_handle_t* lcd_handle, uint8_t location, uint8_t charmap[]);

#ifdef __cplusplus
}
#endif

#endif