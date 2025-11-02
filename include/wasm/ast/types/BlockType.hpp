#ifndef __WASM_BLOCK_TYPE_HPP__
#define __WASM_BLOCK_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <optional>
#include <variant>

#include "ValueType.hpp"

WASM_NAMESPACE_BEGIN

struct BlockType
{
    std::variant<std::optional<ValType>, TypeIndex> blocktype;
};

WASM_NAMESPACE_END

#endif
