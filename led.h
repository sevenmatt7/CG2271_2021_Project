#ifndef LED_H
#define LED_H

#define RED_LED 11  // PortC Pin 11
#define GREEN_LED_1 30  // PortE Pin 30, top most
#define GREEN_LED_2 29  // PortE Pin 29
#define GREEN_LED_3 22  // PortE Pin 22
#define GREEN_LED_4 21  // PortE Pin 21
#define GREEN_LED_5 20  // PortE Pin 20
#define GREEN_LED_6 5  // PortE Pin 5
#define GREEN_LED_7 4  // PortE Pin 4
#define GREEN_LED_8 3  // PortE Pin 3, bottom most

#define MASK(x) (1 << (x))

typedef enum {
	Red, 
	Green1, 
	Green2, 
	Green3,
	Green4,
	Green5,
	Green6,
	Green7,
	Green8
} color_t;

const color_t green_leds[] = {Green1, Green2, Green3, Green4, Green5, Green6, Green7, Green8};
const int green_led_num = sizeof(green_leds)/sizeof(green_leds[0]);

void init_led();
void flash_led(color_t color);
void off_led(color_t color);
void flash_green();
void on_green();
void flash_red(int duration_ms);
void flash_red_fast();
void flash_red_slow();

#endif /* LED_H */