#ifndef __WASM_LIMITS_HPP__
#define __WASM_LIMITS_HPP__

#include <wasm/wasm.hpp>

#include <cstdint>
#include <optional>

WASM_NAMESPACE_BEGIN

struct Limits
{
    std::uint64_t lower;
    std::optional<std::uint64_t> upper;
};

WASM_NAMESPACE_END

#endif
