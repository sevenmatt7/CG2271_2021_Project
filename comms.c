#include "RTE_Components.h"
#include  CMSIS_device_header
#include <stdbool.h>

#define MASK(x) (1 << (x))

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
//#define UART_RX_PORTD2 2

#define UART2_INT_PRIO 128

#define Q_SIZE (32)

volatile uint8_t rx_data = 0x00;
bool debug = false;

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
		} else {
			//error - queue full
			while(1);
		}
	}

}

int main(void) {
	SystemCoreClockUpdate();
	initMotor();
	
	rx_data = 0x00;
	
	while(1) {
		switch(rx_data) {
			case 0x00: 
				forward();
				break;
			case 0x03: 
				reverse();
				break;
			case 0x02:
				turnLeft();
				break;
			case 0x01: 
				turnRight();
				break;
		}
	}
}