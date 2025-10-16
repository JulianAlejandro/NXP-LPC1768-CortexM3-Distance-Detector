#include "control.h"
#include "switch.h"
#include "medida.h"
#include "to_7seg.h"

bool ctrl_can_sleep = true;

DigitalOut *ctrl_dsr_ptr;
DigitalOut *ctrl_dsl_ptr;

BusOut *ctrl_display_ptr;

DigitalOut *ctrl_ledR_ptr;
DigitalOut *ctrl_ledL_ptr;

AnalogIn *ctrl_ldr_ptr;

//////////////////////////////////
bool flagDisplayOn=false;
/////////////
bool TiempoReposo_evnt;
bool cuenta_evnt;
bool mostrarDisplay_evnt;
bool cambioDisplay_evnt;
bool ctrl_inicia;
int valor_luz;

Ticker tick_cambioDisplay;
Ticker TiempoLuz;

Timeout cuenta;
Timeout TiempoReposo;
Timeout mostrarDisplay;

void cambioDisplay_isr(void){cambioDisplay_evnt=true;}
void TiempoReposo_isr(void){TiempoReposo_evnt=true;}
void cuenta_isr(void){cuenta_evnt=true;}
void mostrarDisplay_isr(void){mostrarDisplay_evnt=true;}
void TiempoLuz_isr(void){valor_luz=99*ctrl_ldr_ptr->read();}

//bool cambioDisplay_evnt=true;


//FSM

