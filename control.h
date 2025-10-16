#ifndef CONTROL_H
#define CONTROL_H

#include "mbed.h"
extern bool ctrl_can_sleep;

void ctrl_fsm(void);

void ctrl_init(DigitalOut *dsr, DigitalOut *dsl, BusOut *display, DigitalOut *ledR, DigitalOut *ledL, AnalogIn *ldr);

#endif