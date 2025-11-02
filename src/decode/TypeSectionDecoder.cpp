#include <wasm/wasm.hpp>

#include <sstream>

WASM_DECODE_NAMESPACE_BEGIN

static Error<FieldType> decodeFieldType(TypeSectionDecoder::InputStream &stream)
{
    Byte leading_byte;
    {
        auto success = TypeSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "Can't decode leading byte\n"
                << "  Reason: " << success.error();
            return Error<FieldType>(std::unexpected(msg.str()));
        }

        leading_byte = success.value();
    }

    FieldType ft;

    switch (leading_byte)
    {
    case 0x77:
        ft.fieldtype = StorageType{.storagetype{PackType{I16()}}};
        break;
    case 0x78:
        ft.fieldtype = StorageType{.storagetype{PackType{I8()}}};
        break;
    default:
    {
        auto t = TypeSectionDecoder::decodeValType(stream.seekg(stream.tellg() - std::streampos(1)));
        if (!t)
        {
            std::stringstream msg;
            msg << "Can't decode value type\n"
                << "  Reason: " << t.error();
            return Error<FieldType>(std::unexpected(msg.str()));
        }

        ft.fieldtype = StorageType{.storagetype = std::move(t.value())};
        break;
    }
    }

    Byte is_mutable;
    {
        auto success = GlobalSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "Can't decode mutability byte\n"
                << "  Reason: " << success.error();
            return Error<FieldType>(std::unexpected(msg.str()));
        }
        is_mutable = success.value();
    }

    ft.is_mutable = (is_mutable == 0x01);
    return ft;
}

static Error<CompositeType> decodeCompositeType(TypeSectionDecoder::InputStream &stream)
{
    if (options::is_verbose)
        std::cout << "Decoding a composite type\n";

    Byte leading_byte;
    {
        auto success = TypeSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "Can't decode leading byte\n"
                << "  Reason: " << success.error();
            return Error<CompositeType>(std::unexpected(msg.str()));
        }

        leading_byte = success.value();
    }

    switch (leading_byte)
    {
    case 0x5E:
    {
        if (options::is_verbose)
            std::cout << "Composite type is array\n";

        auto ft = decodeFieldType(stream);
        if (!ft)
        {
            std::stringstream msg;
            msg << "Can't decode element type of array\n"
                << "  Reason: " << ft.error();
            return Error<CompositeType>(std::unexpected(msg.str()));
        }

        return Error<CompositeType>(CompositeType{
            .comptype{
                CompositeType::Array{
                    .elements = std::move(ft.value()),
                },
            },
        });
    }
    case 0x5F:
    {
        if (options::is_verbose)
            std::cout << "Composite type is struct\n";

        auto function_wrapper = std::function(decodeFieldType);
        auto ft = TypeSectionDecoder::decodeList(stream, function_wrapper);
        if (!ft)
        {
            std::stringstream msg;
            msg << "Can't decode field types of struct\n"
                << "  Reason: " << ft.error();
            return Error<CompositeType>(std::unexpected(msg.str()));
        }

        return Error<CompositeType>(CompositeType{
            .comptype{
                CompositeType::Struct{
                    .fields = std::move(ft.value()),
                },
            },
        });
    }
    case 0x60:
    {
        if (options::is_verbose)
            std::cout << "Composite type is func\n";

        auto function_wrapper = std::function(TypeSectionDecoder::decodeValType);
        auto arguments = TypeSectionDecoder::decodeList(stream, function_wrapper);
        if (!arguments)
        {
            std::stringstream msg;
            msg << "Can't decode function argument types\n"
                << "  Reason: " << arguments.error();
            return Error<CompositeType>(std::unexpected(msg.str()));
        }

        auto results = TypeSectionDecoder::decodeList(stream, function_wrapper);
        if (!results)
        {
            std::stringstream msg;
            msg << "Can't decode function argument types\n"
                << "  Reason: " << results.error();
            return Error<CompositeType>(std::unexpected(msg.str()));
        }

        return Error<CompositeType>(CompositeType{
            .comptype{
                CompositeType::Func{
                    .parameters{.resulttype = std::move(arguments.value())},
                    .results{.resulttype = std::move(results.value())},
                },
            },
        });
    }
    default:
    {
        std::stringstream msg;
        msg << "CompositeType: Unknown leading byte sequence: " << leading_byte;
        return Error<CompositeType>(std::unexpected(msg.str()));
    }
    }
};

