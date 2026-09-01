#include<lpc21xx.h>
#include"p_header.h"

void i2c_init(void){

    PINSEL0 |= 0x50;
    I2CONSET = (1<<6);
    I2SCLH = I2SCLL = 75;
}


#define SI ((I2CONSET>>3)&1)


static void i2c_stop(void){
    I2CONSET = (1<<4);
    I2CONCLR = (1<<3);
    while((I2CONSET>>4)&1);
}


void i2c_write(unsigned char sa, unsigned char mr, unsigned char data){

    I2CONSET=(1<<5);
    I2CONCLR=(1<<3);
    while(SI==0);
    I2CONCLR=(1<<5);

    I2DAT=sa;
    I2CONCLR=(1<<3);
    while(SI==0);
    if(I2STAT==0x20){
        uart0_tx_string("Err: SA+W\r\n");
        i2c_stop();
        return;
    }

    I2DAT=mr;
    I2CONCLR=(1<<3);
    while(SI==0);
    if(I2STAT==0x30){
        uart0_tx_string("Err: Mem Addr\r\n");
        i2c_stop();
        return;
    }

    I2DAT=data;
    I2CONCLR=(1<<3);
    while(SI==0);
    if(I2STAT==0x30){
        uart0_tx_string("Err: Data\r\n");
        i2c_stop();
        return;
    }

    i2c_stop();
}


unsigned char i2c_read(unsigned char sa, unsigned char mr){

    unsigned char temp = 0;

    I2CONSET = (1<<5);
    I2CONCLR = (1<<3);
    while(SI==0);
    I2CONCLR = (1<<5);

    I2DAT = sa & 0xFE;
    I2CONCLR = (1<<3);
    while(SI==0);
    if(I2STAT==0x20){
        uart0_tx_string("Err: SA+W\r\n");
        i2c_stop();
        return temp;
    }

    I2DAT = mr;
    I2CONCLR = (1<<3);
    while(SI==0);
    if(I2STAT==0x30){
        uart0_tx_string("Err: Memory Addr\r\n");
        i2c_stop();
        return temp;
    }

    I2CONSET = (1<<5);
    I2CONCLR = (1<<3);
    while(SI==0);
    I2CONCLR = (1<<5);

    I2DAT = sa;
    I2CONCLR = (1<<3);
    while(SI==0);
    if(I2STAT==0x48){
        uart0_tx_string("Err: SA+R\r\n");
        i2c_stop();
        return temp;
    }

    I2CONCLR = (1<<2);
    I2CONCLR = (1<<3);
    while(SI==0);
    temp = I2DAT;
    i2c_stop();

    I2CONSET = (1<<2);

    return temp;
}
