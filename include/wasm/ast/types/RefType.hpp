#ifndef __WASM_REF_TYPE_HPP__
#define __WASM_REF_TYPE_HPP__

#include <wasm/wasm.hpp>

#include "HeapType.hpp"

WASM_NAMESPACE_BEGIN

struct RefType
{
    bool is_nullable;
    HeapType reftype;
};

WASM_NAMESPACE_END

#endif
