/**
 * \file MemorySectionDecoder.hpp
 * \brief Declaration of the MemorySectionDecoder class.
 */

#ifndef __WASM_MEMORY_SECTION_DECODER_HPP__
#define __WASM_MEMORY_SECTION_DECODER_HPP__

#include "SectionDecoder.hpp"

WASM_DECODE_NAMESPACE_BEGIN

/**
 * \brief The spec can be found here: https://webassembly.github.io/spec/core/binary/index.html
 */
class MemorySectionDecoder : public virtual SectionDecoder
{
public:
    MemorySectionDecoder() : SectionDecoder(static_cast<SectionID>(5)) {};

    virtual Error<void> decode(Module &module) const override;
};

WASM_DECODE_NAMESPACE_END

#endif