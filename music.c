#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#include "music.h"

// data to play start music
const note_t start_notes[] = {
	{660, 100, 150},
	{660, 100, 300},
	{660, 100, 300},
	{510, 100, 100},
	{660, 100, 300},
	{770, 100, 550},
	{380, 100, 575}
};

// data to play loop music
const note_t loop_notes[] = {
	{510, 100, 450},
	{380, 100, 400},
	{320, 100, 500},
	{440, 100, 300},
	{480, 80, 330},
	{450, 100, 150},
	{430, 100, 300},
	{380, 100, 200},
	{660, 80, 200},
	{760, 50, 150},
	{860, 100, 300},
	{700, 80, 150},
	{760, 50, 350},
	{660, 80, 300},
	{520, 80, 150},
	{580, 80, 150},

	{480, 80, 500},
	{510, 100, 450},
	{380, 100, 400},
	{320, 100, 500},
	{440, 100, 300},
	{480, 80, 330},
	{450, 100, 150},
	{430, 100, 300},
	{380, 100, 200},
	{660, 80, 200},
	{760, 50, 150},
	{860, 100, 300},
	{700, 80, 150},
	{760, 50, 350},
	{660, 80, 300},
	{520, 80, 150},
	{580, 80, 150},

	{480, 80, 500},

	{500, 100, 300},
	{760, 100, 100},
	{720, 100, 150},
	{680, 100, 150},

	{620, 150, 300},
	{650, 150, 300},
	{380, 100, 150},

	{430, 100, 150},
	{500, 100, 300},
	{430, 100, 150},
	{500, 100, 100},

	{570, 100, 220},

	{500, 100, 300},
	{760, 100, 100},
	{720, 100, 150},
	{680, 100, 150},

	{620, 150, 300},

	{650, 200, 300},
	{1020, 80, 300},
	{1020, 80, 150},

	{1020, 80, 300},
	{380, 100, 300},

	{500, 100, 300},
	{760, 100, 100},
	{720, 100, 150},
	{680, 100, 150},

	{620, 150, 300},
	{650, 150, 300},
	{380, 100, 150},

	{430, 100, 150},
	{500, 100, 300},
	{430, 100, 150},
	{500, 100, 100},

	{570, 100, 420},

	{585, 100, 450},

	{550, 100, 420},

	{500, 100, 360},
	{380, 100, 300},
	{500, 100, 300},
	{500, 100, 150},

	{500, 100, 300},

	{500, 100, 300},
	{760, 100, 100},
	{720, 100, 150},
	{680, 100, 150},

	{620, 150, 300},
	{650, 150, 300},
	{380, 100, 150},

	{430, 100, 150},
	{500, 100, 300},
	{430, 100, 150},
	{500, 100, 100},

	{570, 100, 220},

	{500, 100, 300},
	{760, 100, 100},
	{720, 100, 150},
	{680, 100, 150},

	{620, 150, 300},

	{650, 200, 300},
	{1020, 80, 300},
	{1020, 80, 150},

	{1020, 80, 300},
	{380, 100, 300},

	{500, 100, 300},
	{760, 100, 100},
	{720, 100, 150},
	{680, 100, 150},

	{620, 150, 300},
	{650, 150, 300},
	{380, 100, 150},

	{430, 100, 150},
	{500, 100, 300},
	{430, 100, 150},
	{500, 100, 100},

	{570, 100, 420},

	{585, 100, 450},

	{550, 100, 420},

	{500, 100, 360},
	{380, 100, 300},
	{500, 100, 300},
	{500, 100, 150},

	{500, 100, 300},
	{500, 60, 150},
	{500, 80, 300},
	{500, 60, 350},
	{500, 80, 150},
	{580, 80, 350},
	{660, 80, 150},
	{500, 80, 300},
	{430, 80, 150},

	{380, 80, 600},
	{500, 60, 150},
	{500, 80, 300},
	{500, 60, 350},
	{500, 80, 150},
	{580, 80, 150},

	{660, 80, 550},
	{870, 80, 325},

	{760, 80, 600},
	{500, 60, 150},
	{500, 80, 300},
	{500, 60, 350},
	{500, 80, 150},
	{580, 80, 350},
	{660, 80, 150},
	{500, 80, 300},
	{430, 80, 150},

	{380, 80, 600},
	{660, 100, 150},
	{660, 100, 300},
	{660, 100, 300},
	{510, 100, 100},
	{660, 100, 300},
	{770, 100, 550}
};

// data to play end music
const note_t end_notes[] = {
	{196, 166, 20},
	{262, 166, 20},
	{330, 166, 20},
	{392, 166, 20},
	{523, 166, 20},
	{659, 166, 20},
	{784, 500, 20},
	{659, 500, 20},
				 
	{165, 166, 20},
	{262, 166, 20},
	{311, 166, 20},
	{415, 166, 20},
	{523, 166, 20},
	{622, 166, 20},
	{831, 500, 20},
	{622, 500, 20},
	
	{233, 166, 20},
	{294, 166, 20},
	{349, 166, 20},
	{466, 166, 20},
	{587, 166, 20},
	{698, 166, 20},
	{932, 500, 20},
	{932, 166, 20},
	{932, 166, 20},
	{932, 166, 20},
	{1047, 1300, 0}
};

const int start_notes_num = sizeof(start_notes)/sizeof(start_notes[0]);
const int end_notes_num = sizeof(end_notes)/sizeof(end_notes[0]);
const int loop_notes_num = sizeof(loop_notes)/sizeof(loop_notes[0]);

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
	if (stop_duration_ms > 0) {
		osDelay(stop_duration_ms);
	}
}

void start_theme(void) {
	for (int i = 0; i < start_notes_num; i++) {
		note_t note = start_notes[i];
		beep(note.frequency, note.duration, note.stop_duration);
	}
}

void loop_theme(volatile bool *isEnded) {
	for (int i = 0; i < loop_notes_num && !(*isEnded); i++) {
		note_t note = loop_notes[i];
		beep(note.frequency, note.duration, note.stop_duration);
	}
}

void end_theme(void) {
	for (int i = 0; i < end_notes_num; i++) {
		note_t note = end_notes[i];
		beep(note.frequency, note.duration, note.stop_duration);
	}
}
