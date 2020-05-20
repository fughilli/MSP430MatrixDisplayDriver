#ifndef _GAME_MATH_H_
#define _GAME_MATH_H_

#ifdef __cplusplus

namespace GameMath
{

template <class T>
void clamp(T& val, T minval, T maxval)
{
    if(val < minval)
        val = minval;
    if(val > maxval)
        val = maxval;
}

}

#endif

#endif // _GAME_MATH_H_

