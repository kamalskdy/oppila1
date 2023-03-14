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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS(cc2538_pwm_test, "cc2538 pwm test");
AUTOSTART_PROCESSES(&cc2538_pwm_test);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cc2538_pwm_test, ev, data)
{
	PROCESS_BEGIN();
//	int j;
	while(1){
	//	for(j=0;j<100;j++)
	//	{
		pwm_enable(160000,4, 0,1,0);	
		pwm_start(1,0,0,4);
//	 	printf("%d\n",j);
		clock_delay_usec(16777);
		pwm_enable(160000,50,0,1,0);
                pwm_start(1,0,0,4);
                clock_delay_usec(16777);

		pwm_enable(160000,100,0,1,0);
		pwm_start(1,0,0,4);
		clock_delay_usec(16777);
	//	}
	}
	PROCESS_END();
}
