/**
 * \file FunctionSectionDecoder.hpp
 * \brief Declaration of the FunctionSectionDecoder class.
 */

#ifndef __WASM_FUNCTION_SECTION_DECODER_HPP__
#define __WASM_FUNCTION_SECTION_DECODER_HPP__

#include "SectionDecoder.hpp"

WASM_DECODE_NAMESPACE_BEGIN

/**
 * \brief The spec can be found here: https://webassembly.github.io/spec/core/binary/index.html
 */
class FunctionSectionDecoder : public virtual SectionDecoder
{
public:
    FunctionSectionDecoder() : SectionDecoder(static_cast<SectionID>(3)) {};

    virtual Error<void> decode(Module &module) const override;
};

WASM_DECODE_NAMESPACE_END

#endif