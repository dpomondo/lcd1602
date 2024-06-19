//#include "lcd1602_RGB_Module.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdint.h>
//#include <sys/types.h>
#include "lcd1602_i2c.h"


static inline void _i2c_write_byte(lcd_12c_t *lcd, uint8_t data) {
    switch(i2c_write_blocking(lcd->i2c_i, lcd->address, &data, 1, false)) {
        case PICO_ERROR_GENERIC:
            printf("*****\n\tBad i2c write\n");
            break;
        default:
            break;
    };
}

static inline void _toggle_enable(lcd_12c_t *lcd, uint8_t data) {
#define LCD_TOGGLE_DISPLAY 600
    sleep_us(LCD_TOGGLE_DISPLAY);
    _i2c_write_byte(lcd, data | LCD_ENABLE_PIN);
    sleep_us(LCD_TOGGLE_DISPLAY);
    _i2c_write_byte(lcd, data & ~LCD_ENABLE_PIN);
    sleep_us(LCD_TOGGLE_DISPLAY);
}

static void _generic_lcd_write(lcd_12c_t *lcd, uint8_t data, uint8_t rs) {
    uint8_t command = rs | LCD_BACKLIGHT_ON;
    uint8_t high_nibble_plus = (data & 0xF0) | command;
    uint8_t low_nibble_plus = ((data << 4) & 0xF0) | command;

    /* _i2c_write_byte(lcd, high_nibble_plus); */
    _toggle_enable(lcd, high_nibble_plus);
    
    /* _i2c_write_byte(lcd, low_nibble_plus); */
    _toggle_enable(lcd, low_nibble_plus);
} // lcdGenericWrite

bool lcd_init(lcd_12c_t *lcd, i2c_inst_t *i2c_instance, uint8_t address, uint8_t width, uint8_t lines) {
    /* HD44780 datasheet page 46
     * init by instruction for 4-bit interface
     * */
    lcd->i2c_i          = i2c_instance;
    lcd->address        = address;
    lcd->width          = width;
    lcd->lines          = lines;
    lcd->memory_width   = 0x40; 

    sleep_ms(40);
    lcd_send_command(lcd, 0x03);
    sleep_ms(5);
    lcd_send_command(lcd, 0x03);
    sleep_us(1000);
    lcd_send_command(lcd, 0x03);
    sleep_us(1000);
    lcd_send_command(lcd, 0x02);

    lcd_send_command(lcd, LCD_FUNCTION_SET | LCD_NUMLINES_2);
    lcd_clear_screen(lcd);
    lcd_send_command(lcd, LCD_ENTRY_MODE_SET | LCD_CURSOR_INCREMENT);
    lcd_send_command(lcd, LCD_DISPLAY_CTRL | LCD_DISPLAY_ON);
    lcd_return_home(lcd);
    return 0;
}

void lcd_send_command(lcd_12c_t *lcd, uint8_t command) {
    _generic_lcd_write(lcd, command, LCD_COMMAND);
}

void lcd_put_char(lcd_12c_t *lcd, uint8_t character) {
    _generic_lcd_write(lcd, character, LCD_CHARACTER);
}

void lcd_put_string(lcd_12c_t *lcd, char *string) {
    char c;

    while ( (c = *string) ) {
        lcd_put_char(lcd, c);
        string++;
    }
}

void lcd_go_to(lcd_12c_t *lcd, uint8_t x, uint8_t y) {
    uint8_t command = LCD_DDRAM_ADDR | x;
    if (y > 0) {
        command |= lcd->memory_width;
    }
    lcd_send_command(lcd, command);
}
void lcd_return_home(lcd_12c_t *lcd) {
    lcd_send_command(lcd, LCD_RETURN_HOME);
}
void lcd_clear_screen(lcd_12c_t *lcd) {
    lcd_send_command(lcd, LCD_CLEAR_DISPLAY);
}

