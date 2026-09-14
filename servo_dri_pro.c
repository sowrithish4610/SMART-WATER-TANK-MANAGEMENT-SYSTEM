#include<lpc21xx.h>
#include"p_header.h"
void servo_pwm_init(void) {
    PINSEL0 |= 0x00008000;

    PWMPR = 15 - 1;

    PWMMR0 = 20000;

    PWMMCR = (1<<1);
    PWMPCR = (1<<10);
    PWMTCR = (1<<0) | (1<<3);

    PWMMR2 = 1000;
    PWMLER = (1<<0) | (1<<2);
}

#define SW1_MANUAL (1<<14)
#define SW2_AUTO   (1<<15)
#define SW3_PUMP   (1<<16)

static int current_mode = 0;
static int manual_pump_state = 0;

static int last_sw1 = 1;
static int last_sw2 = 1;
static int last_sw3 = 1;

static int last_logged_mode = -1;
static int last_logged_pump = -1;

static void set_pump(int on){
    if(on)
    {
        PWMMR2 = 2000;
    }
    else
    {
        PWMMR2 = 1000;
    }
    PWMLER = (1<<2);
}

static void log_status(int mode, int pump_on){
    if(mode != last_logged_mode)
    {
        if(mode == 0) 
            uart0_tx_string("Mode: AUTO   | ");
        else          
            uart0_tx_string("Mode: MANUAL | ");
        last_logged_mode = mode;
        last_logged_pump = -1;
    }
    if(pump_on != last_logged_pump)
    {
        if(mode == 0)
        {
            if(pump_on) 
                uart0_tx_string("Pump: ON  (Filling)\r\n");
            else        
                uart0_tx_string("Pump: OFF (Full)\r\n");
        }
        else
        {
            if(pump_on) 
                uart0_tx_string("Pump: FORCED ON\r\n");
            else        
                uart0_tx_string("Pump: FORCED OFF\r\n");
        }
        last_logged_pump = pump_on;
    }
}

void system_pump_control(unsigned int water_percent) {
    int curr_sw1 = (IOPIN0 & SW1_MANUAL) ? 1 : 0;
    int curr_sw2 = (IOPIN0 & SW2_AUTO)   ? 1 : 0;
    int curr_sw3 = (IOPIN0 & SW3_PUMP)   ? 1 : 0;

    if (last_sw1 == 1 && curr_sw1 == 0) {
        current_mode = 1;
    }

    if (last_sw2 == 1 && curr_sw2 == 0) {
        current_mode = 0;
    }

    if (current_mode == 0) {
        int pump_on;
        if (water_percent < 10) {
            set_pump(1);
            pump_on = 1;
        }
        else if (water_percent > 90) {
            set_pump(0);
            pump_on = 0;
        }
        else {
            pump_on = (PWMMR2 >= 1500) ? 1 : 0;
        }
        log_status(0, pump_on);
    }
    else if (current_mode == 1) {
        if (last_sw3 == 1 && curr_sw3 == 0) {
            manual_pump_state = !manual_pump_state;
        }
        set_pump(manual_pump_state);
        log_status(1, manual_pump_state);
    }

    last_sw1 = curr_sw1;
    last_sw2 = curr_sw2;
    last_sw3 = curr_sw3;
}
