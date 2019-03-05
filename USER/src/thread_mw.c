#include "thread_mw.h"
#include "adc.h"
#include "usart2.h"
#include "usart3.h"
#include "jy901.h"

u16 time_x=0,time_xx=0,time_xxx=0,time_xxxx=0;		//¼ÇÂ¼hzÊý
u8 state=0;
u32 state_ST=0,state_HO=0,state_SW=0,state_HS=0,state_num=0,state_flag=0;
float sw_p,st_p,ho_p,hs_p;

#define ST 1
#define HO 2
#define SW 3
#define HS 4

/*
ST Õ¾Á¢×´Ì¬
HO ½Å¸úÀëµØ×´Ì¬
SW »Î¶¯×´Ì¬
HS ½Å¸ú×ÅµØ×´Ì¬
*/

void get_tick_time(u8 a){	
	rt_tick_t tick_temp;
  rt_uint8_t h=0,m=0,s=0;
	u16 temp_ms = 0;
	static rt_uint16_t printf_time = 0,rt_kprintf_time = 0;
	
		tick_temp = rt_tick_get();
		s=tick_temp/RT_TICK_PER_SECOND%60;
		m=tick_temp/RT_TICK_PER_SECOND/60%60;
		h=tick_temp/RT_TICK_PER_SECOND/60/60%24;
		temp_ms = tick_temp%RT_TICK_PER_SECOND;
	
	if(a){
		printf_time++;
		printf("\r\n\r\n%d:%d:%d.%d   %d\r\n",h,m,s,temp_ms,printf_time);
	} else{
		rt_kprintf_time++;
		rt_kprintf("\r\n\r\n%d:%d:%d.%d   %d\r\n",h,m,s,temp_ms,rt_kprintf_time);
	}
}

void get_adc(void* parameter){
	u16 P_f,P_b,P_m,temp=3900;

	while(1){
		time_xx++;
		Analog_v.ch8 = Get_Adc(ADC_CH4);		//12Î»
		Analog_v.ch7 = Get_Adc(ADC_CH5);
		Analog_v.ch6 = Get_Adc(ADC_CH6);
		Analog_v.ch5 = Get_Adc(ADC_CH7);
		Analog_v.ch4 = Get_Adc(ADC_CH8);
		Analog_v.ch3 = Get_Adc(ADC_CH9);
		Analog_v.ch2 = Get_Adc(ADC_CH10);
		Analog_v.ch1 = Get_Adc(ADC_CH11);
		
		P_f=(Analog_v.ch3+Analog_v.ch7+Analog_v.ch8)/3;
		P_m=(Analog_v.ch1+Analog_v.ch2)/2;
		P_b=(Analog_v.ch4+Analog_v.ch5+Analog_v.ch6)/3;

		if(P_f>temp&&P_m>temp&&P_b>temp){																										//SW×´Ì¬
				state=SW;		
				state_SW++;	
		}else if(P_f>temp&&P_m>temp&&P_b<=temp){																						//HS×´Ì¬
				state=HS;
				state_HS++;
				state_flag=1;
			}else if((P_m<=temp&&P_b<=temp)||(P_f<=temp&&P_m>temp&&P_b<=temp)){								//ST×´Ì¬				
				state=ST;	
				state_ST++;
				state_flag=1;
			}else{																																						//HO×´Ì¬	
				state=HO;
				state_HO++;
				state_flag=1;
			}
			
				state_num=state_ST+state_HO+state_SW+state_HS;
			
			if(state==SW&&state_flag&&state_num>1023){
				state_flag=0;
				get_tick_time(1);
//				printf("\r\nstate_ST=%d,state_HO=%d,state_SW=%d,state_HS=%d\r\n",state_ST,state_HO,state_SW,state_HS);
				sw_p=state_SW*100/state_num;
				st_p=state_ST*100/state_num;
				ho_p=state_HO*100/state_num;
				hs_p=state_HS*100/state_num;
//				printf("ALL=%d,SW=%5.2f,HS=%5.2f,ST=%5.2f,HO%5.2f\r\n",state_num,sw_p,hs_p,st_p,ho_p);
				if(sw_p>30&&sw_p<60){				
					if(hs_p<1&&ho_p>15){
						printf(" down ");
					}else if(hs_p<10){
						if(st_p>ho_p&&st_p-ho_p>30){
							printf(" up ");
						}else{
							printf(" pingdi ");
						}
					}
				}else{
//					printf("nuknow\r\n");
					printf("ALL=%d,SW=%5.2f,HS=%5.2f,ST=%5.2f,HO%5.2f\r\n",state_num,sw_p,hs_p,st_p,ho_p);
				}
				state_HS=0;
				state_HO=0;
				state_SW=0;
				state_ST=0;
			}
//			switch(state)  {
//				case SW: printf("°Ú¶¯\r\n");break;
//				case ST: printf("Õ¾Á¢\r\n");break;
//				case HO: printf("½Å¸úÀëµØ\r\n");break;
//				case HS: printf("½Å¸ú×ÅµØ\r\n");break;
//				}
//			printf("state_ST=%d,state_HO=%d,state_HS=%d,state_SW=%d\r\n",state_ST,state_HO,state_HS,state_SW);
//			printf("P_f=%d,p_m=%d,p_b=%d\r\n",P_f,P_m,P_b);

		rt_thread_delay(1);		
		rt_timer_check();
	}
	
}


void send_data(void* parameter){
	while(1){
		time_x++;
		
		rt_thread_delay(5);		
		rt_timer_check();
	}
	
}

void time_thread(void* parameter){
	
	while(1){
//		get_tick_time(0);
		rt_kprintf("send_hz=%d,meg_hz=%d,imu_rig_hz=%d,imu_error_hz=%d\r\n",time_x,time_xx,time_xxxx/4,time_xxx);
		time_x=0;
		time_xx=0;
		time_xxx=0;
		time_xxxx=0;
		rt_thread_delay(RT_TICK_PER_SECOND);
	}
	
}

