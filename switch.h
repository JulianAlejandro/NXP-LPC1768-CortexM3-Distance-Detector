//INCOMPLETO

#ifndef SWITCH_H
#define SWITCH_H

#include "mbed.h"



extern bool sw_largo_msg;
extern bool sw_medio_msg;
extern bool sw_corto_msg;
extern bool sw_can_sleep;

void sw_fsm(void);

void sw_init(InterruptIn *sw);
#endif