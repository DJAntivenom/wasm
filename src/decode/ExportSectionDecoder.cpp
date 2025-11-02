#include <wasm/wasm.hpp>

#include <sstream>

WASM_DECODE_NAMESPACE_BEGIN

static Error<Export> decodeSingleExport(ExportSectionDecoder::InputStream &stream)
{
    auto name = ExportSectionDecoder::decodeName(stream);
    if (!name)
    {
        std::stringstream msg;
        msg << "ExportSection: Can't decode name of export\n"
            << "  Reason: " << name.error();
        return Error<Export>(std::unexpected(msg.str()));
    }

    Byte leading_byte;
    {
        auto success = TypeSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "ExportSection: Can't decode leading byte of export index\n"
                << "  Reason: " << success.error();
            return Error<Export>(std::unexpected(msg.str()));
        }

        leading_byte = success.value();
    }

    if (leading_byte > 0x04)
    {
        std::stringstream msg;
        msg << "ExportSection: Unknown leading byte "
            << std::hex << std::showbase << leading_byte
            << " for extern index";
        return Error<Export>(std::unexpected(msg.str()));
    }

    auto index = ExportSectionDecoder::decodeLEB128<FuncIndex>(stream);
    if (!index)
    {
        std::stringstream msg;
        msg << "ExportSection: Can't decode extern index\n"
            << "  Reason: " << index.error();
        return Error<Export>(std::unexpected(msg.str()));
    }

    switch (leading_byte)
    {
    case 0x00:
        return Error<Export>(Export{
            .name = name.value(),
            .index{Export::Func{.index = index.value()}},
        });
    case 0x01:
        return Error<Export>(Export{
            .name = name.value(),
            .index{Export::Table{.index = index.value()}},
        });
    case 0x02:
        return Error<Export>(Export{
            .name = name.value(),
            .index{Export::Memory{.index = index.value()}},
        });
    case 0x03:
        return Error<Export>(Export{
            .name = name.value(),
            .index{Export::Global{.index = index.value()}},
        });
    default:
        return Error<Export>(Export{
            .name = name.value(),
            .index{Export::Tag{.index = index.value()}},
        });
    }
}

Error<void> ExportSectionDecoder::decode(Module &module) const
{
    if (options::is_verbose)
        std::cout << "ExportSection: Starting to decode\n";

    auto data_stream = getRawDataAsStream();

    std::function<Error<Export>(InputStream &)> decode_single_memory = decodeSingleExport;

    auto export_list = decodeList(data_stream, decode_single_memory);

    if (!export_list)
    {
        std::stringstream msg;
        msg << "ExportSection: Can't decode exports\n"
            << "  Reason: " << export_list.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    module.exports = std::move(export_list.value());

    return Error<void>();
}

WASM_DECODE_NAMESPACE_END
