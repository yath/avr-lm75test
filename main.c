#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "common/common.h"

#define SLA_ADDR LM75_ADDR
#define SLA_R ((SLA_ADDR<<1)|TW_READ)

#define DEG "\xdf" /* ° */

#define msg(x) do { lcd_gotoxy(0, 1); lcd_puts_P(PSTR(x "             ")); } while(0)

const char *itoa(int i) {
    static char buf[12]; /* 2^32 = 4294967296 = 10 + sign + \0 */
    snprintf(buf, sizeof(buf), "%d", i);
    return buf;
}
uint8_t i2c_read_byte(int ack) {
    TWSR = 0;
    TWCR = (1<<TWINT)|(1<<TWEN)|(ack?(1<<TWEA):0);
    while (!(TWCR & (1<<TWINT)));
    DEBUG("TWSR: 0x%02x", TWSR);
    if (TWSR != 0x50)
        return 0;
    DEBUG("Received byte: 0x%02x\n", TWDR);
    return TWDR;
}

void i2c_start(void) {
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    while  (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != TW_START) {
        DEBUG("TWSR: %02x", TWSR);
        return;
    }

    TWDR = SLA_R;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    
    DEBUG("TWSR = %02x", TWSR);
    if ((TWSR & 0xF8) != TW_MR_SLA_ACK) {
        DEBUG("error");
        TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    } else {
        DEBUG("okay!");
    }
}

void i2c_stop(void) {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    while (TWCR & (1<<TWINT));
}
int main(void) {
    uart_init();
    debug_init();
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    DEBUG("init");

    msg("begin loop");
    while(1) {
        msg("i2c_start");
        i2c_start();
        msg("i2c_read_byte(1)");
        uint8_t temp = i2c_read_byte(1);
        DEBUG("get_temp(0): %d", temp);
        lcd_gotoxy(0, 0);
        lcd_puts_P(PSTR("Temp: "));
        lcd_puts(itoa(temp));
        lcd_puts_P(PSTR(DEG"C"));
        msg("i2c_read_byte(0)");
        i2c_read_byte(0);
        msg("i2c_stop");
        i2c_stop();
        msg("delay");
        _delay_ms(1000);
    }
}
