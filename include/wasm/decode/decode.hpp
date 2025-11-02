/**
 * \file decode.hpp
 * \brief Collects all header files in this directory.
 */

#ifndef __WASM_DECODE_HPP__
#define __WASM_DECODE_HPP__

#include <wasm/wasm.hpp>

#include <cstdint>

#define WASM_DECODE_NAMESPACE_BEGIN \
    WASM_NAMESPACE_BEGIN            \
    namespace decode                \
    {
#define WASM_DECODE_NAMESPACE_END \
    WASM_NAMESPACE_END            \
    }

WASM_DECODE_NAMESPACE_BEGIN

/// forward declarations and types

/**
 * @brief A byte can be represented directly.
 */
using Byte = std::uint8_t;

template <class DecodedType>
class Decoder;
class BinaryDecoder;
class SectionDecoder;

WASM_DECODE_NAMESPACE_END

#include "Decoder.hpp"

#endif
