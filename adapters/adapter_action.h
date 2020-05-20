#ifndef ADAPTER_ACTION_H
#define ADAPTER_ACTION_H

namespace Adapters
{
    struct NullActionAdapter
    {
        static bool act() { return true; }
    };
}

#endif
