#include <wasm/wasm.hpp>

#include <sstream>
#include <iostream>

WASM_DECODE_NAMESPACE_BEGIN

Error<std::unique_ptr<SectionDecoder>> SectionDecoder::createDecoder(InputStream &input_stream)
{
    using RetType = Error<std::unique_ptr<SectionDecoder>>;

    if (options::is_verbose)
        std::cout << "Creating Section decoder\n";

    auto starting_byte = input_stream.tellg();
    SectionID id;

    /* read id */
    {
        Byte raw_id;
        input_stream.read(reinterpret_cast<char *>(&raw_id), 1);

        if (raw_id >= SECTION_ID_MAX_VALUE)
        {
            std::stringstream error_message;
            error_message << "Unknown section with ID = " << static_cast<std::uint32_t>(raw_id)
                          << " (At byte " << std::hex << std::showbase << starting_byte << ")";
            return RetType(std::unexpected(error_message.str()));
        }

        id = static_cast<SectionID>(raw_id);
    }

    /* read length */
    auto error_length = decodeLEB128<std::uint32_t>(input_stream);
    if (!error_length)
    {
        std::stringstream error_message;
        error_message << "Can't decode length of section with ID = " << id
                      << " (At byte " << std::hex << std::showbase << starting_byte << ")"
                      << "\n  Reason: " << error_length.error();
        return RetType(std::unexpected(error_message.str()));
    }

    auto decoder = getSectionDecoderFromID(id);
    if (!decoder)
    {
        /* This should never happen, because we check id above */
        return decoder;
    }

    auto &data = decoder.value()->raw_data;
    auto length = error_length.value();
    data.resize(length);
    input_stream.read(reinterpret_cast<char *>(data.data()), length);

    if (input_stream.gcount() != length)
    {
        std::stringstream error_message;
        error_message << "Number of bytes read for section with ID = " << id
                      << " (At byte " << std::hex << std::showbase << starting_byte
                      << ") does not match expected length (" << length << ")";
        return RetType(std::unexpected(error_message.str()));
    }

    return decoder;
}

Error<std::unique_ptr<SectionDecoder>> SectionDecoder::getSectionDecoderFromID(SectionID id)
{
    using RetType = Error<std::unique_ptr<SectionDecoder>>;

    switch (id)
    {
    case SECTION_ID_TYPE:
        return RetType(std::move(std::make_unique<TypeSectionDecoder>()));
    case SECTION_ID_FUNCTION:
        return RetType(std::move(std::make_unique<FunctionSectionDecoder>()));
    case SECTION_ID_MEMORY:
        return RetType(std::move(std::make_unique<MemorySectionDecoder>()));
    case SECTION_ID_GLOBAL:
        return RetType(std::move(std::make_unique<GlobalSectionDecoder>()));
    case SECTION_ID_EXPORT:
        return RetType(std::move(std::make_unique<ExportSectionDecoder>()));
    case SECTION_ID_CODE:
        return RetType(std::move(std::make_unique<CodeSectionDecoder>()));
    default:
        std::stringstream error_message;
        error_message << "Unknown section with id " << id;
        return RetType(std::unexpected(error_message.str()));
    }
}

WASM_DECODE_NAMESPACE_END
