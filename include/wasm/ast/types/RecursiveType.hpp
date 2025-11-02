/**
 * \file RecursiveType.hpp
 * \brief Declaration of the RecursiveType class.
 */

#ifndef __WASM_RECURSIVE_TYPE_HPP__
#define __WASM_RECURSIVE_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <sstream>
#include <vector>

#include "CompositeType.hpp"

WASM_NAMESPACE_BEGIN

/**
 * @brief Abstract representation of a defined type.
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/valid/conventions.html#syntax-deftype
 */
struct DefinedType
{
    /**
     * @brief Index in recursive group.
     */
    TypeIndex n;

    std::shared_ptr<RecType> rectype;
};

/** \brief As found in the spec */
struct SubType
{
    /**
     * \brief Get a rolled up copy of this subtype given x.
     *
     * x is the index of the recursive group.
     * n is the size of this group.
     */
    Error<SubType> roll(std::size_t x, std::size_t n) const;

    Error<void> validate(const Module &context, std::size_t type_index) const;

    /**
     * @brief If true, can't be subtyped itself.
     */
    bool is_final;

    bool is_rolled_up;

    /**
     * @brief Absolute indices of supertypes this type matches, only defined if not rolled up.
     */
    std::vector<TypeUse> super_types;
    /**
     * \brief Only valid when is_rolled_up is true.
     */
    std::vector<std::variant<DefinedType, TypeIndex>> rolled_super_types;

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

    Error<std::vector<DefinedType>> roll(std::size_t index_of_recursive_group);

    Error<void> validate(const Module &context, std::size_t x)
    {
        const std::size_t num_subtypes = subtypes.size();
        for (std::size_t i = 0; i < num_subtypes; ++i)
        {
            auto subtype_valid = subtypes[i].validate(context, x + i);
            if (!subtype_valid)
            {
                std::stringstream msg;
                msg << "RecType::validate(size_t): Invalid subtype with index " << i << "\n"
                    << "  Reason: " << subtype_valid.error();
                return Error<void>(std::unexpected(msg.str()));
            }
        }

        return Error<void>();
    }
};

WASM_NAMESPACE_END

#endif