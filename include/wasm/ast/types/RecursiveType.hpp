/**
 * \file RecursiveType.hpp
 * \brief Declaration of the RecursiveType class.
 */

#ifndef __WASM_RECURSIVE_TYPE_HPP__
#define __WASM_RECURSIVE_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <vector>

WASM_NAMESPACE_BEGIN

/** \brief As found in the spec */
struct SubType
{
    /**
     * @brief If true, can't be subtyped itself.
     */
    bool is_final;

    /**
     * @brief Indices of supertypes this type matches.
     */
    std::vector<TypeUse> typeuse;

    CompositeType comptype;
};

/**
 * @brief Abstract representation of a recursive type.
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/syntax/types.html#syntax-rectype
 */
struct RecType
{
    std::vector<SubType> subtypes;
};

WASM_NAMESPACE_END

#endif