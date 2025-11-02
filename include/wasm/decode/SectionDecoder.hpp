/**
 * \file SectionDecoder.hpp
 * \brief Declaration of the SectionDecoder class.
 */

#ifndef __WASM_SECTION_DECODER_HPP__
#define __WASM_SECTION_DECODER_HPP__

#include <wasm/wasm.hpp>

#include <cstdint>
#include <spanstream>
#include <vector>

WASM_DECODE_NAMESPACE_BEGIN

/**
 * @brief Definitions are taken from the spec.
 */
enum SectionID
{
    SECTION_ID_CUSTOM = 0,
    SECTION_ID_TYPE = 1,
    SECTION_ID_IMPORT = 2,
    SECTION_ID_FUNCTION = 3,
    SECTION_ID_TABLE = 4,
    SECTION_ID_MEMORY = 5,
    SECTION_ID_GLOBAL = 6,
    SECTION_ID_EXPORT = 7,
    SECTION_ID_START = 8,
    SECTION_ID_ELEMENT = 9,
    SECTION_ID_CODE = 10,
    SECTION_ID_DATA = 11,
    SECTION_ID_DATA_COUNT = 12,
    SECTION_ID_TAG = 13,
    SECTION_ID_MAX_VALUE,
};

/**
 * @brief Decodes the binary representation of a section given a stream.
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/binary/modules.html#sections
 */
class SectionDecoder : public virtual Decoder<void>
{
public:
    /**
     * @brief Create a matching decoder for the next section.
     * @param input_stream The next section is read from this stream,
     * however, it is not yet decoded. The stream is advanced.
     * @return A unique_ptr to an object of a subclass capable of
     * decoding the read section. If an error occurs a string explaining
     * the error is returned instead.
     */
    static Error<std::unique_ptr<SectionDecoder>> createDecoder(InputStream &input_stream);

    virtual ~SectionDecoder() {};

    [[nodiscard]]
    virtual Error<void> decode(Module &module) const = 0;

    inline const SectionID &getID() const { return id; };

private:
    /** @brief The type of section */
    SectionID id;

    /** @brief The raw data read in as unsigned bytes */
    std::vector<Byte> raw_data;

    virtual Error<void> decode() const override
    {
        return Error<void>();
    }

protected:
    SectionDecoder(const SectionID &id) : id(id) {};

    /**
     * @brief Get the matching section decoder based on the given id.
     * @return A decoder or an error string.
     */
    static Error<std::unique_ptr<SectionDecoder>> getSectionDecoderFromID(SectionID id);

    inline auto getRawDataSize() const { return raw_data.size(); };

    inline std::ispanstream getRawDataAsStream() const
    {
        std::span<const char> span(reinterpret_cast<const char *>(raw_data.data()), raw_data.size());
        return std::ispanstream(span);
    }
};

WASM_DECODE_NAMESPACE_END

/// include subclasses
#include "TypeSectionDecoder.hpp"
#include "CodeSectionDecoder.hpp"
#include "ExportSectionDecoder.hpp"
#include "FunctionSectionDecoder.hpp"
#include "MemorySectionDecoder.hpp"
#include "GlobalSectionDecoder.hpp"

#endif