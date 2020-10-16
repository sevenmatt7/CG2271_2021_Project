/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include <stdbool.h>

#include "led.h"
#include "music.h"
#include "motors.h"

#define MASK(x) (1 << (x))

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
//#define UART_RX_PORTD2 2
#define UART2_INT_PRIO 128
#define Q_SIZE (32)

// These are flags to indicate what event should be flagged to be performed next by the robot
#define UART_FLAG 0x00000001
#define RUNNING_LED_FLAG 0x00000002
#define CONNECT_FLAG 0x00000004
#define LOOP_FLAG 0x00000008
#define END_FLAG 0x00000010
#define STATIONARY_FLAG 0x00000020
#define CLEAR_FLAG 0xFFFFFFFF

volatile bool *isStopped = &(bool){false};
volatile uint8_t rx_data = 0x00;

osEventFlagsId_t event_select;
osMutextId_t mutex_led;

/** =============== QUEUE STRUCTURE =============== **/
typedef struct {
	unsigned char Data[Q_SIZE];
	unsigned int Head; //points to oldest data element
	unsigned int Tail; //points to next free space
	unsigned int Size; //quantity of elements in queue
} Q_T;

Q_T tx_q, rx_q;

/** =============== QUEUE FUNCTIONS =============== **/
void Q_Init(Q_T *q) {
	unsigned int i;
	for (i=0; i<Q_SIZE; i++) {
		q->Data[i] = 0; //initiate everythin to 0 -> to simplify out lives when debugging
	}
	q->Head = 0;
	q->Tail = 0;
	q->Size = 0;
}

int Q_Empty(Q_T *q) {
	return q->Size == 0;
}

int Q_Full(Q_T *q) {
	return q->Size == Q_SIZE;
}

int Q_Enqueue(Q_T *q, unsigned char d) {
	//When queue is full -> overwrite
	if (!Q_Full(q)) {
		q->Data[q->Tail++] = d;
		q->Tail %= Q_SIZE;
		q->Size++;
		return 1;
	} else {
		return 0;
	}
}

unsigned char Q_Dequeue(Q_T *q) {
	//Check if queue is empty before dequeuing
	unsigned char t = 0;
	if (!Q_Empty(q)) {
		t = q->Data[q->Head];
		q->Data[q->Head++] = 0; //again to simplify debugging
		q->Head %= Q_SIZE;
		q->Size--;
	}
	return t;
}

/** =============== INTIALISE UART2 =============== **/
void initUART2(void)
{
	uint32_t divisor, bus_clock;
	
	//Enable clock gating for UART2 and PORTE
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	//Configure and enable pins for UART Transmitter
	//PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
	//PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);
	
	//Configure and enable pins for UART Receiver
	//PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] = PORT_PCR_MUX(4);
	
	//Disabling UART Module Transmitter and Receiver --> Clearing
	UART2->C2 &= ~(UART_C2_RE_MASK);
	
	//Default system clock --> 48MHz
	//bus_clock --> Runs UART at half the frequency (Default system design)
	bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
	divisor = bus_clock / (BAUD_RATE * 16); //baud_rate * 16 --> oversampling to minimise error (noise)
	UART2->BDH = UART_BDH_SBR(divisor >> 8); //high register
	UART2->BDL = UART_BDL_SBR(divisor); //low register
	
	UART2->C1 = UART2->S2 = UART2->C3 = 0;
	
	//Enabling UART Module Transmitter and Receiver --> Setting
	UART2->C2 |= (UART_C2_RE_MASK);
	//Enabling interrupts for UART2
	UART2->C2 |= (UART_C2_RIE_MASK);
	
	NVIC_SetPriority(UART2_IRQn, 2);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	
	Q_Init(&tx_q);
	Q_Init(&rx_q);
}

/** =============== UART2 INTERRUPT HANDLER =============== **/
void UART2_IRQHandler(void) {	
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	/*
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_data = UART2->D;
		debug = !debug;
	}
	*/
	
	//RECEIVER
	if (UART2->S1 & UART_S1_RDRF_MASK) {		
		if (!Q_Full(&rx_q)) {
			Q_Enqueue(&rx_q, UART2->D);
			rx_data = Q_Dequeue(&rx_q);
			osEventFlagsSet(event_select, UART_FLAG);
		} else {
			//error - queue full
			while(1);
		}
	}

}

/** =============== PROGRAM THREADS =============== **/

// void green_led_thread(void *argument) {
// 	while (true) {
// 		if (*isStopped) {
			 
// 		} 
// 		else {
// 			off_green();  // turn off all leds first in case switching from all on mode
// 			flash_green(isStopped);  // should stop immediately when the motor is stopped
// 		}
// 	}
// }

// void red_led_thread(void *argument) {
// 	while (true) {
// 		if (*isStopped) {
// 			flash_red_fast();
// 		} else {
// 			flash_red_slow();
// 		}
// 	}
// }

