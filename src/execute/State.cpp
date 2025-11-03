#include <wasm/wasm.hpp>

WASM_EXECUTE_NAMESPACE_BEGIN

Error<std::size_t> State::getMemorySize(MemoryIndex memidx)
{
    auto memaddr = store->translateMemoryIndex(memidx);
    if (!memaddr)
    {
        std::stringstream msg;
        msg << "State::getMemorySize(): original module index " << memidx << " can't be translated to runtime address\n"
            << "  Reason: " << memaddr.error();
        return Error<std::size_t>(std::unexpected(msg.str()));
    }

    if (store->memoryCount() <= memaddr.value())
    {
        std::stringstream msg;
        msg << "State::getMemorySize(): translated address " << memaddr.value() << " is invalid";
        return Error<std::size_t>(std::unexpected(msg.str()));
    }

    return Error<std::size_t>(store->getMemoryInstance(memaddr.value()).getSize());
}

Error<void> State::writeBytesToMemory(MemAddr memaddr, std::size_t offset, const std::vector<std::uint8_t> &bytes)
{
    if (store->memoryCount() <= memaddr)
    {
        std::stringstream msg;
        msg << "State::writeByteToMemory(): invalid memory address " << memaddr;
        return Error<void>(std::unexpected(msg.str()));
    }

    store->getMemoryInstance(memaddr).write(offset, bytes);
    return Error<void>();
}
std::vector<std::uint8_t> State::readBytesFromMemory(MemAddr memaddr, std::size_t offset, std::size_t count)
{
    return store->getMemoryInstance(memaddr).read(offset, count);
}

WASM_EXECUTE_NAMESPACE_END