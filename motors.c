#include "RTE_Components.h"
#include  CMSIS_device_header

#define PTB0_Pin 0  //PTB0 TPM1_CH0, front right motor
#define PTB1_Pin 1  //PTB1 TPM1_CH1, back right motor
#define PTB2_Pin 2  //PTB2 TPM2_CH0, front left motor
#define PTB3_Pin 3  //PTB3 TPM2_CH1, back left motor
#define UP 0x00   
#define DOWN 0x11
#define LEFT 0x10
#define RIGHT 0x01
#define SPEED 100

volatile uint8_t direction_left = 0x00;
volatile uint8_t direction_right = 0x00;

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
		if (direction_left == LEFT) {
				TPM2_C0V = (int) (percentage * 7499.0 / 255);
				TPM2_C1V = 0;
		} else if (direction_left == DOWN) {
				TPM2_C0V = 0;
				TPM2_C1V = (int) (percentage * 7499.0 / 255);
		} else {
				TPM2_C0V = (int) (percentage * 7499.0 / 255);
				TPM2_C1V = 0;
		}
}


void setRightPower(uint8_t percentage) {
		if (direction_right == RIGHT) {
				TPM1_C0V = (int) (percentage * 7499.0 / 255);
				TPM1_C1V = 0;
		} else if (direction_right == DOWN) {
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
	direction_left = UP;
	direction_right = UP;
	setLeftPower(SPEED);
	setRightPower(SPEED);
}

void reverse(void) {
	direction_left = DOWN;
	direction_right = DOWN;
	setLeftPower(SPEED);
	setRightPower(SPEED);
}

void turnRight(void) {
	direction_left = LEFT;
	direction_right = LEFT;
	setLeftPower(SPEED);
	setRightPower(SPEED/4);
}

void turnLeft(void) {
	direction_left = RIGHT;
	direction_right = RIGHT;
	setLeftPower(SPEED/4);
	setRightPower(SPEED);
}

void pivotRight(void) {
	direction_left = UP;
	direction_right = DOWN;
	setLeftPower(SPEED);
	setRightPower(SPEED);
}

void pivotLeft(void) {
	direction_left = DOWN;
	direction_right = UP;
	setLeftPower(SPEED);
	setRightPower(SPEED);
}
