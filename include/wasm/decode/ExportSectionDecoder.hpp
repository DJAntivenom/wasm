#ifndef __WASM_EXPORT_SECTION_DECODER_HPP__
#define __WASM_EXPORT_SECTION_DECODER_HPP__

#include "SectionDecoder.hpp"

WASM_DECODE_NAMESPACE_BEGIN

/**
 * \brief The spec can be found here: https://webassembly.github.io/spec/core/binary/index.html
 */
class ExportSectionDecoder : public virtual SectionDecoder
{
public:
    ExportSectionDecoder() : SectionDecoder(static_cast<SectionID>(7)) {};

    virtual Error<void> decode(Module &module) const override;
};

WASM_DECODE_NAMESPACE_END

#endif