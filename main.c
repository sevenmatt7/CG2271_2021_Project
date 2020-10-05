/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include <stdbool.h>

#include "led.h"
#include "music.h"

volatile bool *isStopped = &(bool){false};

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

void music_thread(void *argument) {
	start_theme();
	// TODO: loop this and check condition, if condition is met jump out directly to play the end_theme
	loop_theme();
	end_theme();
}

int main (void) {
 
	// System Initialization
	SystemCoreClockUpdate();
	init_led();
	init_music();

	osKernelInitialize();                 // Initialize CMSIS-RTOS
	osThreadNew(green_led_thread, NULL, NULL);   
	osThreadNew(red_led_thread, NULL, NULL);  
	osThreadNew(music_thread, NULL, NULL);  

	osKernelStart();                      // Start thread execution
	for (;;) {}
}
