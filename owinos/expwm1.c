#include "contiki.h"
#include "cpu.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/sys-ctrl.h"
#include "pwm.h"
#include "lpm.h"
#include "dev/ioc.h"
#include <stdio.h>
#include <stdint.h>
#include "sys/clock.h"

/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static uint8_t pwm_en[1];
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(cc2538_pwm_test, "cc2538 pwm test");
AUTOSTART_PROCESSES(&cc2538_pwm_test);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cc2538_pwm_test, ev, data)
{
	PROCESS_BEGIN();
	memset(pwm_en, 0,1);

	PRINTF("\nStarting the test\n");
	int j;
	while(1) 
	{
		for(j = 1; j < 100; j++) 
	         {
			if(pwm_enable(160000,j, 0,1,0) == PWM_SUCCESS)
		       	{
				pwm_en[0] = 1;
				PRINTF("%s (%u) configuration OK\n", gpt_name(1),0);
                                
			}

				if((pwm_en[0]) && (pwm_start(1,0,0,4) != PWM_SUCCESS)) 
                                 PRINTF(" successfully  started \n");
				  clock_delay_usec(90000);
			
		
	          }


	}

	PROCESS_END();
}
