#include "lcd.h"

#include <string.h>

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "esp_timer.h"

#define NOP() __asm__ volatile ("nop")

void IRAM_ATTR wait_us(uint32_t us) {
    uint32_t t0 = esp_timer_get_time();
    if(us) {
        uint32_t t1 = t0 + us;
        if(t0 > t1) {
            //overflow 보상
            while(esp_timer_get_time() > t1) NOP();
        }
        while(esp_timer_get_time() < t1) NOP();
    }
};

void _pulse_enable(uint8_t en_pin) {
    gpio_set_level(en_pin, 0);
    wait_us(1);
    gpio_set_level(en_pin, 1); 
    wait_us(1); // enable pulse must be >450ns
    gpio_set_level(en_pin, 0);
    wait_us(100); // commands need > 37us to settle
}

void _send_4bit(lcd_handle_t *lcd_handle, uint8_t value) {
    for(int i=0; i<4; i++) {
        gpio_set_level(lcd_handle->data_pins[i], (value >> i) & 0x01);
    }
    _pulse_enable(lcd_handle->en_pin);
}
void _send_8bit(lcd_handle_t *lcd_handle, uint8_t value) {
    for(int i=0; i<8; i++) {
        gpio_set_level(lcd_handle->data_pins[i], (value >> i) & 0x01);
    }
    _pulse_enable(lcd_handle->en_pin);
}
void _send_byte(lcd_handle_t *lcd_handle, uint8_t value, uint8_t mode) {
    gpio_set_level(lcd_handle->rs_pin, mode); //Register Selection

    //만약 rw 핀이 설정되어 있다면, write로 설정
    if(lcd_handle->rw_pin != 255) gpio_set_level(lcd_handle->rw_pin, 0);

    if((lcd_handle->_displayfunction) & LCD_8BITMODE) {
        _send_8bit(lcd_handle, value);
    }
    else {
        _send_4bit(lcd_handle, (value >> 4)); //sends high nible
        _send_4bit(lcd_handle, value); //sends low nible
    }

}

lcd_handle_t lcd_init(uint8_t cols, uint8_t rows, bool fourBitsMode) {
    lcd_handle_t lcd_handle = {
        .rows = rows,
        .cols = cols,

        .rs_pin = 255,
        .rw_pin = 255,
        .en_pin = 255,
        .data_pins = {255},

        ._displayfunction = LCD_1LINE | LCD_5x8DOTS,
        ._row_offsets = {0x00, 0x40, 0x00 + cols, 0x40 + rows}
    };

    if(rows > 1) {
        lcd_handle._displayfunction |= LCD_2LINE;
    }

    if(fourBitsMode) {
        lcd_handle._displayfunction |= LCD_4BITMODE;
    }
    else {
        lcd_handle._displayfunction |= LCD_8BITMODE;
    }
    return lcd_handle;
};

void lcd_setDataPin(lcd_handle_t* lcd_handle, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
    lcd_handle->data_pins[0] = d0;
    lcd_handle->data_pins[1] = d1;
    lcd_handle->data_pins[2] = d2;
    lcd_handle->data_pins[3] = d3;
    lcd_handle->data_pins[4] = d4;
    lcd_handle->data_pins[5] = d5;
    lcd_handle->data_pins[6] = d6;
    lcd_handle->data_pins[7] = d7;
};

bool lcd_begin(lcd_handle_t *lcd_handle) {
    if(lcd_handle->en_pin == 255 || lcd_handle->rs_pin == 255) return false;

    //gpio_설정
    gpio_set_direction(lcd_handle->rs_pin, GPIO_MODE_OUTPUT);
    if(lcd_handle->rw_pin != 255) {
        gpio_set_direction(lcd_handle->rw_pin, GPIO_MODE_OUTPUT);
    }
    gpio_set_direction(lcd_handle->en_pin, GPIO_MODE_OUTPUT);

    for(int i=0; i<(lcd_handle->_displayfunction & LCD_8BITMODE) ? 8 : 4; i++) {
        gpio_set_direction(lcd_handle->data_pins[i], GPIO_MODE_OUTPUT);
    }

    vTaskDelay(50/portTICK_PERIOD_MS); //lcd 켜지고 40ms 이상의 대기가 필요

    gpio_set_level(lcd_handle->rs_pin, 0);
    if(lcd_handle->rw_pin != 255) {
        gpio_set_level(lcd_handle->rw_pin, 0);
    }
    gpio_set_level(lcd_handle->en_pin, 0);

    //lcd 8비트 or 4비트 모드 설정
    if(lcd_handle->_displayfunction & LCD_8BITMODE) {
        //Send function set command sequence
        _send_byte(lcd_handle, LCD_FUNCTIONSET | lcd_handle->_displayfunction, LCD_COMMAND);
        wait_us(4500); // wait more than 4.1ms

        //second try
        _send_byte(lcd_handle, LCD_FUNCTIONSET | lcd_handle->_displayfunction, LCD_COMMAND);
        wait_us(150);

        //third
        _send_byte(lcd_handle, LCD_FUNCTIONSET | lcd_handle->_displayfunction, LCD_COMMAND);
    }
    else {
        //4bit mode, 8비트에서 시작
        _send_4bit(lcd_handle, 0x03);
        wait_us(4500); // wait more than 4.1ms

        //second try
        _send_4bit(lcd_handle, 0x03);
        wait_us(4500); // wait more than 4.1ms

        //third
        _send_4bit(lcd_handle, 0x03);
        wait_us(150);

        _send_4bit(lcd_handle, 0x02);
    }
    _send_byte(lcd_handle, LCD_FUNCTIONSET | lcd_handle->_displayfunction, LCD_COMMAND);

    //백라이트, 커서, 커서 점멸 설정
    lcd_handle->_displaycontrol = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKOFF;
    _send_byte(lcd_handle, lcd_handle->_displaycontrol, LCD_COMMAND);
    _send_byte(lcd_handle, LCD_CLEARDISPLAY, LCD_COMMAND);

    //엔트리 설정
    lcd_handle->_displaymode = LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTINCREMENT;
    _send_byte(lcd_handle, lcd_handle->_displaymode, LCD_COMMAND);
    return true;
};