/**
 * \file Module.hpp
 * \brief Declaration of the Module class.
 */

#ifndef __WASM_MODULE_HPP__
#define __WASM_MODULE_HPP__

#include <wasm/wasm.hpp>
#include <wasm/execute/ModuleInstance.hpp>

#include <vector>

WASM_NAMESPACE_BEGIN

/**
 * @brief Represents one module of a WebAssembly program.
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/syntax/modules.html
 * 
 * This once the `validate` method is called this Module also acts like a context
 * for validation purposes.
 * 
 * I haven't looked at it yet, but I'm sure I will also give it an `execute` method
 * that executes the wasm code represented in this Module.
 */
class Module
{
public:
    std::vector<RecType> types;
    std::vector<Function> functions;
    std::vector<Global> globals;
    std::vector<MemoryType> memory_types;
    std::vector<Export> exports;

    /**
     * \brief Instantiates this abstract representation of a module.
     * 
     * This includes validation, import, global evaluation, data initialization, etc.
     * However, unfortunately most of it is unfinished because I don't have enought time
     * because I blew it on trying to understand subtyping validation for 8 hours and
     * decoding everything by hand instead of asking a LLM.
     */
    Error<std::shared_ptr<execute::Store>> instantiate();

    /**
     * \brief Validate this module according to the type semantics.
     *
     * The spec can be found here: https://webassembly.github.io/spec/core/valid/modules.html
     */
    Error<void> validate();

private:
    std::vector<DefinedType> defined_types;

    /**
     * @brief Validates a single type based on the ones stored in `defined_types`.
     */
    Error<void> validateSingleType(RecType &rectype);

    /**
     * @brief Validates the types stored in \ref types and populates \ref defined_types.
     * 
     * After this operation is complete the `defined_types` array can be used to validate
     * the other sections. One way to think about it is that we slowly build up a context
     * used for validation.
     * @return Possibly an error message stored in `ret.error()`
     */
    Error<void> validateTypes();
};

WASM_NAMESPACE_END

#endif
