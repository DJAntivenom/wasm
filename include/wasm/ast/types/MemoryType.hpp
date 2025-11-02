#ifndef __WASM_MEMORY_TYPE_HPP__
#define __WASM_MEMORY_TYPE_HPP__

#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

struct MemoryType
{
    AddressType addrtype;
    Limits limits;
};

WASM_NAMESPACE_END

#endif
