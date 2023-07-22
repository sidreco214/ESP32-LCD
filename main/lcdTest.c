/*
4비트 모드 확인 못해봄
아두이노 LiquidCrystal.h로 해봐도 안되는걸 보면, LCD가 지원안하거나 설정 방법이 다른 듯

create char도 확인 못해봄
아두이노 라이브러리를 사용해도 안되는 걸 보면, LCD가 지원안하거나 설정 방법이 다른 듯
*/

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"

#include "lcd.h"
#define LCD_ROWS 2
#define LCD_COLS 16

void app_main(void) {
    lcd_pin_t lcd_pin = {
        .en_pin = 4,
        .rs_pin = 15,
        .data_pins = {16, 17, 5, 18, 19, 21, 22, 23}
    };
    lcd_handle_t lcd_handle = lcd_init(lcd_pin, LCD_COLS, LCD_ROWS, false);
    lcd_begin(&lcd_handle);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    for(int i=0; i<3; i++) {
        lcd_setCursor(lcd_handle, 0, 0);
        lcd_printf(lcd_handle, "Hello %d", i+1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        lcd_clear(lcd_handle);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "ESP32 LCD");
    lcd_setCursor(lcd_handle, 0, 1);
    lcd_printf(lcd_handle, "setCursor Test");
    vTaskDelay(2500 / portTICK_PERIOD_MS);
    lcd_clear(lcd_handle);
    vTaskDelay(10 / portTICK_PERIOD_MS); //clear 명령은 실행하는데 시간이 걸림

    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "Screen Test");
    vTaskDelay(2500 / portTICK_PERIOD_MS);
    
    for(int i=0; i<LCD_ROWS; i++) for(int j=0; j<LCD_COLS; j++) {
        lcd_setCursor(lcd_handle, j, i);
        lcd_write(lcd_handle, '>');
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    lcd_clear(lcd_handle);
    vTaskDelay(10 / portTICK_PERIOD_MS); //clear 명령은 실행하는데 시간이 걸림
  
    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "Test noDisplay");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    lcd_noDisplay(lcd_handle);
    lcd_clear(lcd_handle);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "Display");
    lcd_Display(lcd_handle);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    lcd_clear(lcd_handle);
    vTaskDelay(10 / portTICK_PERIOD_MS); //clear 명령은 실행하는데 시간이 걸림

    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "Cursor");
    lcd_Cursor(lcd_handle);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    lcd_clear(lcd_handle);
    vTaskDelay(10 / portTICK_PERIOD_MS); //clear 명령은 실행하는데 시간이 걸림

    lcd_noCursor(lcd_handle);
    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "noCursor");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    lcd_clear(lcd_handle);
    vTaskDelay(10 / portTICK_PERIOD_MS); //clear 명령은 실행하는데 시간이 걸림

    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "Blink");
    lcd_blink(lcd_handle);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear(lcd_handle);
    vTaskDelay(10 / portTICK_PERIOD_MS); //clear 명령은 실행하는데 시간이 걸림

    lcd_noBlink(lcd_handle);
    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "noBlink");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear(lcd_handle);
    vTaskDelay(10 / portTICK_PERIOD_MS); //clear 명령은 실행하는데 시간이 걸림

    lcd_setCursor(lcd_handle, 0, 0);
    lcd_printf(lcd_handle, "Create char: ");
    uint8_t bell[8] = {0b00100,
                       0b01110,
                       0b01110,
                       0b01110,
                       0b11111,
                       0b00000,
                       0b00100,
                       0b00000
                       };
    lcd_create_char(lcd_handle, 0, bell);
    lcd_write(lcd_handle, 0);
}
