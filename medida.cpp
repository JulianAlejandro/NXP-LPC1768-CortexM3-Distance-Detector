#include "medida.h"


bool medida_msg;
bool medida_error_msg;
bool medida_ok_msg;
bool medida_oor_msg;
int valor_medida_msg;
bool medida_can_sleep;


DigitalOut *medida_TRIG_ptr;
InterruptIn *medida_ECHO_ptr;


bool medida_inicia;

bool pulsoTriguer_evnt;
bool subidaEcho_evnt;
bool bajadaEcho_evnt;
bool tiempoECHO_evnt;


Timeout pulsoTriguer;
Timeout tiempoECHO;
Timer tiempo;


void pulsoTriguer_isr(void){pulsoTriguer_evnt=true;}
void subidaEcho_isr(void){subidaEcho_evnt=true;}
void bajadaEcho_isr(void){bajadaEcho_evnt=true;}
void tiempoECHO_isr(void){tiempoECHO_evnt=true;}
   int distCm;	
   int tiempo_us;

//FSM

void medida_fsm(void){
	typedef enum{S0,S1,S2,S3}estado_t;
	static estado_t estado = S0;
	
	if(medida_inicia){
	  	switch(estado){
			case(S0):
				if(medida_msg){
					*medida_TRIG_ptr=1;
				  pulsoTriguer.attach_us(pulsoTriguer_isr,1000);	
				
					medida_msg=false;
					estado=S1;
				}
			   pulsoTriguer_evnt=false;
				 subidaEcho_evnt=false;
				
				break;
				
			case(S1):
				if(pulsoTriguer_evnt){
				*medida_TRIG_ptr=0; 
			  tiempoECHO.attach_us(tiempoECHO_isr ,40000);//revisar este valor
				estado=S2;
					medida_ECHO_ptr->fall(bajadaEcho_isr);
					
					pulsoTriguer_evnt=false;
				}
				medida_msg=false;
				break;
				
			case(S2):
				if(tiempoECHO_evnt && !subidaEcho_evnt){
		   //mensaje de error, algo no esta funcionando
					 estado=S0;
					
					 valor_medida_msg=0;
			     medida_error_msg=true;
					 
					//ECHO.rise(NULL);
					tiempoECHO.detach();
					tiempoECHO_evnt=false;
					subidaEcho_evnt=false;
		    }else if(subidaEcho_evnt && !tiempoECHO_evnt){
		      
						tiempo.reset();
			      tiempo.start();
			      tiempoECHO.detach();
					  estado=S3;
						
					tiempoECHO.detach();
					tiempoECHO_evnt=false;
					subidaEcho_evnt=false;		
		     }
				bajadaEcho_evnt=false;
				pulsoTriguer_evnt=false;
				 medida_msg=false;
				break;
				 
			default:
				if(bajadaEcho_evnt){
		      tiempo.stop();
		      tiempo_us = tiempo.read_us();
				  distCm=tiempo_us/58;
					//valorDistancia_msg=distCm;	
					
			   if(distCm>99){
				/*mensaje out of range*/
			  	 estado=S0;
					 valor_medida_msg=0;	
				   medida_oor_msg=true;
				
         }else{  /*mensaje de que la medida fue correcta y */
					 valor_medida_msg=distCm;	
			  	 medida_ok_msg=true;
           
           estado=S0;				 
			 }
				//medida_ECHO_ptr->fall(NULL);
			  medida_msg=false;
			  bajadaEcho_evnt=false;
		}
		   break;
		
		
		}
	
	if(!medida_msg && !pulsoTriguer_evnt && !bajadaEcho_evnt  ){
					  medida_can_sleep=true;
					}
	}

}

void medida_init(DigitalOut *TRIG, InterruptIn *ECHO) {
  if(!medida_inicia) {           
    medida_inicia = true;
    medida_TRIG_ptr = TRIG; //<-----------------------------------------------------  
    //*TRIG = 0;              
		medida_ECHO_ptr = ECHO;
    ECHO->rise(subidaEcho_isr); 
		ECHO->fall(NULL);
  }
}


