#include "RTE_Components.h"
#include  CMSIS_device_header
#include <stdbool.h>

#define MASK(x) (1 << (x))

#define PTB0_Pin 0  //PTB0 TPM1_CH0, front right motor
#define PTB1_Pin 1  //PTB1 TPM1_CH1, back right motor
#define PTB2_Pin 2  //PTB2 TPM2_CH0, front left motor
#define PTB3_Pin 3  //PTB3 TPM2_CH1, back left motor
#define UP 0x00   
#define DOWN 0x11
#define LEFT 0x10
#define RIGHT 0x01
#define SPEED 100

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
//#define UART_RX_PORTD2 2

#define UART2_INT_PRIO 128

#define Q_SIZE (32)

volatile uint8_t direction = 0x00;
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

/** Sets up the pins used for pwm to control the motors */ 
void initMotor (void) {
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
		
	//set all pins to function as TPM
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
		
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);	
		
	PORTB->PCR[PTB2_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB2_Pin] |= PORT_PCR_MUX(3);
		
	PORTB->PCR[PTB3_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB3_Pin] |= PORT_PCR_MUX(3);
		
	//enable clocks to TPM1 and TPM2
	SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM2_MASK;
		
	//use MCGFLLCLK as clock for TPM
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
		
	TPM1->MOD = 7499;
		
	//set Prescale factor to 128 and select count up mode
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
		
	//set to edge-aligned high true pulses
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSB_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSB_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
		
	TPM2->MOD = 7499;
		
	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK);

	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSB_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSB_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	initUART2();
}

/* The two functions below change the duty cycle of the PWM signal, changing the power supplied to the motors
		which will change the speed that it runs at 
void setLeftPower(uint8_t percentage) {
	if (direction == 0x10) {
		TPM2_C0V = 0;
		TPM2_C1V = (int) (percentage * 7499.0 / 255);
	} else {
		TPM2_C0V = (int) (percentage * 7499.0 / 255);
		TPM2_C1V = 0;
	}
}


void setRightPower(uint8_t percentage) {
	if (direction == 0x01) {
		TPM1_C0V = 0;
		TPM1_C1V = (int) (percentage * 7499.0 / 255);
	} else {
		TPM1_C0V = (int) (percentage * 7499.0 / 255);
		TPM1_C1V = 0;
	}
}
*/

/* The two functions below change the duty cycle of the PWM signal, changing the power supplied to the motors
		which will change the speed that it runs at */
void setLeftPower(uint8_t percentage) {
		if (direction == 0x10) {
				TPM2_C0V = (int) (percentage * 7499.0 / 255);
				TPM2_C1V = 0;
		} else if (direction == 0x11) {
				TPM2_C0V = 0;
				TPM2_C1V = (int) (percentage * 7499.0 / 255);
		} else {
				TPM2_C0V = (int) (percentage * 7499.0 / 255);
				TPM2_C1V = 0;
		}
}


void setRightPower(uint8_t percentage) {
		if (direction == 0x01) {
				TPM1_C0V = (int) (percentage * 7499.0 / 255);
				TPM1_C1V = 0;
		} else if (direction == 0x11) {
				TPM1_C0V = 0;
				TPM1_C1V = (int) (percentage * 7499.0 / 255);
		} else {
				TPM1_C0V = (int) (percentage * 7499.0 / 255);
				TPM1_C1V = 0;
		}
}

// to stop all the motors
void stop(void) {	
	TPM1_C0V = 0;
	TPM1_C1V = 0;
	TPM2_C0V = 0;
	TPM2_C1V = 0;
}

// functions to move robot are below

void forward(void) {
	direction = UP;
	setLeftPower(SPEED);
	setRightPower(SPEED);
}

void reverse(void) {
	direction = DOWN;
	setLeftPower(SPEED);
	setRightPower(SPEED);
}

void turnRight(void) {
	direction = RIGHT;
	setLeftPower(SPEED/4);
	setRightPower(SPEED);
}

void turnLeft(void) {
	direction = LEFT;
	setLeftPower(SPEED);
	setRightPower(SPEED/4);
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