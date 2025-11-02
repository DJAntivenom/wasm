/**
 * \file Module.hpp
 * \brief Declaration of the Module class.
 */

#ifndef __WASM_MODULE_HPP__
#define __WASM_MODULE_HPP__

#include <wasm/wasm.hpp>

#include <vector>

WASM_NAMESPACE_BEGIN

/**
 * @brief Represents one module of a WebAssembly program.
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/syntax/modules.html
 */
class Module
{
public:
    std::vector<TypeDefinition> types;
    std::vector<Function> functions;
    std::vector<GlobalType> globals;
    std::vector<MemoryType> memory_types;
    std::vector<Export> exports;
};

WASM_NAMESPACE_END

#endif
