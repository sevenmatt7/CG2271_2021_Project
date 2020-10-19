#ifndef MUSIC_H
#define MUSIC_H

#include <stdbool.h>
#include "RTE_Components.h"
#include  CMSIS_device_header

#define MUSIC_PTC4 4  // PTC4
#define TPM0_PRESCALER 128  

typedef struct {
	int frequency;
	int duration;
	int stop_duration;
} note_t;

extern const note_t start_notes[];
extern const note_t loop_notes[];
extern const note_t end_notes[];
extern const int start_notes_num;
extern const int end_notes_num;
extern const int loop_notes_num;

void init_music(void);
uint32_t freq_to_mod(uint32_t frequency);
void stop_beep(void);
void beep(uint32_t frequency, uint32_t duration_ms, uint32_t stop_duration_ms);
void start_theme(void);
void loop_theme(volatile bool *isEnded);
void end_theme(void);

#endif /* MUSIC_H */
