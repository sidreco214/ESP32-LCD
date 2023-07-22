/*
4비트 모드 확인 못해봄
아두이노 LiquidCrystal.h로 해봐도 안되는걸 보면, LCD가 지원안하거나 설정 방법이 다른 듯

create char도 확인 못해봄
아두이노 라이브러리를 사용해도 안되는 걸 보면, LCD가 지원안하거나 설정 방법이 다른 듯
*/

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
#define LCD_ENTRYRIGHT 0x02 //correct
#define LCD_ENTRYLEFT 0x00
#define LCD_ENTRYSHIFTON 0x01
#define LCD_ENTRYSHIFTOFF 0x00

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

#ifndef byte
typedef uint8_t byte;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t en_pin; //Enable pin, Falling edge on the pin, LCD execute command
    uint8_t rs_pin; //resistor selection 0: command, 1: resistor read/write
    uint8_t data_pins[8]; //databus pins
} lcd_pin_t;

typedef struct {
    uint8_t rows; //LCD's rows
    uint8_t cols; //LCD's columns
    lcd_pin_t pins; //lcd_pin_t struct

    byte _entryMod; //Entry Mode Set
    byte _displaycontrol; //Display on off blink control
    byte _functionSet; //8bit / 4bit, line, font size
    uint8_t _row_offsets[4]; //lcd memory address offsets
} lcd_handle_t;

/// @brief Write Charecter to LCD
/// @param lcd_handle lcd_handle_t struct
/// @param value value to write on databus
void lcd_write(lcd_handle_t lcd_handle, uint8_t value);
/// @brief Send command to LCD
/// @param lcd_handle lcd_handle_t struct
/// @param cmd command to send on databus
void lcd_command(lcd_handle_t lcd_handle, uint8_t cmd);

/// @brief initializes lcd_handle_t struct 
/// @param lcd_pin lcd_pin_t struct
/// @param cols LCD's columns
/// @param rows LCD's rows
/// @param Fourbits if true, Use 4bit operation
/// @return initialized lcd_handle_t struct
/// @todo Veryfy 4bit operation
lcd_handle_t lcd_init(lcd_pin_t lcd_pin, uint8_t cols, uint8_t rows, bool Fourbits);
/// @brief begin lcd
/// @param lcd_handle lcd_handle_t struct
void lcd_begin(lcd_handle_t* lcd_handle);

/// @brief clear LCD
/// @param lcd_handle lcd_handle_t struct
/// @details This command need time interval(roughtly 10ms)
void lcd_clear(lcd_handle_t lcd_handle);
/// @brief return cursor to home
/// @param lcd_handle 
void lcd_home(lcd_handle_t lcd_handle);

/// @brief set Cursor position(column, row)
/// @param lcd_handle lcd_handle_t struct
/// @param col LCD column
/// @param row LCD row
void lcd_setCursor(lcd_handle_t lcd_handle, uint8_t col, uint8_t row);
void lcd_printf(lcd_handle_t lcd_handle, const char* Format, ...);

void lcd_noDisplay(lcd_handle_t lcd_handle);
void lcd_Display(lcd_handle_t lcd_handle);
/// @brief hide cursor
/// @param lcd_handle lcd_handle_t struct
void lcd_noCursor(lcd_handle_t lcd_handle);
/// @brief show cursor
/// @param lcd_handle lcd_handle_t struct
void lcd_Cursor(lcd_handle_t lcd_handle);
/// @brief stop blinking cursor
/// @param lcd_handle 
void lcd_noBlink(lcd_handle_t lcd_handle);
/// @brief blink cursor
/// @param lcd_handle lcd_handle_t struct
void lcd_blink(lcd_handle_t lcd_handle);

/// @brief Create customized char in CGRAMADDR
/// @param lcd_handle lcd_handle_t struct
/// @param index 0~7 integer
/// @param charmap your char array (5*8 font)
/// @todo Veryfy Operation
void lcd_create_char(lcd_handle_t lcd_handle, uint8_t index, uint8_t charmap[8]);

#ifdef __cplusplus
}
#endif

#endif