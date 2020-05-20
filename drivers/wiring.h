#ifndef WIRING_H
#define WIRING_H

#include <stdint.h>

typedef uint16_t PinMask_t;

enum class PinMode_t : uint8_t
{
    INPUT = 0,
    OUTPUT
};

enum class PinState_t : uint8_t
{
    LOW = 0,
    HIGH
};

void pinMode(PinMask_t mask, PinMode_t mode);
PinMode_t getPinMode(PinMask_t mask);
void digitalWrite(PinMask_t mask, PinState_t state);
PinState_t digitalRead(PinMask_t mask);

PinMode_t operator~(PinMode_t arg);
PinState_t operator~(PinState_t arg);

#endif
