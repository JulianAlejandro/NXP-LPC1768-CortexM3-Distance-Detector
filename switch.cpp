#include "switch.h"

//MENSAJES EXTERNOS QUE SE ENVIAN

bool sw_largo_msg;
bool sw_medio_msg;
bool sw_corto_msg;

bool sw_can_sleep = true;

//eventos, irqs, Timeouts

InterruptIn *boton_ptr;

bool sw_inicia;

bool rebote_evnt;
bool swr_evnt;
bool flagFall_evnt;
bool flagRise_evnt;
bool tiempo2seg_evnt;



Timeout rebote;
Timeout tiempo2seg;
Timer tiempo_swr;


void fall_isr(void){ flagFall_evnt=true;}
void rise_isr(void){ flagRise_evnt=true;}
void tiempo2seg_isr(void){tiempo2seg_evnt= true;}
void rebote_isr(void){rebote_evnt = true;}


//FSM

void sw_fsm(void) {
    typedef enum{S0,S1,S2,S3}estado_t;
	  static estado_t estado = S0;
 
 	  static int tiempo;

if(sw_inicia){
		switch(estado){
		
			case (S0):
			if(flagFall_evnt){
				boton_ptr->fall(NULL);
				rebote.attach_us(rebote_isr, 40000);
				estado = S1;
				flagFall_evnt = false;
				}else{
				 estado=S0;
				}
			
				
			  flagRise_evnt=false;
			  //rebote_evnt = false;
				//swr_evnt = false;
				//tiempo2seg_evnt= false;
				break;
			
			
			case(S1):
				if(rebote_evnt){
				  if(!boton_ptr->read()){
					 swr_evnt=true;
					 estado = S2;
					 // tiempo2seg.attach_us(tiempo2seg_isr, 2000000);
						//tiempo_swr.reset();
						//tiempo_swr.start();
						
					
					}else{
					 estado = S0;
						swr_evnt=false;
					}
             					
					
					boton_ptr->fall(fall_isr);
				 rebote_evnt=false;
				}
				//flagFall_evnt = false;
				//flagRise_evnt = false;
				//tiempo2seg_evnt = false;
				//swr_evnt = false;
				
				
				break;
				
			case (S2):
				if(swr_evnt){
				  tiempo2seg.attach_us(tiempo2seg_isr, 2000000);
					tiempo_swr.reset();
					tiempo_swr.start();
					estado = S3;
				swr_evnt=false;
				}
				
				//flagFall_evnt = false;
				//flagRise_evnt = false;
				//rebote_evnt = false;
			//	tiempo2seg_evnt = false;
				break;
			
			default:
				if(flagRise_evnt ){
					 
				  tiempo_swr.stop();
					tiempo = tiempo_swr.read_ms();
					if(tiempo < 500){
						estado = S0;
					  sw_corto_msg = true;
					  tiempo2seg.detach();}
					if(tiempo > 500 && tiempo < 2000){
						estado = S0;
				    sw_medio_msg = true;
					  tiempo2seg.detach();}
					
						flagRise_evnt=false;
				}else
				if(tiempo2seg_evnt ){
					  estado = S0;
            sw_largo_msg=true; 	
            tiempo2seg_evnt = false;	
            					
				}
				//flagFall_evnt = false;
				//flagRise_evnt = false;
				//swr_evnt = false;
				break;
				
		
		
		
		}

 if(!flagFall_evnt && !rebote_evnt && !swr_evnt && !flagRise_evnt && !tiempo2seg_evnt){
       	sw_can_sleep = true;
	}
	

  }
}

void sw_init(InterruptIn *sw){
   if(!sw_inicia){
	 sw_inicia = true;
		 
	 
	  boton_ptr = sw;
		 sw->fall(fall_isr);
	   sw->rise(rise_isr);
	 }



}






