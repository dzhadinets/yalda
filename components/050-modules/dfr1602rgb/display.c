#include "display.h"
#include <linux/i2c.h>
#include <linux/delay.h>

static struct i2c_adapter *i2c_adapter = NULL;
static struct i2c_client *i2c_lcd = NULL;
static struct i2c_client *i2c_rgb = NULL;

static u8 _showcontrol;
static u8 _showmode;

static void rgb_set_reg(u8 reg, u8 value);
static void lcd_command(u8 cmd);
static void lcd_write(u8 data);

static int display_i2c_probe(struct i2c_client *client,
			     const struct i2c_device_id *id);
static int display_i2c_remove(struct i2c_client *client);

#define DRIVER_NAME "dfr1602rgb"
#define SLAVE_NAME_LCD DRIVER_NAME "_lcd"
#define SLAVE_NAME_RGB DRIVER_NAME "_rgb"

enum { DFR_LCD,
       DFR_RGB,
};

/*
** Structure that has slave device id
*/
static const struct i2c_device_id display_i2c_ids[] = {
	{ SLAVE_NAME_LCD, DFR_LCD },
	{ SLAVE_NAME_RGB, DFR_RGB },
	{}
};
MODULE_DEVICE_TABLE(i2c, display_i2c_ids);

/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver i2c_driver = {
    .driver = {
        .name   = DRIVER_NAME,
        .owner  = THIS_MODULE,
    },
    .probe          = display_i2c_probe,
    .remove         = display_i2c_remove,
    .id_table       = display_i2c_ids,
};

int display_init(u8 busno)
{
	static struct i2c_board_info lcd_info = { I2C_BOARD_INFO(SLAVE_NAME_LCD,
								 LCD_ADDRESS) };

	static struct i2c_board_info rgb_info = { I2C_BOARD_INFO(SLAVE_NAME_RGB,
								 RGB_ADDRESS) };

	i2c_adapter = i2c_get_adapter(busno);
	if (i2c_adapter != NULL) {
		i2c_put_adapter(i2c_adapter);
	}

	i2c_lcd = i2c_new_client_device(i2c_adapter, &lcd_info);
	i2c_rgb = i2c_new_client_device(i2c_adapter, &rgb_info);

	return i2c_add_driver(&i2c_driver);
}

void display_deinit(void)
{
	i2c_unregister_device(i2c_lcd);
	i2c_unregister_device(i2c_rgb);
	i2c_del_driver(&i2c_driver);
}

static int display_i2c_probe(struct i2c_client *client,
			     const struct i2c_device_id *id)
{
	if (client->addr == LCD_ADDRESS) {
		///< this is according to the hitachi HD44780 datasheet
		///< page 45 figure 23

		///< Send function set command sequence
		lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE |
			    LCD_2LINE | LCD_5x8DOTS);
		udelay(50);

		///< second try
		lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE |
			    LCD_2LINE | LCD_5x8DOTS);
		udelay(50);

		///< third go
		lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE |
			    LCD_2LINE | LCD_5x8DOTS);

		///< turn the display on with no cursor or blinking default
		_showcontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

		lcd_command(LCD_DISPLAYCONTROL | _showcontrol);

		///< clear it off
		display_clear();

		///< Initialize to default text direction (for romance languages)
		///< set the entry mode
		_showmode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

		lcd_command(LCD_ENTRYMODESET | _showmode);
	} else if (client->addr == RGB_ADDRESS) {
		///< backlight init
		rgb_set_reg(REG_MODE1, 0);
		///< set LEDs controllable by both PWM and GRPPWM registers
		rgb_set_reg(REG_OUTPUT, 0xFF);
		///< set MODE2 values
		///< 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
		rgb_set_reg(REG_MODE2, 0x20);

		display_set_rgb(0xff, 0xff, 0xff);
	}

	return 0;
}

static int display_i2c_remove(struct i2c_client *client)
{
	if (client->addr == LCD_ADDRESS) {
		display_clear();
		display_enable_display(false);
	} else if (client->addr == RGB_ADDRESS) {
		display_set_color(0);
	}
	return 0;
}

void display_clear()
{
	// clear display, set cursor position to zero
	lcd_command(LCD_CLEARDISPLAY);
}

void display_enable_display(bool enable)
{
	if (enable) {
		_showcontrol |= LCD_DISPLAYON;
	} else {
		_showcontrol &= ~LCD_DISPLAYON;
	}
	lcd_command(LCD_DISPLAYCONTROL | _showcontrol);
}

void display_enable_cursor(bool enable)
{
	if (enable) {
		_showcontrol |= LCD_CURSORON;
	} else {
		_showcontrol &= ~LCD_CURSORON;
	}
	lcd_command(LCD_DISPLAYCONTROL | _showcontrol);
}

void display_enable_blink(bool enable)
{
	if (enable) {
		_showcontrol |= LCD_BLINKON;
	} else {
		_showcontrol &= ~LCD_BLINKON;
	}
	lcd_command(LCD_DISPLAYCONTROL | _showcontrol);
}

void display_autoscroll(bool enable)
{
	if (enable) {
		_showmode |= LCD_ENTRYSHIFTINCREMENT;
	} else {
		_showmode &= ~LCD_ENTRYSHIFTINCREMENT;
	}
	lcd_command(LCD_ENTRYMODESET | _showmode);
}

void display_set_cursor(u8 col, u8 row)
{
	col = (row == 0 ? col | 0x80 : col | 0xc0);
	lcd_command(col);
}

void display_home()
{
	lcd_command(LCD_RETURNHOME);
}

void display_set_color(u32 color)
{
	u8 b = (color >> 16) & 0xFF;
	u8 g = (color >> 8) & 0xFF;
	u8 r = color & 0xFF;
	display_set_rgb(r, g, b);
}

void display_set_rgb(u8 r, u8 g, u8 b)
{
	rgb_set_reg(REG_RED, r);
	rgb_set_reg(REG_GREEN, g);
	rgb_set_reg(REG_BLUE, b);
}

void display_print(const char text[])
{
	size_t i, len = strlen(text);
	for (i = 0; i < len; i++) {
		lcd_write(text[i]);
	}
}

void lcd_command(u8 cmd)
{
	u8 data[3] = { 0x80, cmd };
	i2c_master_send(i2c_lcd, data, 2);
}

void lcd_write(u8 ch)
{
	u8 data[3] = { 0x40, ch };
	i2c_master_send(i2c_lcd, data, 2);
}

void rgb_set_reg(u8 reg, u8 value)
{
	u8 data[3] = { reg, value };
	i2c_master_send(i2c_rgb, data, 2);
}
