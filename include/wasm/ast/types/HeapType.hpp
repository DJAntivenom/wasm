#ifndef __WASM_HEAP_TYPE_HPP__
#define __WASM_HEAP_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <variant>

WASM_NAMESPACE_BEGIN

struct AbsHeapType
{
    struct Any
    {
    };
    struct Eq
    {
    };
    struct I31
    {
    };
    struct Struct
    {
    };
    struct Array
    {
    };
    struct None
    {
    };
    struct Func
    {
    };
    struct NoFunc
    {
    };
    struct Exn
    {
    };
    struct NoExn
    {
    };
    struct Extern
    {
    };
    struct NoExtern
    {
    };

    std::variant<Any, Eq, I31, Struct, Array, None, Func, NoFunc, Exn, NoExn, Extern, NoExtern> absheaptype;
};

struct HeapType
{
    std::variant<AbsHeapType, TypeUse> heaptype;
};

WASM_NAMESPACE_END

#endif
