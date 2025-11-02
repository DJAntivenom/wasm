/**
 * \file TypeSectionDecoder.hpp
 * \brief Declaration of the TypeSectionDecoder class.
 */

#ifndef __WASM_TYPE_SECTION_DECODER_HPP__
#define __WASM_TYPE_SECTION_DECODER_HPP__

#include "SectionDecoder.hpp"

#include <vector>

WASM_DECODE_NAMESPACE_BEGIN

/**
 * \brief The spec can be found here: https://webassembly.github.io/spec/core/binary/index.html
 */
class TypeSectionDecoder : public virtual SectionDecoder
{
public:
    TypeSectionDecoder() : SectionDecoder(static_cast<SectionID>(1)) {};

    virtual Error<void> decode(Module &module) const override;
};

WASM_DECODE_NAMESPACE_END

#endif