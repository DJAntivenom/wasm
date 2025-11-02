#ifndef __WASM_NUM_TYPE_HPP__
#define __WASM_NUM_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <variant>

WASM_NAMESPACE_BEGIN

struct NumType
{
    struct I32
    {
    };
    struct I64
    {
    };
    struct F32
    {
    };
    struct F64
    {
    };

    std::variant<I32, I64, F32, F64> numtype;
};

struct VecType
{
};

WASM_NAMESPACE_END

#endif
