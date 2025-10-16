#include "hardware.h"
#include "pinout.h"

//switches

InterruptIn sw(SWL_PIN);

//SEÑAL TRIGUER
DigitalOut TRIG(TRG_PIN);

//SEÑAL ECHO
InterruptIn ECHO(ECH_PIN);

//display
DigitalOut dsl(DSL_PIN);
DigitalOut dsr(DSR_PIN);

BusOut    display(SGA_PIN ,SGB_PIN ,SGC_PIN ,SGD_PIN, SGE_PIN, SGF_PIN ,SGG_PIN ); 

//LEDS
DigitalOut ledR(LDR_PIN);
DigitalOut ledL(LDL_PIN);

AnalogIn ldr(LIT_PIN);


void hw_init(void){
  sw.mode(PullUp);

}