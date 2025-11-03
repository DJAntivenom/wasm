/**
 * \file MemoryInstance.hpp
 */

#ifndef __WASM_MEMORY_INSTANCE_HPP__
#define __WASM_MEMORY_INSTANCE_HPP__

#include <wasm/wasm.hpp>

#include <vector>

WASM_EXECUTE_NAMESPACE_BEGIN

class MemoryInstance
{
public:
    MemoryInstance(const MemoryType &type)
        : type(type), bytes(type.limits.lower * (1 << 16), 0) {};
    MemoryInstance(const MemoryInstance &other) = default;
    MemoryInstance(MemoryInstance &&other) = default;
    ~MemoryInstance() = default;
    MemoryInstance &operator=(const MemoryInstance &other) = default;
    MemoryInstance &operator=(MemoryInstance &&other) noexcept = default;

    void write(std::size_t offset, const std::vector<std::uint8_t> &bytes)
    {
        std::size_t num_bytes = bytes.size();
        for (std::size_t i = 0; i < num_bytes && i + offset < this->bytes.size(); ++i)
        {
            this->bytes[i + offset] = bytes[i];
        }
    }

    std::vector<std::uint8_t> read(std::size_t offset, std::size_t num_bytes)
    {
        const std::vector<std::uint8_t> bytes;
        for (std::size_t i = 0; i < num_bytes && i + offset < this->bytes.size(); ++i)
        {
            this->bytes[i + offset] = bytes[i];
        }
        return bytes;
    }
    inline std::size_t getSize() const { return bytes.size(); };

private:
    MemoryType type;
    std::vector<std::uint8_t> bytes;
};

WASM_EXECUTE_NAMESPACE_END

#endif