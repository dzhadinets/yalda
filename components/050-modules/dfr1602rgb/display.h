#ifndef HEADER_DISPLAY_H_
#define HEADER_DISPLAY_H_

#include <linux/types.h>

/*!
 *  @brief Device I2C Arress
 */
#define LCD_ADDRESS 0x3e
#define RGB_ADDRESS 0x60

/*!
 *  @brief color define
 */
#define WHITE 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define ONLY 3

#define REG_RED 0x04 // pwm2
#define REG_GREEN 0x03 // pwm1
#define REG_BLUE 0x02 // pwm0
#define REG_ONLY 0x02

#define REG_MODE1 0x00
#define REG_MODE2 0x01
#define REG_OUTPUT 0x08

/*!
 *  @brief commands
 */
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/*!
 *  @brief flags for display entry mode
 */
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02

#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/*!
 *  @brief flags for display on/off control
 */
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

/*!
 *  @brief flags for display/cursor shift
 */
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

/*!
 *  @brief flags for function set
 */
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

int display_init(u8 busno);
void display_deinit(void);
void display_clear(void);
void display_enable_display(bool enable);
void display_enable_cursor(bool enable);
void display_enable_blink(bool enable);
void display_autoscroll(bool enable);
void display_home(void);
void display_set_cursor(u8 col, u8 row);
void display_set_color(u32 color);
void display_set_rgb(u8 r, u8 g, u8 b);
void display_print(const char text[]);

#endif
