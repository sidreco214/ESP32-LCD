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
#define LCD_ENTRYRIGHT 0x02 //여기 수정함
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
    uint8_t en_pin; //en_pin 펄스의 상승 엣지에서 명령어 실행
    uint8_t rs_pin; //resistor selection 0: command, 1: resistor read/write
    uint8_t data_pins[8];
} lcd_pin_t;

typedef struct {
    uint8_t rows;
    uint8_t cols;
    lcd_pin_t pins;

    byte _entryMod; 
    byte _displaycontrol; //Display on off blink control
    byte _functionSet;
    uint8_t _row_offsets[4];
} lcd_handle_t;

void lcd_write(lcd_handle_t lcd_handle, uint8_t value);
void lcd_command(lcd_handle_t lcd_handle, uint8_t cmd);

lcd_handle_t lcd_init(lcd_pin_t lcd_pin, uint8_t cols, uint8_t rows, bool Fourbits);
void lcd_begin(lcd_handle_t* lcd_handle);

void lcd_clear(lcd_handle_t lcd_handle);
void lcd_home(lcd_handle_t lcd_handle);

void lcd_setCursor(lcd_handle_t lcd_handle, uint8_t col, uint8_t row);
void lcd_printf(lcd_handle_t lcd_handle, const char* Format, ...);

void lcd_noDisplay(lcd_handle_t lcd_handle);
void lcd_Display(lcd_handle_t lcd_handle);
void lcd_noCursor(lcd_handle_t lcd_handle);
void lcd_Cursor(lcd_handle_t lcd_handle);
void lcd_noBlink(lcd_handle_t lcd_handle);
void lcd_blink(lcd_handle_t lcd_handle);

void lcd_create_char(lcd_handle_t lcd_handle, uint8_t index, uint8_t charmap[8]);

#ifdef __cplusplus
}
#endif

#endif