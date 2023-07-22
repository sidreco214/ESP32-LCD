#include "lcd.h"

#include <string.h>
#include <stdarg.h>

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "esp_system.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"


//private
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

void lcd_pulse_enable(uint8_t en_pin);
void lcd_send_4bit(lcd_pin_t* lcd_pin, uint8_t value);
void lcd_send_8bit(lcd_pin_t* lcd_pin, uint8_t value);

void lcd_pulse_enable(uint8_t en_pin) {
    gpio_set_level(en_pin, 0);
    wait_us(1);
    gpio_set_level(en_pin, 1); 
    wait_us(1); // enable pulse must be >450ns
    gpio_set_level(en_pin, 0);
    wait_us(100); // commands need > 37us to settle
};

void lcd_send_4bit(lcd_pin_t* lcd_pin, uint8_t value) {
    for(int i=0; i<4; i++) {
        gpio_set_level(lcd_pin->data_pins[i], (value >> i) & 0x01);
    }
    lcd_pulse_enable(lcd_pin->en_pin);
};

void lcd_send_8bit(lcd_pin_t* lcd_pin, uint8_t value) {
    for(int i=0; i<8; i++) {
        gpio_set_level(lcd_pin->data_pins[i], (value >> i) & 0x01);
    }
    lcd_pulse_enable(lcd_pin->en_pin);
};

void lcd_write(lcd_handle_t lcd_handle, uint8_t value) {
    gpio_set_level(lcd_handle.pins.rs_pin, 1);
    if((lcd_handle._functionSet) & LCD_8BITMODE) {
        //8bit mode
        lcd_send_8bit(&(lcd_handle.pins), value);
    }
    else {
        //4bit mode
        lcd_send_4bit(&(lcd_handle.pins), value);
    }
};

void lcd_command(lcd_handle_t lcd_handle, uint8_t cmd) {
    gpio_set_level(lcd_handle.pins.rs_pin, 0);
    if((lcd_handle._functionSet) & LCD_8BITMODE) {
        //8bit mode
        lcd_send_8bit(&(lcd_handle.pins), cmd);
    }
    else {
        //4bit mode
        lcd_send_4bit(&(lcd_handle.pins), cmd);
    }
};

lcd_handle_t lcd_init(lcd_pin_t lcd_pin, uint8_t cols, uint8_t rows, bool Fourbits) {
    lcd_handle_t lcd_handle = {
        .cols = cols,
        .rows = rows,
        .pins = lcd_pin,

        ._entryMod = LCD_ENTRYMODESET | LCD_ENTRYRIGHT | LCD_ENTRYSHIFTOFF, //입력 후 커서 오른쪽 이동, 화면이동 없음
        ._displaycontrol = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF,
        ._functionSet = LCD_FUNCTIONSET | LCD_1LINE | LCD_5x8DOTS,
        ._row_offsets = {0x00, 0x40, 0x00 + cols, 0x40 + cols}
    };
    if(!Fourbits) lcd_handle._functionSet |= LCD_8BITMODE;       
    if(rows > 1) lcd_handle._functionSet |= LCD_2LINE; 
    return lcd_handle;
};

void lcd_begin(lcd_handle_t* lcd_handle) {
    //gpio 설정, TX2, RX2같은 특별한 핀은 os에서 pull이 설정되어 있는 듯, gpio_set_direction으로만 하니 문제됨
    gpio_config_t gpio_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };

    gpio_cfg.pin_bit_mask = (1 << lcd_handle->pins.rs_pin) | (1 << lcd_handle->pins.en_pin);
    for(int i=0; i< (((lcd_handle->_functionSet) & LCD_8BITMODE) ? 8 : 4); i++) {
        gpio_cfg.pin_bit_mask |= (1 << lcd_handle->pins.data_pins[i]);
    }
    if(gpio_config(&gpio_cfg) != ESP_OK) {
        ESP_LOGW("GPIO", "GPIO config failed");
        return;
    }

    wait_us(50000); //LCD에 전압 올라오는 데 시간 걸림
    gpio_set_level(lcd_handle->pins.rs_pin, 0);
    gpio_set_level(lcd_handle->pins.en_pin, 0);

    //8bit or 4bit 설정, functionset
    if(!((lcd_handle->_functionSet) & LCD_8BITMODE)) {
        lcd_command(*lcd_handle, 0x02); //4비트 모드 추가 명령
    }
    lcd_command(*lcd_handle, lcd_handle->_functionSet);

    //display control
    lcd_command(*lcd_handle, lcd_handle->_displaycontrol);

    lcd_clear(*lcd_handle);

    lcd_command(*lcd_handle, lcd_handle->_entryMod);
};

void lcd_clear(lcd_handle_t lcd_handle) {
    lcd_command(lcd_handle, LCD_CLEARDISPLAY);
    vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_home(lcd_handle_t lcd_handle) {
    lcd_command(lcd_handle, LCD_RETURNHOME);
    vTaskDelay(2 / portTICK_PERIOD_MS);
};

void lcd_setCursor(lcd_handle_t lcd_handle, uint8_t col, uint8_t row) {
    if(row > lcd_handle.rows) row = lcd_handle.rows -1;
    if(col > lcd_handle.cols) col = lcd_handle.cols -1;

    lcd_command(lcd_handle, LCD_SETDDRAMADDR | (col + lcd_handle._row_offsets[row]));
};

void lcd_printf(lcd_handle_t lcd_handle, const char* Format, ...) {
    char buf[81];
    va_list args;
    va_start(args, Format);
    vsnprintf(buf, 81, Format, args);
    for(int i=0; i<strlen(buf); i++) {
        lcd_write(lcd_handle, buf[i]);
    }
    va_end(args);
}

void lcd_noDisplay(lcd_handle_t lcd_handle) {
    lcd_handle._displaycontrol &= ~LCD_DISPLAYON;
    lcd_command(lcd_handle, lcd_handle._displaycontrol);
};

void lcd_Display(lcd_handle_t lcd_handle) {
    lcd_handle._displaycontrol |= LCD_DISPLAYON;
    lcd_command(lcd_handle, lcd_handle._displaycontrol);
};

void lcd_noCursor(lcd_handle_t lcd_handle) {
    lcd_handle._displaycontrol &= ~LCD_CURSORON;
    lcd_command(lcd_handle, lcd_handle._displaycontrol);
};

void lcd_Cursor(lcd_handle_t lcd_handle) {
    lcd_handle._displaycontrol |= LCD_CURSORON;
    lcd_command(lcd_handle, lcd_handle._displaycontrol);
};

void lcd_noBlink(lcd_handle_t lcd_handle) {
    lcd_handle._displaycontrol &= ~LCD_BLINKON;
    lcd_command(lcd_handle, lcd_handle._displaycontrol);
};

void lcd_blink(lcd_handle_t lcd_handle) {
    lcd_handle._displaycontrol |= LCD_BLINKON;
    lcd_command(lcd_handle, lcd_handle._displaycontrol);
};

void lcd_create_char(lcd_handle_t lcd_handle, uint8_t index, uint8_t charmap[8]) {
    index &= 0x07; //index는 8개 뿐이니 맨 앞비트 클리어해서 범위 조절
    lcd_command(lcd_handle, LCD_SETCGRAMADDR | (index << 3));
    for(int i=0; i<8; i++) lcd_write(lcd_handle, charmap[i]);
};