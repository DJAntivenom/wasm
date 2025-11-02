/**
 * \file BinaryDecoder.hpp
 * \brief Declaration of the BinaryDecoder class.
 */

#ifndef __WASM_BINARY_DECODER_HPP__
#define __WASM_BINARY_DECODER_HPP__

#include "Decoder.hpp"
#include "SectionDecoder.hpp"

#include <vector>
#include <filesystem>
#include <fstream>

WASM_DECODE_NAMESPACE_BEGIN

/**
 * @brief Decodes the binary representation of a module given a stream.
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/binary/index.html
 */
class BinaryDecoder : public virtual Decoder<Module>
{
public:
    BinaryDecoder() = default;
    BinaryDecoder(BinaryDecoder &&other)
        : section_decoders(std::move(other.section_decoders)) {};
    BinaryDecoder &operator=(BinaryDecoder &&other) noexcept
    {
        if (this != &other)
        {
            section_decoders = std::move(other.section_decoders);
        }
        return *this;
    }

    /**
     * @brief Creates a binary decoder from the rest of the content of `stream`.
     * @param filename The path to the file that should be decoded.
     */
    [[nodiscard]]
    static Error<std::unique_ptr<BinaryDecoder>> createBinaryDecoder(const std::string &filename);

    virtual Error<Module> decode() const override;

private:
    /**
     * @brief A list of decoders needed for the individual sections.
     */
    std::vector<std::unique_ptr<SectionDecoder>> section_decoders;
};

WASM_DECODE_NAMESPACE_END

#endif