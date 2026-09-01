#include<lpc21xx.h>
#include"p_header.h"

#define SW1_MANUAL (1<<14)
#define SW2_AUTO   (1<<15)
#define SW3_PUMP   (1<<16)

static void eeprom_write_word(unsigned char sa, unsigned char base, unsigned int val){
    i2c_write(sa, base,     (unsigned char)(val & 0xFF));
    delay_ms(10);
    i2c_write(sa, base + 1, (unsigned char)((val >> 8) & 0xFF));
    delay_ms(10);
}

static unsigned int eeprom_read_word(unsigned char sa, unsigned char base){
    unsigned char lo = i2c_read(sa, base);
    delay_ms(5);
    unsigned char hi = i2c_read(sa, base + 1);
    delay_ms(5);
    return ((unsigned int)hi << 8) | lo;
}

int main(){
    unsigned int water_level, actual_temp;
    unsigned int distance_cm;
    unsigned char s, m, h, date, month, year;
    unsigned int mem_water, mem_temp, mem_dist;

    IODIR0 &= ~(SW1_MANUAL | SW2_AUTO | SW3_PUMP);

    VPBDIV = 0x00;

    uart0_init(9600);
    adc_init();
    ultrasonic_init();
    i2c_init();
    servo_pwm_init();

    i2c_write(0xD0, 0x00,0x00);
    delay_ms(10);
    i2c_write(0xD0, 0x01,0x05);
    delay_ms(10);
    i2c_write(0xD0, 0x02,0x16);
    delay_ms(10);
    i2c_write(0xD0, 0x04,0x18);
    delay_ms(10);
    i2c_write(0xD0, 0x05,0x08);
    delay_ms(10);
    i2c_write(0xD0, 0x06,0x26);
    delay_ms(10);

    uart0_tx_string("\r\n--- SMART WATER TANK SYSTEM ---\r\n");

    while(1){

        water_level  = smart_tank_water_level_percent();
        actual_temp  = smart_tank_adc_temp();
        distance_cm  = ultrasonic_distance_cm();

        s     = i2c_read(0xD1, 0x00); delay_ms(5);
        m     = i2c_read(0xD1, 0x01); delay_ms(5);
        h     = i2c_read(0xD1, 0x02); delay_ms(5);
        date  = i2c_read(0xD1, 0x04); delay_ms(5);
        month = i2c_read(0xD1, 0x05); delay_ms(5);
        year  = i2c_read(0xD1, 0x06); delay_ms(5);

        uart0_tx_string("Water Level: ");
        uart0_integer(water_level);
        uart0_tx_string("\r\n");
        delay_ms(50);

        uart0_tx_string("Distance    : ");
        uart0_integer(distance_cm);
        uart0_tx_string(" cm\r\n");
        delay_ms(50);

        uart0_tx_string("Temperature: ");
        uart0_integer(actual_temp);
        uart0_tx_string(" C\r\n");

        uart0_tx_string("Date: ");
        uart0_time_div(date); uart0_tx('/');
        uart0_time_div(month); uart0_tx_string("/20");
        uart0_time_div(year); uart0_tx_string("\r\n");

        uart0_tx_string("Current Time: ");
        uart0_time_div(h); uart0_tx(':');
        uart0_time_div(m); uart0_tx(':');
        uart0_time_div(s); uart0_tx_string("\r\n");

        system_pump_control(water_level);

        eeprom_write_word(0xA0, 0x07, water_level);
        eeprom_write_word(0xA0, 0x09, distance_cm);
        eeprom_write_word(0xA0, 0x20, actual_temp);

        mem_water = eeprom_read_word(0xA1, 0x07);
        mem_dist  = eeprom_read_word(0xA1, 0x09);
        mem_temp  = eeprom_read_word(0xA1, 0x20);

        uart0_tx_string("[ EEPROM READBACK ]\r\n");
        uart0_tx_string("Saved Water : "); uart0_integer(mem_water); uart0_tx_string(" %\r\n");
        uart0_tx_string("Saved Dist  : "); uart0_integer(mem_dist);  uart0_tx_string(" cm\r\n");
        uart0_tx_string("Saved Temp  : "); uart0_integer(mem_temp);  uart0_tx_string(" C\r\n");

        uart0_tx_string("----------------------\r\n");

        delay_ms(5000);
    }
}
