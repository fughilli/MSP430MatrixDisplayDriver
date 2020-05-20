#ifndef TONE_H
#define TONE_H

#include <stdint.h>
#include <stdbool.h>

#include "tones.h"

typedef struct {
    uint16_t freq;
    uint16_t dur_ms;
} song_note_t;

void tone_init();
void tone_play(uint16_t freq, uint16_t ms);
void tone_song_play(const song_note_t* song, uint32_t song_length, bool loop);
void tone_stop();
bool tone_busy();

#endif
