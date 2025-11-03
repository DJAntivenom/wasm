#include <wasm/wasm.hpp>

#include <sstream>

WASM_DECODE_NAMESPACE_BEGIN

extern Error<Expression> decodeExpression(SectionDecoder::InputStream &stream);

static Error<Global> decodeSingleGlobal(GlobalSectionDecoder::InputStream &stream)
{
    if (options::is_verbose)
        std::cout << "GlobalSection: Decoding one global definition\n";

    auto t = GlobalSectionDecoder::decodeValType(stream);

    if (!t)
    {
        std::stringstream msg;
        msg << "Can't decode value type\n"
            << "  Reason: " << t.error();
        return Error<Global>(std::unexpected(msg.str()));
    }

    Byte is_mutable;
    {
        auto success = GlobalSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "Can't decode mutability byte\n"
                << "  Reason: " << success.error();
            return Error<Global>(std::unexpected(msg.str()));
        }
        is_mutable = success.value();
    }

    auto expr = decodeExpression(stream);
    if (!expr)
    {
        std::stringstream msg;
        msg << "decodeSingleGlobal(): Can't decode initialization expression\n"
            << "  Reason: " << expr.error();
        return Error<Global>(std::unexpected(msg.str()));
    }

    return Error<Global>(
        Global(
            GlobalType{
                .is_mutable{is_mutable == 0x01},
                .reftype{std::move(t.value())},
            },
            expr.value()));
}

Error<void> GlobalSectionDecoder::decode(Module &module) const
{
    if (options::is_verbose)
        std::cout << "GlobalSection: Starting to decode\n";

    auto data_stream = getRawDataAsStream();

    auto global_list = decodeList(data_stream, std::function(decodeSingleGlobal));

    if (!global_list)
    {
        std::stringstream msg;
        msg << "Can't decode global variables\n"
            << "  Reason: " << global_list.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    module.globals = std::move(global_list.value());

    return Error<void>();
}

WASM_DECODE_NAMESPACE_END
