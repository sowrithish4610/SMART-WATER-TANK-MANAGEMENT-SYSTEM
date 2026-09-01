#include<lpc21xx.h>
#include"p_header.h"
void delay_ms(unsigned int ms){
    T0PR = 15000-1;
    T0PC = 0;
    T0TC = 0;
    T0TCR = 1;
    while(T0TC < ms);
    T0TCR = 0;
}
void delay_us(unsigned int us) {
    unsigned int i, j;
    for(i=0; i<us; i++) {
        for(j=0; j<15; j++);
    }
}
