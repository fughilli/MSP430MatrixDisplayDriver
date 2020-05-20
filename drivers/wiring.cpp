#include "wiring.h"
#include <msp430.h>

#define _clearBit(__REG__,__MASK__) ((__REG__) &= ~(__MASK__))
#define _setBit(__REG__,__MASK__) ((__REG__) |= (__MASK__))
#define _getBit(__REG__,__MASK__) ((__REG__) & (__MASK__))

void pinMode(PinMask_t mask, PinMode_t mode)
{
    switch(mode)
    {
        case PinMode_t::INPUT:
            _clearBit(P1DIR, mask & 0xFF);
            _clearBit(P2DIR, (mask >> 8) & 0xFF);
            break;
        case PinMode_t::OUTPUT:
            _setBit(P1DIR, mask & 0xFF);
            _setBit(P2DIR, (mask >> 8) & 0xFF);
            break;
    }
}

void digitalWrite(PinMask_t mask, PinState_t state)
{
    switch(state)
    {
        case PinState_t::LOW:
            _clearBit(P1OUT, mask & 0xFF);
            _clearBit(P2OUT, (mask >> 8) & 0xFF);
            break;
        case PinState_t::HIGH:
            _setBit(P1OUT, mask & 0xFF);
            _setBit(P2OUT, (mask >> 8) & 0xFF);
            break;
    }
}

PinMode_t getPinMode(PinMask_t mask)
{
    if(mask > 0x80)
    {
        if(_getBit(P2DIR, (mask >> 8) & 0xFF))
            return PinMode_t::OUTPUT;
        return PinMode_t::INPUT;
    }
    if(_getBit(P1DIR, mask & 0xFF))
        return PinMode_t::OUTPUT;
    return PinMode_t::INPUT;
}

PinState_t digitalRead(PinMask_t mask)
{
    switch(getPinMode(mask))
    {
        case PinMode_t::OUTPUT:
            if(mask > 0x80)
            {
                if(_getBit(P2OUT, (mask >> 8) & 0xFF))
                    return PinState_t::HIGH;
                return PinState_t::LOW;
            }
            if(_getBit(P1OUT, mask & 0xFF))
                return PinState_t::HIGH;
            return PinState_t::LOW;

        case PinMode_t::INPUT:
            if(mask > 0x80)
            {
                if(_getBit(P2IN, (mask >> 8) & 0xFF))
                    return PinState_t::HIGH;
                return PinState_t::LOW;
            }
            if(_getBit(P1IN, mask & 0xFF))
                return PinState_t::HIGH;
            return PinState_t::LOW;
    }
}

PinMode_t operator~(PinMode_t arg)
{
    switch (arg)
    {
        case PinMode_t::INPUT:
            return PinMode_t::OUTPUT;
        case PinMode_t::OUTPUT:
            return PinMode_t::INPUT;
    }
}

PinState_t operator~(PinState_t arg)
{
    switch (arg)
    {
        case PinState_t::LOW:
            return PinState_t::HIGH;
        case PinState_t::HIGH:
            return PinState_t::LOW;
    }
}
