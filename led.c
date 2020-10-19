#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include <stdbool.h>

#include "led.h"

const color_t green_leds[] = {Green1, Green2, Green3, Green4, Green5, Green6, Green7, Green8};
const int green_led_num = sizeof(green_leds)/sizeof(green_leds[0]);

void init_led(void) {
	// Enable Clock to PORTC and PORTE
	SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTE_MASK);
	
	// Configure MUX settings to make the pins GPIO
	PORTC->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_1] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_2] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_2] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_3] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_3] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_4] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_4] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_5] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_5] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_6] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_6] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_7] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_7] |= PORT_PCR_MUX(1);
	PORTE->PCR[GREEN_LED_8] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_8] |= PORT_PCR_MUX(1);
	
	// Set the pins to output
	PTC->PDDR |= MASK(RED_LED);
	PTE->PDDR |= (MASK(GREEN_LED_1) | MASK(GREEN_LED_2) | MASK(GREEN_LED_3) | MASK(GREEN_LED_4) 
				| MASK(GREEN_LED_5) | MASK(GREEN_LED_6) | MASK(GREEN_LED_7) | MASK(GREEN_LED_8));
}

void flash_led(color_t color) {
	// The LEDs are active high
	switch (color) {
		case Red:
			GPIOC_PDOR |= MASK(RED_LED);
			break;
		case Green1:
			GPIOE_PDOR |= MASK(GREEN_LED_1);
			break;
		case Green2:
			GPIOE_PDOR |= MASK(GREEN_LED_2);
			break;
		case Green3:
			GPIOE_PDOR |= MASK(GREEN_LED_3);
			break;
		case Green4:
			GPIOE_PDOR |= MASK(GREEN_LED_4);
			break;
		case Green5:
			GPIOE_PDOR |= MASK(GREEN_LED_5);
			break;
		case Green6:
			GPIOE_PDOR |= MASK(GREEN_LED_6);
			break;
		case Green7:
			GPIOE_PDOR |= MASK(GREEN_LED_7);
			break;
		case Green8:
			GPIOE_PDOR |= MASK(GREEN_LED_8);
			break;
	}
}

void off_led(color_t color) {
	switch (color) {
		case Red:
			GPIOC_PDOR &= ~MASK(RED_LED);
			break;
		case Green1:
			GPIOE_PDOR &= ~MASK(GREEN_LED_1);
			break;
		case Green2:
			GPIOE_PDOR &= ~MASK(GREEN_LED_2);
			break;
		case Green3:
			GPIOE_PDOR &= ~MASK(GREEN_LED_3);
			break;
		case Green4:
			GPIOE_PDOR &= ~MASK(GREEN_LED_4);
			break;
		case Green5:
			GPIOE_PDOR &= ~MASK(GREEN_LED_5);
			break;
		case Green6:
			GPIOE_PDOR &= ~MASK(GREEN_LED_6);
			break;
		case Green7:
			GPIOE_PDOR &= ~MASK(GREEN_LED_7);
			break;
		case Green8:
			GPIOE_PDOR &= ~MASK(GREEN_LED_8);
			break;
	}
}

void toggle_red(void) {
	PTC->PTOR |= MASK(RED_LED);
}

void flash_green(volatile bool *shouldStop) {
	for (int i = 0; i < green_led_num && !(*shouldStop); i++) {
		flash_led(green_leds[i]);
		osDelay(500);
		off_led(green_leds[i]);
	}
}

void on_green(void) {
	for (int i = 0; i < green_led_num; i++) {
		flash_led(green_leds[i]);
	}
}

void off_green(void) {
	for (int i = 0; i < green_led_num; i++) {
		off_led(green_leds[i]);
	}
}

void flash_red(int duration_ms) {
	flash_led(Red);
	osDelay(duration_ms);
	off_led(Red);
	osDelay(duration_ms);
}

void flash_red_fast(void) {
	flash_red(250);
}

void flash_red_slow(void) {
	flash_red(500);
	// PTC->PTOR |= MASK(RED_LED);
}

/* Green LEDs will be in running mode and red will flash SLOW */
void running_leds(volatile bool *shouldStop) {
	off_green();
	for (int i = 0; i < green_led_num && !(*shouldStop); i++) {
		toggle_red();
		flash_led(green_leds[i]);
		osDelay(500);
		off_led(green_leds[i]);
	}
}

/* Green LEDs will be on throughout and red will flash FAST */
void stationary_leds(void) {
	on_green();
	flash_red_fast();
}

/* When bluetooth connection is successful, green LEDs will blink twice*/
void connection_leds(void) {
	on_green();
	osDelay(200);
	off_green();
	osDelay(200);
	on_green();
	osDelay(200);
	off_green();
	osDelay(200);
}