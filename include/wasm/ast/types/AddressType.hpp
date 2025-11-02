#ifndef __WASM_ADDRESS_TYPE_HPP__
#define __WASM_ADDRESS_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <variant>

WASM_NAMESPACE_BEGIN

struct AddressType
{
    struct I32
    {
    };
    struct I64
    {
    };

    std::variant<I32, I64> addrtype;
};

WASM_NAMESPACE_END

#endif
