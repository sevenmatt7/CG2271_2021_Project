/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include <stdbool.h>

#include "led.h"

volatile bool *isStopped = &(bool){true};

void green_led_thread(void *argument) {
	while (true) {
		if (*isStopped) {
			on_green();  
		} 
		else {
			off_green();  // turn off all leds first in case switching from all on mode
			flash_green(isStopped);  // should stop immediately when the motor is stopped
		}
	}
}

void red_led_thread(void *argument) {
	while (true) {
		if (*isStopped) {
			flash_red_fast();
		} else {
			flash_red_slow();
		}
	}
}

int main (void) {
 
	// System Initialization
	SystemCoreClockUpdate();
	init_led();

	osKernelInitialize();                 // Initialize CMSIS-RTOS
	osThreadNew(green_led_thread, NULL, NULL);   
	osThreadNew(red_led_thread, NULL, NULL);  

	osKernelStart();                      // Start thread execution
	for (;;) {}
}
