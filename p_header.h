#include<lpc21xx.h>


void delay_ms(unsigned int ms);
void delay_us(unsigned int us) ;
void uart0_init(unsigned int baud);
void uart0_tx(unsigned char data);
void uart0_tx_string(char *ptr);
void uart0_integer(int num);
void uart0_time_div(char t);
void i2c_init(void);
void i2c_write(unsigned char sa, unsigned char mr, unsigned char data);
unsigned char i2c_read(unsigned char sa, unsigned char mr);
void adc_init(void);
unsigned int adc_read(unsigned char ch_num)     ;
int smart_tank_adc_temp(void);
unsigned int smart_tank_water_level_percent(void);
void ultrasonic_init(void);
unsigned int ultrasonic_distance_cm(void)       ;
void servo_pwm_init(void)       ;
void system_pump_control(unsigned int water_percent);

