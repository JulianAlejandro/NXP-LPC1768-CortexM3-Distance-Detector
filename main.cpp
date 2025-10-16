#include "mbed.h"
#include "switch.h"
#include "control.h"
#include "hardware.h"
#include "medida.h"

void main(void) {
   hw_init();
	 sw_init(&sw);
	 medida_init(&TRIG, &ECHO);
	 ctrl_init(&dsr, &dsl, &display, &ledR, &ledL, &ldr);
	
  while(true) {
		sw_fsm();
		medida_fsm();
		ctrl_fsm();
		
		

   if(ctrl_can_sleep && medida_can_sleep && sw_can_sleep && !sw_largo_msg && !sw_medio_msg &&
    !sw_corto_msg && !medida_msg && !medida_error_msg && !medida_ok_msg && !medida_oor_msg){

__WFE();
   
		}
  } // while(true)
} // main()
