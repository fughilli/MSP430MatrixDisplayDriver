#ifndef REGISTER_UTILS_H
#define REGISTER_UTILS_H

#include <stdint.h>

#define ARR_LEN(x) (sizeof(x) / sizeof((x)[0]))
#define REGMAP_EMPTY(__size__,__name__) uint8_t __name__[(__size__)]

namespace RegisterUtil
{    

    template<typename T, intptr_t ptr>
    constexpr T* declareAt()
    {
        return reinterpret_cast<T*>(ptr);
    }

}

#endif
