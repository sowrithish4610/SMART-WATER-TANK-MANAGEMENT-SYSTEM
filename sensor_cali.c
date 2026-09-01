#include<lpc21xx.h>
#include"p_header.h"

#define TRIG (1<<12)
#define ECHO (1<<13)

int smart_tank_adc_temp(void){
    unsigned int adc_out_3;
    int vout,temp;

    adc_out_3 = adc_read(2);
    vout = (adc_out_3 * 3300) / 1023;
    temp = vout / 10;
    if(temp < 0) return 0;
    return temp;
}

unsigned int smart_tank_water_level_percent(void) {
    unsigned int adc_out_1 = adc_read(0);

    if (adc_out_1 < 120) return 0;
    if (adc_out_1 > 900) return 100;

    return ((adc_out_1 - 120) * 100) / (900 - 120);
}

void ultrasonic_init(void) {
    IODIR0 |= TRIG;
    IODIR0 &= ~ECHO;
    IOCLR0 = TRIG;
}

unsigned int ultrasonic_distance_cm(void) {
    unsigned int count=0, distance;
    IOCLR0 = TRIG;
    delay_us(2);
    IOSET0 = TRIG;
    delay_us(10);
    IOCLR0 = TRIG;
    while(!(IOPIN0 & ECHO));
    while(IOPIN0 & ECHO) {
        count++;
        delay_us(1);
    }
    distance = count / 58;
    return distance;
}
