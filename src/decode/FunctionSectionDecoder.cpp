#include <wasm/wasm.hpp>

#include <sstream>

WASM_DECODE_NAMESPACE_BEGIN

Error<void> FunctionSectionDecoder::decode(Module &module) const
{
    if (options::is_verbose)
        std::cout << "FunctionSection: starting to decode\n";

    auto data_stream = getRawDataAsStream();

    std::function<Error<TypeIndex>(InputStream &)> decode_single_index = decodeLEB128<TypeIndex>;

    auto type_list = decodeList(data_stream, decode_single_index);

    if (!type_list)
    {
        std::stringstream msg;
        msg << "Can't decode function indices\n"
            << "  Reason: " << type_list.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    module.functions.clear();
    for (const auto &type_index : type_list.value())
        module.functions.emplace_back(type_index);

    return Error<void>();
}

WASM_DECODE_NAMESPACE_END
