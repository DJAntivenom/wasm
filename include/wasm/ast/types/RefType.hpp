#ifndef __WASM_REF_TYPE_HPP__
#define __WASM_REF_TYPE_HPP__

#include <wasm/wasm.hpp>

#include "HeapType.hpp"

WASM_NAMESPACE_BEGIN

struct RefType
{
    bool is_nullable;
    HeapType reftype;

    bool operator==(const RefType &other) const
    {
        return reftype == other.reftype;
    }

    bool operator!=(const RefType &other) const
    {
        return !(*this == other);
    }

    friend std::ostream &operator<<(std::ostream &os, const RefType &obj)
    {
        os << "Ref";
        if (obj.is_nullable)
            os << " null";
        return os;
    }
};

WASM_NAMESPACE_END

#endif
