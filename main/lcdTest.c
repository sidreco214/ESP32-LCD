#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"

#include "lcd.h"
#define LCD_ROWS 2
#define LCD_COLS 16

void app_main(void) {
    lcd_handle_t lcd_handle = lcd_init(LCD_COLS, LCD_ROWS, false);
    lcd_handle.en_pin = 0;
    lcd_handle.rs_pin = 15;
    lcd_setDataPin(&lcd_handle, 2, 4, 16, 17, 5, 18, 19, 21);
    if(!lcd_begin(&lcd_handle)) {
        printf("Failed to begining lcd\n");
        while(1) vTaskDelay(1000);
    }

    //test screen
    for(int i=0; i<LCD_ROWS; i++) for(int j=0; j<LCD_COLS; j++) {
        lcd_setCuror(&lcd_handle, i, j);
        lcd_print_char(&lcd_handle, '>');
        vTaskDelay(250 / portTICK_PERIOD_MS);
        lcd_clear(&lcd_handle);
    }

    //test printf
    lcd_setCuror(&lcd_handle, 0, 0);
    lcd_printf(&lcd_handle, "Hello %s", "world");
    vTaskDelay(2000 /portTICK_PERIOD_MS);

    lcd_setCuror(&lcd_handle, 0, 0);
    lcd_printf(&lcd_handle, "noCursor");
    lcd_noCursor(&lcd_handle);
    vTaskDelay(500 /portTICK_PERIOD_MS);
    lcd_clear(&lcd_handle);
    lcd_setCuror(&lcd_handle, 0, 0);
    lcd_printf(&lcd_handle, "cursor");
    lcd_cursor(&lcd_handle);
    vTaskDelay(500 /portTICK_PERIOD_MS);
    lcd_clear(&lcd_handle);

    lcd_setCuror(&lcd_handle, 0, 0);
    lcd_printf(&lcd_handle, "Cursor Blink");
    lcd_blink(&lcd_handle);
    vTaskDelay(500 /portTICK_PERIOD_MS);
    lcd_noBlink(&lcd_handle);
    lcd_clear(&lcd_handle);

    //create char
    uint8_t bell[8] = {0b00100,
                       0b01110,
                       0b01110,
                       0b01110,
                       0b11111,
                       0b00000,
                       0b00100,
                       0b00000
                       };
    lcd_create_char(&lcd_handle, 0, bell);
    lcd_setCuror(&lcd_handle, 0, 0);
    lcd_print_char(&lcd_handle, 0);
}
