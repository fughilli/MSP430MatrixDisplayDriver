#ifndef REGISTERS_GPTIMER_H
#define REGISTERS_GPTIMER_H

#include "util/register_utils.h"
#include "util/static_utils.h"

#include <stdint.h>

namespace RegistersGPTimer
{
    enum class TimerWidth : uint8_t
    {
        NARROW = 32,
        WIDE = 64
    };

    template<TimerWidth _width>
    struct Map
    {
        uint32_t CFG;           // Offset 0x000
        uint32_t TAMR;          // Offset 0x004
        uint32_t TBMR;          // Offset 0x008
        uint32_t CTL;           // Offset 0x00C
        uint32_t SYNC;          // Offset 0x010

        REGMAP_EMPTY(0x018-0x014, pad0);

        uint32_t IMR;           // Offset 0x018
        uint32_t RIS;           // Offset 0x01C
        uint32_t MIS;           // Offset 0x020
        uint32_t ICR;           // Offset 0x024
        uint32_t TAILR;         // Offset 0x028
        uint32_t TBILR;         // Offset 0x02C
        uint32_t TAMATCHR;      // Offset 0x030
        uint32_t TBMATCHR;      // Offset 0x034
        uint32_t TAPR;          // Offset 0x038
        uint32_t TBPR;          // Offset 0x03C
        uint32_t TAPMR;         // Offset 0x040
        uint32_t TBPMR;         // Offset 0x044
        uint32_t TAR;           // Offset 0x048
        uint32_t TBR;           // Offset 0x04C
        uint32_t TAV;           // Offset 0x050
        uint32_t TBV;           // Offset 0x054
        uint32_t RTCPD;         // Offset 0x058
        uint32_t TAPS;          // Offset 0x05C
        uint32_t TBPS;          // Offset 0x060
        uint32_t TAPV;          // Offset 0x064
        uint32_t TBPV;          // Offset 0x068

        REGMAP_EMPTY(0xFC0-0x06C, pad1);

        uint32_t PP;            // Offset 0xFC0

        constexpr static const TimerWidth width = _width;
    };

    namespace CFG
    {
        Field<uint32_t, 0, 3> CFG;
    }

    namespace TxMR
    {
        Field<uint32_t, 0, 2> TxMR;
        Field<uint32_t, 2, 1> TxCMR;
        Field<uint32_t, 3, 1> TxAMS;
        Field<uint32_t, 4, 1> TxCDIR;
        Field<uint32_t, 5, 1> TxMIE;
        Field<uint32_t, 6, 1> TxWOT;
        Field<uint32_t, 7, 1> TxSNAPS;
        Field<uint32_t, 8, 1> TxILD;
        Field<uint32_t, 9, 1> TxPWMIE;
        Field<uint32_t, 10, 1> TxMRSU;
        Field<uint32_t, 11, 1> TxPLO;
    }

    namespace CTL
    {
        Field<uint32_t, 0, 1> TAEN;
        Field<uint32_t, 1, 1> TASTALL;
        Field<uint32_t, 2, 2> TAEVENT;
        Field<uint32_t, 4, 1> RTCEN;
        Field<uint32_t, 5, 1> TAOTE;
        Field<uint32_t, 6, 1> TAPWML;
        Field<uint32_t, 8, 1> TBEN;
        Field<uint32_t, 9, 1> TBSTALL;
        Field<uint32_t, 10, 2> TBEVENT;
        Field<uint32_t, 13, 1> TBOTE;
        Field<uint32_t, 14, 1> TBPWML;
    }

    constexpr static Map<TimerWidth::NARROW>* const timer0Map =
        RegisterUtil::declareAt<Map<TimerWidth::NARROW>, 0x40030000>();
    constexpr static Map<TimerWidth::NARROW>* const timer1Map =
        RegisterUtil::declareAt<Map<TimerWidth::NARROW>, 0x40031000>();
    constexpr static Map<TimerWidth::NARROW>* const timer2Map =
        RegisterUtil::declareAt<Map<TimerWidth::NARROW>, 0x40032000>();
    constexpr static Map<TimerWidth::NARROW>* const timer3Map =
        RegisterUtil::declareAt<Map<TimerWidth::NARROW>, 0x40033000>();
    constexpr static Map<TimerWidth::NARROW>* const timer4Map =
        RegisterUtil::declareAt<Map<TimerWidth::NARROW>, 0x40034000>();
    constexpr static Map<TimerWidth::NARROW>* const timer5Map =
        RegisterUtil::declareAt<Map<TimerWidth::NARROW>, 0x40035000>();

    constexpr static Map<TimerWidth::WIDE>* const wideTimer0Map =
        RegisterUtil::declareAt<Map<TimerWidth::WIDE>, 0x40030000>();
    constexpr static Map<TimerWidth::WIDE>* const wideTimer1Map =
        RegisterUtil::declareAt<Map<TimerWidth::WIDE>, 0x40031000>();
    constexpr static Map<TimerWidth::WIDE>* const wideTimer2Map =
        RegisterUtil::declareAt<Map<TimerWidth::WIDE>, 0x40032000>();
    constexpr static Map<TimerWidth::WIDE>* const wideTimer3Map =
        RegisterUtil::declareAt<Map<TimerWidth::WIDE>, 0x40033000>();
    constexpr static Map<TimerWidth::WIDE>* const wideTimer4Map =
        RegisterUtil::declareAt<Map<TimerWidth::WIDE>, 0x40034000>();
    constexpr static Map<TimerWidth::WIDE>* const wideTimer5Map =
        RegisterUtil::declareAt<Map<TimerWidth::WIDE>, 0x40035000>();
}

#endif
