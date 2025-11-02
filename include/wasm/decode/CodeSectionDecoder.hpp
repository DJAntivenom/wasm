#ifndef __WASM_CODE_SECTION_DECODER_HPP__
#define __WASM_CODE_SECTION_DECODER_HPP__

#include "SectionDecoder.hpp"

WASM_DECODE_NAMESPACE_BEGIN

/**
 * \brief The spec can be found here: https://webassembly.github.io/spec/core/binary/index.html
 */
class CodeSectionDecoder : public virtual SectionDecoder
{
public:
    CodeSectionDecoder() : SectionDecoder(static_cast<SectionID>(10)) {};

    virtual Error<void> decode(Module &module) const override;
};

WASM_DECODE_NAMESPACE_END

#endif