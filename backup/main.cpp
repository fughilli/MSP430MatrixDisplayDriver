/*
 * main.c
 */

#include <msp430.h>
#include "util/static_utils.h"
#include "gamelib/gamelib.h"
#include "LEDMatrix/LEDMatrix.h"
#include "drivers/wiring.h"
#include "font3x5.h"
#include "font5x7.h"
#include "gamelib/utils/math/game_math.h"

#include "tone.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int8_t lsin_LUT[] = { 0, 2, 3, 2, 0, -2, -3, -2 };

const song_note_t happy_birthday[] = {
    { TONE_G5, 90 },
    { 0, 10 },
    { TONE_G5, 90 },
    { 0, 10 },
    { TONE_A5, 190 },
    { 0, 10 },
    { TONE_G5, 190 },
    { 0, 10 },
    { TONE_C6, 190 },
    { 0, 10 },
    { TONE_B5, 390 },
    { 0, 10 },

    { TONE_G5, 90 },
    { 0, 10 },
    { TONE_G5, 90 },
    { 0, 10 },
    { TONE_A5, 190 },
    { 0, 10 },
    { TONE_G5, 190 },
    { 0, 10 },
    { TONE_D6, 190 },
    { 0, 10 },
    { TONE_C6, 390 },
    { 0, 10 },

    { TONE_G5, 90 },
    { 0, 10 },
    { TONE_G5, 90 },
    { 0, 10 },
    { TONE_G6, 190 },
    { 0, 10 },
    { TONE_E6, 190 },
    { 0, 10 },
    { TONE_C6, 190 },
    { 0, 10 },
    { TONE_B5, 190 },
    { 0, 10 },
    { TONE_A5, 390 },
    { 0, 410 },

    { TONE_F6, 90 },
    { 0, 10 },
    { TONE_F6, 90 },
    { 0, 10 },
    { TONE_E6, 190 },
    { 0, 10 },
    { TONE_C6, 190 },
    { 0, 10 },
    { TONE_D6, 190 },
    { 0, 10 },
    { TONE_C6, 390 },
    { 0, 410 }
};

#define NUM_NOTES (sizeof(happy_birthday) / sizeof(song_note_t))

uint16_t note_idx = 0;

#define lsin(x) (lsin_LUT[(x) & 0x7])
#define lcos(x) (lsin_LUT[((x) + 2) & 0x7])

uint8_t char_mapping(uint8_t p)
{
    return p;
}

Point_t modpos;
uint32_t modtime = 0;

Point_t& text_posmod(uint32_t charnum)
{
    modpos.x = lsin(charnum + modtime/4);
    modpos.y = lcos(charnum + modtime/4);
    return modpos;
}

const uint8_t screen_width = 32, screen_height = 16;

Bitmap_t bitmap5x7(font5x7, font5x7_width, font5x7_height);
Font_t font5x7_font(&bitmap5x7, 5, 7, 5, 1, char_mapping);

uint8_t screenBuffer[screen_width * screen_height / 8];
uint8_t drawBuffer[screen_width * screen_height / 8];

LEDMatrix matrix((1<<0), (1<<1), (1<<2), (1<<3),
                 (1<<4), (1<<5), (1<<8), (1<<9));

Screen screen((uint8_t*)drawBuffer,
              screen_width,
              screen_height);

int8_t time_4s = 0;

int16_t scroll_pos;
int16_t scroll_pos_max;

const char * bannerStrings[] = 
{
    "4 years and $120k later...",
    "When's the final, again?",
    "I never bought my textbooks ;)",
    "Anyone have 19P?",
    "GO BRUINS!!!",
    "This grad cap brought to you by CAFFEINE (c)"
};

char * bannerString;

struct ActionToggle
{
    static bool value;

    static void fire()
    {
        value = !value;
    }
};

bool ActionToggle::value = false;

template<uint32_t _period, class ActionAdapter>
struct PeriodicAction
{
    constexpr static uint32_t period = _period;
    static uint32_t count;
    
    static void init()
    {
        count = period;
    }   

    static void tick()
    {
        if(!--count) { ActionAdapter::fire(); count = _period; }
    }
};

template<uint32_t _period, class ActionAdapter>
uint32_t PeriodicAction<_period,ActionAdapter>::count = 0;

typedef PeriodicAction<4, ActionToggle> boxToggleTicker;

bool reducer = false;

__attribute__((interrupt(TIMER1_A0_VECTOR)))
void timer_interrupt()
{
    reducer = !reducer;
    if(reducer)
        return;

    time_4s++;
    modtime++;

    scroll_pos++;
    if(scroll_pos >= scroll_pos_max)
        scroll_pos = -32;

    //screen.box(0,0,32,8,Color_t::BLACK, Color_t::BLACK);
    screen.clear(Color_t::BLACK);
    screen.text_plus_offset(font5x7_font, {-scroll_pos, 4}, bannerString, text_posmod, Bitmap_mode_t::MODE_BLEND_INVERT);

    if(ActionToggle::value)
        screen.box(0,0,31,15,Color_t::WHITE,Color_t::NONE);
    else
        screen.box(1,1,30,14,Color_t::WHITE,Color_t::NONE);

    boxToggleTicker::tick();
}

__attribute__((interrupt(TIMER1_A1_VECTOR)))
    void other_timer_interrupt()
{
}

constexpr uint16_t timer_period = (F_CPU / 8 / 8 / 8) - 1;

// Program entry point
int main(void)
{
    WDTCTL = WDTHOLD | WDTPW;

    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    BCSCTL2 = SELM_0 | DIVM_0 | DIVS_3;

    TA1CTL = TACLR;
    TA1CTL = TASSEL_2 | MC_0 | ID_1;
    TA1CCTL0 = CCIE;
    TA1CCR0 = timer_period;
    TA1CTL |= MC_1;

    _BIS_SR(GIE);

    boxToggleTicker::init();

    matrix.begin(screenBuffer, screen_width, screen_height);

    screen.clear(Color_t::BLACK);

    scroll_pos_max = strlen(bannerString) * 6;
    scroll_pos_max += 32;
    scroll_pos = 0;

    pinMode((1<<6), PinMode_t::OUTPUT);

    digitalWrite((1<<6), PinState_t::HIGH);

    tone_init();
    
    uint32_t song_idx = 0;

    tone_play(100, 1000);

    while(1)
    {
        matrix.scan();

        if(!tone_busy())
        {
            tone_play(happy_birthday[song_idx].freq, happy_birthday[song_idx].dur_ms);
            song_idx++;
            if(song_idx == NUM_NOTES)
                song_idx = 0;
        }

    }
}
