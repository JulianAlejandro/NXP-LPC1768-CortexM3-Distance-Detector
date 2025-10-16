#ifndef HARDWARE_H
#define HARDWARE_H

#include "mbed.h"
//switches

extern InterruptIn sw;

//SE�AL TRIGUER
extern DigitalOut TRIG;

//SE�AL ECHO
extern InterruptIn ECHO;

//display
extern DigitalOut dsl;
extern DigitalOut dsr;

extern BusOut    display; 

//LEDS
extern DigitalOut ledR;
extern DigitalOut ledL;

extern AnalogIn ldr;


void hw_init(void);




#endif