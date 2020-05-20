/*
 * main.c
 */

#include <msp430.h>
#include "util/static_utils.h"
#include "util/register_utils.h"
#include "gamelib/gamelib.h"
#include "LEDMatrix/LEDMatrix.h"
#include "drivers/wiring.h"
#include "resources/font3x5.h"
#include "resources/font5x7.h"
#include "gamelib/utils/math/game_math.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int8_t lsin_LUT[] = { 0, 2, 3, 2, 0, -2, -3, -2 };

#define lsin(x) (lsin_LUT[(x) & 0x7])
#define lcos(x) (lsin_LUT[((x) + 2) & 0x7])

uint8_t char_mapping(uint8_t p)
{
    return p;
}

Point_t modpos;
uint32_t modtime = 0;

Point_t& text_posmod_1(uint32_t charnum)
{
    modpos.y = (2 * ((charnum + modtime/8) % 2)) - 1;
    modpos.x = 0;
    return modpos;
}

Point_t& text_posmod_2(uint32_t charnum)
{
    modpos.x = lsin(charnum + modtime/4)/2;
    modpos.y = lcos(charnum + modtime/4)/2;
    return modpos;
}

Point_t& text_posmod_3(uint32_t charnum)
{
    uint8_t res = (charnum + modtime / 16) % 4;
    if (res == 0)
    {
        modpos.y = 2;
        modpos.x = -1;
    }
    else if (res == 2)
    {
        modpos.y = -1;
        modpos.x = 2;
    }
    else
    {
        modpos.y = 0;
        modpos.x = 0;
    }
    return modpos;
}

const int8_t zigzag_table[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, -1, -2, -3, -4, -3, -2, -1 };

Point_t& text_posmod_4(uint32_t charnum)
{
    uint8_t res = (charnum + modtime / 2) % ARR_LEN(zigzag_table);
    modpos.y = zigzag_table[res];
    modpos.x = 0;
    return modpos;
}

Point_t& (*posmod_funcs[])(uint32_t) =
{
    text_posmod_1,
    text_posmod_2,
    text_posmod_3,
    text_posmod_4
};

Point_t& (*posmod_func)(uint32_t) = text_posmod_1;
const uint8_t numPosmodFuncs = ARR_LEN(posmod_funcs);

void updatePosmodFunc(uint8_t idx)
{
    posmod_func = posmod_funcs[idx % numPosmodFuncs];
}

Point_t& text_posmod(uint32_t charnum)
{
    return posmod_func(charnum);
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

const char * const bannerStrings[] =
{
    "4 years and $120k later...",
    "When's the final, again?",
    "I never bought my textbooks ;)",
    "Anyone have 19P?",
    "GO BRUINS!!!",
    "This grad cap brought to you by CAFFEINE (TM)",
    "Where is Public Affairs?",
    "Bring back the roasted turkey!",
    "*8-CLAPPING INTENSIFIES*",
    "Hi Mom and Dad!",
    "Am I an engineer now?",
    "Congratulations class of 2017 :D",
    "Please keep your hands, arms, feet, and legs inside of Boelter Hall at all times.",
};

const char * const lowProbabilityBannerStrings[] =
{
    "Gene Blocc is the homedawg",
    "Shoutout to UCLA Memes for Sick AF Tweens"
};

const uint8_t odds = 100;

const uint8_t numBannerStrings = ARR_LEN(bannerStrings);
const uint8_t numBannerStringsLowProb = ARR_LEN(lowProbabilityBannerStrings);
const uint16_t bannerStringCompleteMask = (1ul << numBannerStrings) - 1;
uint16_t bannerStringMask = 0;
const uint16_t lowProbabilityBannerStringCompleteMask = (1ul << numBannerStringsLowProb) - 1;
uint16_t lowProbabilityBannerStringMask = 0;

const char * bannerString;

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
volatile bool draw_flag = false;

void updateBannerStringLowProb(uint8_t idx)
{
    idx %= numBannerStringsLowProb;

    if(lowProbabilityBannerStringMask == lowProbabilityBannerStringCompleteMask)
        lowProbabilityBannerStringMask = 0;

    while(lowProbabilityBannerStringMask & (1 << idx))
    {
        idx = (idx + 1) % numBannerStringsLowProb;
    }

    lowProbabilityBannerStringMask |= (1 << idx);

    bannerString = lowProbabilityBannerStrings[idx];
    scroll_pos_max = strlen(bannerString) * 6;
    scroll_pos_max += 32;
    scroll_pos = 0;
}

void updateBannerString(uint8_t idx)
{
    idx %= numBannerStrings;

    if(bannerStringMask == bannerStringCompleteMask)
        bannerStringMask = 0;

    while(bannerStringMask & (1 << idx))
    {
        idx = (idx + 1) % numBannerStrings;
    }

    bannerStringMask |= (1 << idx);

    bannerString = bannerStrings[idx];
    scroll_pos_max = strlen(bannerString) * 6;
    scroll_pos_max += 32;
    scroll_pos = 0;
}

__attribute__((interrupt(TIMER1_A0_VECTOR)))
void timer_interrupt()
{
    time_4s++;
    modtime++;

    scroll_pos++;
    if(scroll_pos >= scroll_pos_max)
    {
        if(rand() % odds == 0)
        {
            updateBannerStringLowProb(rand());
        }
        else
        {
            updateBannerString(rand());
        }
        updatePosmodFunc(rand());
        scroll_pos = -32;
    }

    if(!draw_flag)
    {
        screen.clear(Color_t::BLACK);
        screen.text_plus_offset(font5x7_font, {-scroll_pos, 4}, bannerString, text_posmod, Bitmap_mode_t::MODE_BLEND_INVERT);

        draw_flag = true;
    }

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

    updateBannerString(0);

    pinMode((1<<6), PinMode_t::OUTPUT);
    digitalWrite((1<<6), PinState_t::HIGH);

    while(1)
    {
        matrix.scan();

        if(matrix.finishedScan() && draw_flag)
        {
            memcpy(screenBuffer, drawBuffer, sizeof(screenBuffer));
            draw_flag = false;
        }
    }
}