static Error<SubType> decodeSingleSubType(TypeSectionDecoder::InputStream &stream)
{
    if (options::is_verbose)
        std::cout << "Decoding a subtype\n";

    Byte leading_byte;
    {
        auto success = TypeSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "Can't decode leading byte\n"
                << "  Reason: " << success.error();
            return Error<SubType>(std::unexpected(msg.str()));
        }

        leading_byte = success.value();
    }

    switch (leading_byte)
    {
    case 0x4F:
    case 0x50:
    {
        if (options::is_verbose)
            std::cout << "Subtype is: sub " << (leading_byte == 0x4F ? "final" : "") << " x* ct\n";

        auto decode_single_typeindex = std::function(TypeSectionDecoder::decodeLEB128<TypeIndex>);
        auto x = TypeSectionDecoder::decodeList(stream, decode_single_typeindex);
        if (!x)
        {
            std::stringstream msg;
            msg << "Can't decode type indices\n"
                << "  Reason: " << x.error();
            return Error<SubType>(std::unexpected(msg.str()));
        }

        auto ct = decodeCompositeType(stream);
        if (!ct)
        {
            std::stringstream msg;
            msg << "Can't decode type composite type\n"
                << "  Reason: " << ct.error();
            return Error<SubType>(std::unexpected(msg.str()));
        }

        return Error<SubType>(SubType{
            .is_final = (leading_byte == 0x4F),
            .is_rolled_up = false,
            .super_types = std::move(x.value()),
            .rolled_super_types{},
            .comptype = ct.value(),
        });
    }

    default:
    {
        if (options::is_verbose)
            std::cout << "Subtype is: sub final \"\" ct\n";

        auto ct = decodeCompositeType(stream.seekg(stream.tellg() - std::streampos(1)));
        if (!ct)
        {
            std::stringstream msg;
            msg << "Can't decode type composite type\n"
                << "  Reason: " << ct.error();
            return Error<SubType>(std::unexpected(msg.str()));
        }

        return Error<SubType>(SubType{
            .is_final = true,
            .is_rolled_up = false,
            .super_types{},
            .rolled_super_types{},
            .comptype = ct.value(),
        });
    }
    }
}

static Error<RecType> decodeSingleType(TypeSectionDecoder::InputStream &stream)
{
    using ErrType = Error<RecType>;

    if (options::is_verbose)
        std::cout << "Decoding a single type in the type section\n";

    Byte leading_byte;
    {
        auto success = TypeSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "Can't decode leading byte\n"
                << "  Reason: " << success.error();
            return ErrType(std::unexpected(msg.str()));
        }

        leading_byte = success.value();
    }

    switch (leading_byte)
    {
    case 0x4E:
    {
        RecType rec{};
        auto subtypes = TypeSectionDecoder::decodeList(stream, std::function(decodeSingleSubType));

        if (!subtypes)
        {
            std::stringstream msg;
            msg << "Can't decode subtypes\n"
                << "  Reason: " << subtypes.error();
            return ErrType(std::unexpected(msg.str()));
        }

        rec.subtypes = std::move(subtypes.value());
        return ErrType(std::move(rec));
    }
    default:
    {
        RecType rec{};
        auto subtype = decodeSingleSubType(stream.seekg(stream.tellg() - static_cast<std::streampos>(1)));

        if (!subtype)
        {
            std::stringstream msg;
            msg << "Can't decode subtype\n"
                << "  Reason: " << subtype.error();
            return ErrType(std::unexpected(msg.str()));
        }

        rec.subtypes.push_back(std::move(subtype.value()));
        return ErrType(std::move(rec));
    }
    }
};

Error<void> TypeSectionDecoder::decode(Module &module) const
{
    if (options::is_verbose)
        std::cout << "Decoding type section of " << getRawDataSize() << " bytes\n";

    auto data_stream = getRawDataAsStream();

    auto type_list = decodeList(data_stream, std::function(decodeSingleType));

    if (!type_list)
    {
        std::stringstream msg;
        msg << "Can't decode type definitions\n"
            << "  Reason: " << type_list.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    module.types = std::move(type_list.value());

    return Error<void>();
}

WASM_DECODE_NAMESPACE_END
