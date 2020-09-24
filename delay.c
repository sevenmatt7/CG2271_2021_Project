#include "MKL25Z4.h"                    // Device header
#include "delay.h"

void delay(int delay_ms) {
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;  // enable clock gate for low-power timer (LPTMR)
	
	LPTMR0_CSR = 0;  // disable LPTMR
	LPTMR0_CMR = LPTMR_CMR_COMPARE(delay_ms);  // set the compare value

	LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;  // disable the prescaler 

	LPTMR0_CSR = LPTMR_CSR_TEN_MASK;  // enable timer
	while(!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK));  // busy waiting

	LPTMR0_CSR = 0;  // disable the timer back
}