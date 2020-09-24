#include "MKL25Z4.h"                    // Device header
#include <stdbool.h> 
#include "led.h"
#include "delay.h"

void init_led() {
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
	if (color == Red) {
		GPIOC_PDOR &= ~MASK(RED_LED);
	}
}

void flash_green() {
	// turn off all leds first in case switching from all on mode
	for (int i = 0; i < green_led_num; i++) {
		off_led(green_leds[i]);
	}
	
	while (true) {
		for (int i = 0; i < green_led_num; i++) {
			flash_led(green_leds[i]);
			delay(500);
			off_led(green_leds[i]);
		}
	}
}

void on_green() {
	for (int i = 0; i < green_led_num; i++) {
		flash_led(green_leds[i]);
	}
}

void flash_red(int duration_ms) {
	while (true) {
		flash_led(Red);
		delay(duration_ms);
		off_led(Red);
		delay(duration_ms);
	}
}

void flash_red_fast() {
	flash_red(250);
}

void flash_red_slow() {
	flash_red(500);
}

///* MAIN function */
//int main(void) 
//{
//	init_led();
//	flash_green();
//}
