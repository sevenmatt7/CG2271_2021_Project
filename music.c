#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#include "music.h"

void init_music(void) {
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  // enable PortC clock gate
	
	// Configure PortC Pin4 to be TPM0 CH3 (PWM output for speaker)
	PORTC->PCR[MUSIC_PTC4] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[MUSIC_PTC4] |= PORT_PCR_MUX(4);  // alternative 4
	
	SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;  // enable clock gate for TPM0
	
	//use MCGFLLCLK as clock for TPM
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	TPM0->MOD = 7500;
	
	//set Prescale factor to 128 and select count up mode
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Setup PWM mode for TPM0 CH3
	// Disable the channel as recommended when switching
	TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	// Set to edge-aligned PWM High-true pulses (clear Output on match, set Output on reload)
	TPM0_C3SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

/*
* Convert music notes in frequency to timer MOD
*/
uint32_t freq_to_mod(uint32_t frequency) {
	return SystemCoreClock / TPM0_PRESCALER / frequency;
}

void stop_beep(void) {
	TPM0->MOD = 0;
	TPM0_C3V = 0;
}

/*
* Play the note to the buzzer for the specified duration
*/
void beep(uint32_t frequency, uint32_t duration_ms, uint32_t stop_duration_ms) {
	uint32_t mod = freq_to_mod(frequency);
	TPM0->MOD = mod;
	TPM0_C3V = mod / 2;  // 50% duty cycle
	osDelay(duration_ms);
	stop_beep();
	osDelay(stop_duration_ms);
}

void start_theme(void) {
	beep(660, 100, 150);
	beep(660, 100, 300);
	beep(660, 100, 300);
	beep(510, 100, 100);
	beep(660, 100, 300);
	beep(770, 100, 550);
}

void loop_theme(void) {
	beep(380, 100, 575);
	beep(510, 100, 450);
	beep(380, 100, 400);
	beep(320, 100, 500);
	beep(440, 100, 300);
	beep(480, 80, 330);
	beep(450, 100, 150);
	beep(430, 100, 300);
	beep(380, 100, 200);
	beep(660, 80, 200);
	beep(760, 50, 150);
	beep(860, 100, 300);
	beep(700, 80, 150);
	beep(760, 50, 350);
	beep(660, 80, 300);
	beep(520, 80, 150);
	beep(580, 80, 150);

	beep(480, 80, 500);
	beep(510, 100, 450);
	beep(380, 100, 400);
	beep(320, 100, 500);
	beep(440, 100, 300);
	beep(480, 80, 330);
	beep(450, 100, 150);
	beep(430, 100, 300);
	beep(380, 100, 200);
	beep(660, 80, 200);
	beep(760, 50, 150);
	beep(860, 100, 300);
	beep(700, 80, 150);
	beep(760, 50, 350);
	beep(660, 80, 300);
	beep(520, 80, 150);
	beep(580, 80, 150);

	beep(480, 80, 500);

	beep(500, 100, 300);
	beep(760, 100, 100);
	beep(720, 100, 150);
	beep(680, 100, 150);

	beep(620, 150, 300);
	beep(650, 150, 300);
	beep(380, 100, 150);

	beep(430, 100, 150);
	beep(500, 100, 300);
	beep(430, 100, 150);
	beep(500, 100, 100);

	beep(570, 100, 220);

	beep(500, 100, 300);
	beep(760, 100, 100);
	beep(720, 100, 150);
	beep(680, 100, 150);

	beep(620, 150, 300);

	beep(650, 200, 300);
	beep(1020, 80, 300);
	beep(1020, 80, 150);

	beep(1020, 80, 300);
	beep(380, 100, 300);

	beep(500, 100, 300);
	beep(760, 100, 100);
	beep(720, 100, 150);
	beep(680, 100, 150);

	beep(620, 150, 300);
	beep(650, 150, 300);
	beep(380, 100, 150);

	beep(430, 100, 150);
	beep(500, 100, 300);
	beep(430, 100, 150);
	beep(500, 100, 100);

	beep(570, 100, 420);

	beep(585, 100, 450);

	beep(550, 100, 420);

	beep(500, 100, 360);
	beep(380, 100, 300);
	beep(500, 100, 300);
	beep(500, 100, 150);

	beep(500, 100, 300);

	beep(500, 100, 300);
	beep(760, 100, 100);
	beep(720, 100, 150);
	beep(680, 100, 150);

	beep(620, 150, 300);
	beep(650, 150, 300);
	beep(380, 100, 150);

	beep(430, 100, 150);
	beep(500, 100, 300);
	beep(430, 100, 150);
	beep(500, 100, 100);

	beep(570, 100, 220);

	beep(500, 100, 300);
	beep(760, 100, 100);
	beep(720, 100, 150);
	beep(680, 100, 150);

	beep(620, 150, 300);

	beep(650, 200, 300);
	beep(1020, 80, 300);
	beep(1020, 80, 150);

	beep(1020, 80, 300);
	beep(380, 100, 300);

	beep(500, 100, 300);
	beep(760, 100, 100);
	beep(720, 100, 150);
	beep(680, 100, 150);

	beep(620, 150, 300);
	beep(650, 150, 300);
	beep(380, 100, 150);

	beep(430, 100, 150);
	beep(500, 100, 300);
	beep(430, 100, 150);
	beep(500, 100, 100);

	beep(570, 100, 420);

	beep(585, 100, 450);

	beep(550, 100, 420);

	beep(500, 100, 360);
	beep(380, 100, 300);
	beep(500, 100, 300);
	beep(500, 100, 150);

	beep(500, 100, 300);
	beep(500, 60, 150);
	beep(500, 80, 300);
	beep(500, 60, 350);
	beep(500, 80, 150);
	beep(580, 80, 350);
	beep(660, 80, 150);
	beep(500, 80, 300);
	beep(430, 80, 150);

	beep(380, 80, 600);
	beep(500, 60, 150);
	beep(500, 80, 300);
	beep(500, 60, 350);
	beep(500, 80, 150);
	beep(580, 80, 150);

	beep(660, 80, 550);
	beep(870, 80, 325);

	beep(760, 80, 600);
	beep(500, 60, 150);
	beep(500, 80, 300);
	beep(500, 60, 350);
	beep(500, 80, 150);
	beep(580, 80, 350);
	beep(660, 80, 150);
	beep(500, 80, 300);
	beep(430, 80, 150);

	beep(380, 80, 600);
	beep(660, 100, 150);
	beep(660, 100, 300);
	beep(660, 100, 300);
	beep(510, 100, 100);
	beep(660, 100, 300);
	beep(770, 100, 550);
}

void end_theme(void) {
	beep(196, 166, 0);
	beep(262, 166, 0);
	beep(330, 166, 0);
	beep(392, 166, 0);
	beep(523, 166, 0);
	beep(659, 166, 0);
	beep(784, 500, 0);
	beep(659, 500, 0);
	
	beep(165, 166, 0);
	beep(262, 166, 0);
	beep(311, 166, 0);
	beep(415, 166, 0);
	beep(523, 166, 0);
	beep(622, 166, 0);
	beep(831, 500, 0);
	beep(622, 500, 0);
	
	beep(233, 166, 0);
	beep(294, 166, 0);
	beep(349, 166, 0);
	beep(466, 166, 0);
	beep(587, 166, 0);
	beep(698, 166, 0);
	beep(932, 500, 20);
	beep(932, 166, 20);
	beep(932, 166, 20);
	beep(932, 166, 0);
	beep(1047, 1800, 0);
}