void ctrl_fsm(void){
  typedef enum{APAGADO, REPOSO, MEDIDARAPIDA, MEDIA, MOSTRAR}estado_t;
	static estado_t estado = APAGADO;
	static int decenas;
	static int unidades;
	static int cnt;
	static int suma;
	static int media;
	static bool multiplex=true;
	static bool flagMostrarPR;
	static bool medidaPeriodica;
	static bool flagLuz = false;
	static int maximo=0;
	static int minimo=99;
	//bool flagPRUEBA=false;
	if(ctrl_inicia){
		//if(sw_largo_msg){*ctrl_ledL_ptr=1;}
		//if(sw_corto_msg){*ctrl_ledR_ptr=1;}
		//if(sw_medio_msg){}
		///////////////////////////////////////////////
		if(cambioDisplay_evnt && flagDisplayOn){
		   if(multiplex==true){
	       *ctrl_dsr_ptr=0;
			   *ctrl_display_ptr=to_7seg(decenas);
         *ctrl_dsl_ptr=1;
      		
	      }else{
	      
			   *ctrl_dsl_ptr=0;
			   *ctrl_display_ptr=to_7seg(unidades);
	       *ctrl_dsr_ptr=1;	
	      }
		 multiplex=!multiplex;
			
			
		 cambioDisplay_evnt=false;
		}else if(cambioDisplay_evnt && flagMostrarPR){
		      if(multiplex){
	        *ctrl_dsr_ptr=0;
			    *ctrl_display_ptr = 0x73;
          *ctrl_dsl_ptr=1;
      		
	       }else{
	      
			    *ctrl_dsl_ptr=0;
			    *ctrl_display_ptr=to_7seg(unidades);
	        *ctrl_dsr_ptr=1;	
	       }
		 multiplex=!multiplex;
			
			
		 cambioDisplay_evnt=false;
		}
		///////////////////////////////////////////////
				switch(estado){
			case (APAGADO):
				if((sw_largo_msg && !flagLuz) || (valor_luz>10 && flagLuz)){
					
					TiempoLuz.attach_us(TiempoLuz_isr, 1000000);
					//
					flagDisplayOn=true;
				  estado=REPOSO;
					//*ctrl_ledL_ptr=1;
				  decenas=11;
				  unidades=11;
				TiempoReposo.attach(TiempoReposo_isr,30);
					flagLuz=false;
				sw_largo_msg=false;
				}
//				else
//				if(99*(ctrl_ldr_ptr->read())>10){
//				 flagDisplayOn=true;
//				  estado=REPOSO;
//					//*ctrl_ledL_ptr=1;
//				  decenas=11;
//				  unidades=11;
//				//TiempoReposo.attach(TiempoReposo_isr,30);
//				}
				sw_corto_msg=false;
				sw_medio_msg=false;
				break;
			
			case (REPOSO):
				
				if(sw_corto_msg){
					estado=MEDIDARAPIDA;
					TiempoReposo.detach();
					medida_msg=true; //control envia mensaje a medidas
					*ctrl_ledR_ptr=1;
				
				sw_corto_msg=false;
				
				}else if(sw_medio_msg){
				  estado=MEDIA;
					TiempoReposo.detach();
					cuenta.attach_us(cuenta_isr,400000);//TIMEOUT
					cnt=0;
					suma=0;
					*ctrl_ledL_ptr=1;
					
					unidades=12;//r
					flagDisplayOn=false;
					flagMostrarPR=true;
				
					
					sw_medio_msg=false;
				}else if(sw_largo_msg){
				  estado=APAGADO;
					//
					flagDisplayOn=false;
					*ctrl_dsl_ptr=0;
					*ctrl_dsr_ptr=0;
					//
					TiempoReposo.detach();
					*ctrl_ledL_ptr=0;
					*ctrl_ledR_ptr=0;
					
					sw_largo_msg=false;
				}else if(TiempoReposo_evnt){
					estado=APAGADO;
					//
					flagDisplayOn=false;
					*ctrl_dsl_ptr=0;
					*ctrl_dsr_ptr=0;
					//
					TiempoReposo.detach();
					*ctrl_ledL_ptr=0;
					*ctrl_ledR_ptr=0;
					
					
				  TiempoReposo_evnt=false;
				}else 
				if(valor_luz <= 10){
				   estado=APAGADO;
					flagLuz=true;
					//
					flagDisplayOn=false;
					*ctrl_dsl_ptr=0;
					*ctrl_dsr_ptr=0;
					//
					TiempoReposo.detach();
					*ctrl_ledL_ptr=0;
					*ctrl_ledR_ptr=0;
					
					
				}
				medida_error_msg=false;
				medida_oor_msg=false;
				medida_ok_msg=false;
				break;
			
			case (MEDIDARAPIDA):
				if(medida_error_msg){
					estado=MOSTRAR;
					//TiempoReposo.attach(TiempoReposo_isr,30);
					unidades=12;//r
					decenas=14;//e
					mostrarDisplay.attach(mostrarDisplay_isr,5);
					//ledR=0;
					
					medida_msg=false;
					medida_error_msg=false;
				}else
				if(medida_oor_msg){
					estado=MOSTRAR;
					//TiempoReposo.attach(TiempoReposo_isr,30);
					unidades=12;//r
					decenas=0;//0
				  mostrarDisplay.attach(mostrarDisplay_isr,5);
					//ledR=0;
					medida_msg=false;
				medida_oor_msg=false;
				}else
				if(medida_ok_msg){
					estado=MOSTRAR;
					//TiempoReposo.attach(TiempoReposo_isr,30);
					decenas=valor_medida_msg/10;
					unidades=valor_medida_msg-decenas*10;
					mostrarDisplay.attach(mostrarDisplay_isr,5);
					//ledR=0;
					medida_msg=false;
				medida_ok_msg=false;
				}
					sw_corto_msg=false;
				  sw_largo_msg=false;
				  sw_medio_msg=false;
				break;
			case(MOSTRAR):
				if(mostrarDisplay_evnt && !medidaPeriodica){//timeout que funaliza la muestra del resultado en el desplay
					estado=REPOSO;
					TiempoReposo.attach(TiempoReposo_isr,30);
					
					unidades=11;//-
					decenas=11;//-
					*ctrl_ledR_ptr=0;
				
				mostrarDisplay_evnt=false;
				}else if(mostrarDisplay_evnt && medidaPeriodica){
				}
				  medida_msg=false;
					sw_corto_msg=false;
				  sw_largo_msg=false;
				  sw_medio_msg=false;
				break;
			
			default://MEDIA
				if(cuenta_evnt && cnt<9){
					estado=MEDIA;
				  cnt=cnt+1;
					
					medida_msg=true;	
         	
           if(valor_medida_msg>maximo){maximo=valor_medida_msg;}			
            if(valor_medida_msg<minimo){minimo=valor_medida_msg;}					 
					suma=suma+valor_medida_msg;
					
					
					//media=suma/9;
					
					cuenta_evnt=false;
					//cnt=cnt+1;
					cuenta.attach_us(cuenta_isr,400000);
				}else if(cnt>=9){
					//medida_msg=false;
					valor_medida_msg=0;
					cuenta.detach();
          flagMostrarPR=false;
					flagDisplayOn=true;
					media=suma/9;
					
				  estado=MOSTRAR;
				  decenas=media/10;
					unidades=media-decenas*10;
					media=0;
					suma=0;
					mostrarDisplay.attach(mostrarDisplay_isr,5);
					*ctrl_ledL_ptr=0;
				 
				}
				//valor_medida_msg=0;
				sw_corto_msg=false;
				sw_largo_msg=false;
				sw_medio_msg=false;
				
				break;
		
     }
				if(!sw_largo_msg && !sw_corto_msg && !sw_medio_msg && !TiempoReposo_evnt && 
       !medida_error_msg && !medida_oor_msg && !medida_ok_msg && !mostrarDisplay_evnt){
		   ctrl_can_sleep=true;
		 }
	
	}

}

void ctrl_init(DigitalOut *dsr, DigitalOut *dsl, BusOut *display,DigitalOut *ledR, DigitalOut *ledL, AnalogIn *ldr){
if(!ctrl_inicia){
	ctrl_inicia = true;
	ctrl_dsr_ptr=dsr;
	ctrl_dsl_ptr=dsl;
	ctrl_display_ptr=display;
  ctrl_ledL_ptr=ledL;
	ctrl_ledR_ptr=ledR;
	ctrl_ldr_ptr=ldr;
  tick_cambioDisplay.attach_us(cambioDisplay_isr,4000);
	
}


}