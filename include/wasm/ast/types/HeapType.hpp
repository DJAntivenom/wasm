#ifndef __WASM_HEAP_TYPE_HPP__
#define __WASM_HEAP_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <variant>

WASM_NAMESPACE_BEGIN

struct AbsHeapType
{
    enum
    {
        Any,
        Eq,
        I31,
        Struct,
        Array,
        None,
        Func,
        NoFunc,
        Exn,
        NoExn,
        Extern,
        NoExtern
    } absheaptype;

    bool operator==(const AbsHeapType &other) const
    {
        return absheaptype == other.absheaptype;
    }

    bool operator!=(const AbsHeapType &other) const
    {
        return !(*this == other);
    }
};

struct HeapType
{
    std::variant<AbsHeapType, TypeUse> heaptype;

    bool operator==(const HeapType &other) const
    {
        return heaptype == other.heaptype;
    }

    bool operator!=(const HeapType &other) const
    {
        return !(*this == other);
    }
};

WASM_NAMESPACE_END

#endif
