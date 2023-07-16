#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"

#include "lcd.h"

void app_main(void) {
    lcd_handle_t lcd_handle = lcd_init(16, 2, false);
    lcd_handle.en_pin = 0;
    lcd_handle.rs_pin = 15;
    lcd_setDataPin(&lcd_handle, 2, 4, 16, 17, 5, 18, 19, 21);
    if(!lcd_begin(&lcd_handle)) {
        printf("Failed to begining lcd\n");
        while(1) sleep(1);
    }
}
