#ifndef __WASM_NUM_TYPE_HPP__
#define __WASM_NUM_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <variant>

WASM_NAMESPACE_BEGIN

struct NumType
{
    enum
    {
        I32,
        I64,
        F32,
        F64,
    } numtype;

    inline bool isI32() const { return numtype == I32; };
    inline bool isI64() const { return numtype == I64; };
    inline bool isF32() const { return numtype == F32; };
    inline bool isF64() const { return numtype == F64; };

    NumType(decltype(numtype) type) : numtype(type) {};

    bool operator==(const NumType &other) const
    {
        return numtype == other.numtype;
    }

    bool operator!=(const NumType &other) const
    {
        return !(*this == other);
    }
};

struct VecType
{
    bool operator==(const VecType &other) const
    {
        (void)other;
        return true;
    }

    bool operator!=(const VecType &other) const
    {
        (void)other;
        return false;
    }
};

WASM_NAMESPACE_END

#endif
