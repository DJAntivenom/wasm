#include <wasm/wasm.hpp>

#include <sstream>

WASM_DECODE_NAMESPACE_BEGIN

Error<void> MemorySectionDecoder::decode(Module &module) const
{
    if (options::is_verbose)
        std::cout << "MemorySection: Starting to decode\n";

    auto data_stream = getRawDataAsStream();

    std::function<Error<MemoryType>(InputStream &)> decode_single_memory = decodeLimits;

    auto memory_list = decodeList(data_stream, decode_single_memory);

    if (!memory_list)
    {
        std::stringstream msg;
        msg << "Can't decode memory types\n"
            << "  Reason: " << memory_list.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    module.memory_types.clear();
    for (const auto &memory_type : memory_list.value())
        module.memory_types.emplace_back(memory_type);

    return Error<void>();
}

WASM_DECODE_NAMESPACE_END
