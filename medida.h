#ifndef MEDIDA_H
#define MEDIDA_H

#include "mbed.h"

extern bool medida_msg;
extern bool medida_error_msg;
extern bool medida_ok_msg;
extern bool medida_oor_msg;
extern int valor_medida_msg;
extern bool medida_can_sleep;


void medida_fsm(void);

void medida_init(DigitalOut *TRIG, InterruptIn *ECHO);

#endif