#ifndef __WASM_GLOBAL_TYPE_HPP__
#define __WASM_GLOBAL_TYPE_HPP__

#include <wasm/wasm.hpp>

#include "ValueType.hpp"

WASM_NAMESPACE_BEGIN

struct GlobalType
{
    bool is_mutable;
    ValType reftype;
};

WASM_NAMESPACE_END

#endif
