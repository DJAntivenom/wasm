/**
 * \file TypeDefinition.hpp
 * \brief Declaration of the TypeDefinition class.
 */

#ifndef __WASM_TYPE_DEFINITION_HPP__
#define __WASM_TYPE_DEFINITION_HPP__

#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

/**
 * @brief Abstract representation of a type definition.
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/syntax/modules.html#types
 */
struct TypeDefinition
{
    TypeDefinition(RecType t) : global_type_index_offset(0), type(t) {};

    /**
     * @brief The starting index of range the sub-types.
     *
     * Because indices of subtypes are indexed globally in a module,
     * we need an offset into the global array.
     */
    TypeIndex global_type_index_offset;

    RecType type;
};

WASM_NAMESPACE_END

#endif