//This is the thread that executes throughout as the robot is moving
void loop_music_thread(void *argument) {
	while (true) {
		// start_theme();
		// TODO: loop this and check condition, if condition is met jump out directly to play the end_theme
		osEventFlagsWait(event_select, LOOP_FLAG, osFlagsNoClear, osWaitForever);
		loop_theme();
		// end_theme();	
	}
}

// This is the thread that executes after the robot has received confirmation that it has reached the end of the maze
void end_thread(void *argument) {
	while (true) {
		osEventFlagsWait(event_select, END_FLAG, osFlagsWaitAny, osWaitForever);
		osEventFlagsClear(event_select, LOOP_FLAG);
		end_theme();
		osEventFlagsSet(event_select, LOOP_FLAG);
	}
}

// This is the thread that executes after the robot has received confirmation of bluetooth connection from the app
void bluetooth_thread(void *argument) {
	start_theme();
	while (true) {
		osEventFlagsWait(event_select, CONNECT_FLAG, osFlagsWaitAny, osWaitForever);
		osMutexAcquire(mutex_led, osWaitForever);
		connection_leds();
		osMutexRelease(mutex_led);
		osEventFlagsSet(event_select, LOOP_FLAG);
	}
}

// This is the thread that executes for the LEDs to be in 'static' mode when the robot is stationary
void static_led_thread(void *argument) {
	while (true) {
		//use NoClear so the flags won't be cleared after one execution, only clear in the brain_thread
		osEventFlagsWait(event_select, STATIONARY_FLAG, osFlagsNoClear, osWaitForever);
		osMutexAcquire(mutex_led, osWaitForever);
		stationary_leds();
		osMutexRelease(mutex_led);
	}
}

// This is the thread that executes for the LEDs to be in 'running' mode when the robot is moving
void running_led_thread(void *argument) {
	while (true) {
		//use NoClear so the flags won't be cleared after one execution, only clear in the brain thread
		osEventFlagsWait(event_select, RUNNING_LED_FLAG, osFlagsNoClear, osWaitForever); 
		osMutexAcquire(mutex_led, osWaitForever);
		running_leds(*isStopped);
		osMutexRelease(mutex_led);
	}
}

// This is the thread that executes after every UART Interrupt has been handled.
void brain_thread(void *argument) {
	while (true) {
		osEventFlagsWait(event_select, UART_FLAG, osFlagsWaitAny, osWaitForever);
		switch(rx_data) {
			case 0x00:   //if 00 is received from bluetooth, the robot will be stationary
				stop();
				osEventFlagsClear(event_select, RUNNING_LED_FLAG);
				osEventFlagsSet(event_select, STATIONARY_FLAG);
				*isStopped = true;
				break;
			case 0x03:   //if 03 is received from bluetooth, the robot will reverse
				reverse();
				osEventFlagsClear(event_select, STATIONARY_FLAG);
				osEventFlagsSet(event_select, RUNNING_LED_FLAG);
				*isStopped = false;
				break;
			case 0x02:  //if 02 is received from bluetooth, the robot will turn left
				turnLeft();
				osEventFlagsClear(event_select, STATIONARY_FLAG);
				osEventFlagsSet(event_select, RUNNING_LED_FLAG);
				*isStopped = false;
				break;
			case 0x01:  //if 01 is received from bluetooth, the robot will turn right
				turnRight();
				osEventFlagsClear(event_select, STATIONARY_FLAG);
				osEventFlagsSet(event_select, RUNNING_LED_FLAG);
				*isStopped = false;
				break;
			case 0x04:  //if 04 is received from bluetooth, the robot will move forward
				forward();
				osEventFlagsClear(event_select, STATIONARY_FLAG);
				osEventFlagsSet(event_select, RUNNING_LED_FLAG);
				*isStopped = false;
				break;
			case 0x05:  //if 05 is received from bluetooth, the end_flag will be set and the end music will play
				osEventFlagsSet(event_select, END_FLAG);
				break;
			case 0x06: //if 06 is received from bluetooth, this signals to the robot that bluetooth has been established
				osEventFlagsSet(event_select, CONNECT_FLAG);
				break;
		}
	}
}


int main (void) {
	// System Initialization
	SystemCoreClockUpdate();
	osKernelInitialize();         // Initialize CMSIS-RTOS

	init_led();
	init_music();
	initUART2();
	initMotor();

	// Initialise mutex and event flags to prevent race conditions
	mutex_led = osMutexNew(NULL);

	event_select = osEventFlagsNew(NULL);
	osEventFlagsClear(event_select, CLEAR_FLAG);
	osEventFlagsSet(event_select_id, STATIONARY_FLAG);

	// Initialise all the threads
	osThreadNew(brain_thread, NULL, NULL);   
	osThreadNew(bluetooth_thread, NULL, NULL);  
	osThreadNew(static_led_thread, NULL, NULL);  
	osThreadNew(running_led_thread, NULL, NULL); 
	osThreadNew(end_thread, NULL, NULL); 
	osThreadNew(loop_music_thread, NULL, NULL); 

	osKernelStart();    // Start thread execution
	for (;;) {}
}
