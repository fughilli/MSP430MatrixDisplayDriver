#include "tone.h"

#include <msp430.h>

#define F_SMCLK (2000000ul)

static uint32_t song_idx;
static uint32_t song_len;
static uint32_t tone_end;
static bool busy;
static const song_note_t* tone_song;
static bool song_loop;


void tone_song_play(const song_note_t* song, uint32_t song_length, bool loop)
{
    tone_song = song;
    song_len = song_length;
    song_idx = 0;
    song_loop = loop;

    tone_play(song[0].freq, song[0].dur_ms);
}

void tone_init()
{
    TA0CTL = TACLR;
    TA0CTL = TASSEL_2 | ID_0 | TAIE;

    TA0CCR0 = 100;

    TA0CCR1 = 0;

    TA0CCTL1 = OUTMOD_7;

    P2DIR |= (1 << 6);
    P2SEL |= (1 << 6);
    P2SEL &= ~(1 << 7);
    P2SEL2 &= ~(3 << 6);

    tone_song = 0;
    song_loop = false;
}

bool tone_busy()
{
    return busy;
}

void tone_play(uint16_t freq, uint16_t ms)
{
    if(freq)
    {
        TA0CCR0 = F_SMCLK / freq;
        TA0CCR1 = TA0CCR0 / 2;
    }
    else
    {
        TA0CCR0 = 2000;
        TA0CCR1 = 0;
    }

    tone_end = 2000ul * ms;

    busy = true;

    TA0CTL |= MC_1;
}

void tone_stop()
{
    tone_end = 0;
    TA0CTL &= ~MC_3;
    busy = false;
}

__attribute__((__interrupt__(TIMER0_A0_VECTOR)))
void ISR_timer0_a0()
{

}

__attribute__((__interrupt__(TIMER0_A1_VECTOR)))
void ISR_timer0_a1()
{
    TA0CTL &= ~TAIFG;

    if(tone_end == 0)
    {
        //if(tone_song)
        //{
        //    song_idx++;

        //    if(song_idx == song_len)
        //    {
        //        if(song_loop)
        //        {
        //            song_idx = 0;
        //        }
        //        else
        //        {
        //            tone_song = 0;
        //            tone_stop();
        //        }
        //    }

        //    if(tone_song)
        //        tone_play(tone_song[song_idx].freq, tone_song[song_idx].dur_ms);
        //}
        //else
        //{
            tone_stop();
        //}
    }
    else if(tone_end <= TA0CCR0)
    {
        tone_end = 0;
    }
    else
    {
        tone_end -= TA0CCR0;
    }
}
