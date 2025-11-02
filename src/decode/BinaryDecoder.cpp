#include <wasm/wasm.hpp>

#include <iostream>
#include <sstream>

WASM_DECODE_NAMESPACE_BEGIN

Error<std::unique_ptr<BinaryDecoder>> BinaryDecoder::createBinaryDecoder(const std::string &filename)
{
    using RetType = Error<std::unique_ptr<BinaryDecoder>>;

    if (options::is_verbose)
        std::cout << "Trying to read file\n";

    /* check if file exists */
    if (!std::filesystem::exists(filename))
    {
        std::stringstream error_message;
        error_message << "File \"" << filename << "\" does not exist";
        return RetType(std::unexpected(error_message.str()));
    }

    /* try to open file */
    std::ifstream input_stream(filename, std::ios::binary);
    if (!input_stream)
    {
        std::stringstream error_message;
        error_message << "Can't open file \"" << filename << "\"";
        return RetType(std::unexpected(error_message.str()));
    }

    if (options::is_verbose)
        std::cout << "Checking magic bytes\n";

    /* check magic bytes */
    {
        Byte magic_expected[4] = {0, 'a', 's', 'm'};
        Byte magic_actual[4];
        input_stream.read(reinterpret_cast<char *>(magic_actual), 4);

        if (!std::equal(std::begin(magic_expected), std::end(magic_expected), std::begin(magic_actual)))
        {
            std::stringstream error_message;
            error_message << "Magic bytes don't look like .wasm file ("
                          << std::hex << std::showbase << static_cast<std::uint32_t>(magic_actual[0]) << ","
                          << std::hex << std::showbase << static_cast<std::uint32_t>(magic_actual[1]) << ","
                          << std::hex << std::showbase << static_cast<std::uint32_t>(magic_actual[2]) << ","
                          << std::hex << std::showbase << static_cast<std::uint32_t>(magic_actual[3]) << ")";
            return RetType(std::unexpected(error_message.str()));
        }
    }

    if (options::is_verbose)
        std::cout << "Checking file version\n";

    /* check document version */
    {
        std::uint32_t version_expected = 1u;
        Byte version_raw[4];
        input_stream.read(reinterpret_cast<char *>(version_raw), 4);
        std::uint32_t version_actual = static_cast<std::uint32_t>(version_raw[0]) |
                                       (static_cast<std::uint32_t>(version_raw[1]) << 8) |
                                       (static_cast<std::uint32_t>(version_raw[2]) << 16) |
                                       (static_cast<std::uint32_t>(version_raw[3]) << 24);

        if (version_expected != version_actual)
        {
            std::stringstream error_message;
            error_message << "Only version 1 of .wasm is supported, got " << version_actual;
            return RetType(std::unexpected(error_message.str()));
        }
    }

    auto decoder = std::make_unique<BinaryDecoder>();
    while (input_stream)
    {
        int next = input_stream.peek();
        if (next == EOF)
        {
            if (options::is_verbose)
            {
                input_stream.clear();
                std::cout << "Reached the end of the file after " << std::hex << std::showbase
                          << input_stream.tellg() << " bytes\n"
                          << std::dec;
                input_stream.peek();
            }

            break;
        }

        if (options::is_verbose)
            std::cout << "Creating new decoder after reading " << std::hex << std::showbase
                      << input_stream.tellg() << " bytes\n";

        auto section = SectionDecoder::createDecoder(input_stream);
        if (!section)
        {
            std::stringstream error_message;
            error_message << "Can't create section decoder\n"
                          << "  Reason: " << section.error();
            return RetType(std::unexpected(error_message.str()));
        }

        decoder->section_decoders.push_back(std::move(section.value()));
    }

    return RetType(std::move(decoder));
};

Error<Module> BinaryDecoder::decode() const
{
    if (options::is_verbose)
        std::cout << "Starting to decode " << section_decoders.size() << " sections\n";

    Module module;
    for (std::size_t i = 0; i < section_decoders.size(); ++i)
    {
        auto is_success = section_decoders[i]->decode(module);
        if (!is_success)
        {
            std::stringstream error_message;
            error_message << "Can't decode section " << i << "\n"
                          << "  Reason: " << is_success.error();
            return Error<Module>(std::unexpected(error_message.str()));
        }

        if (options::is_verbose)
            std::cout << "Finished section " << i + 1 << " of " << section_decoders.size()
                      << " (Id = " << section_decoders[i]->getID() << ")\n";
    }

    return Error<Module>(std::move(module));
}

WASM_DECODE_NAMESPACE_END
