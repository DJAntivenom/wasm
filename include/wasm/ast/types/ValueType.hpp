/**
 * \file ValueType.hpp
 * \brief Declaration of the ValType class.
 */

#ifndef __WASM_VALUE_TYPE_HPP__
#define __WASM_VALUE_TYPE_HPP__

#include <wasm/wasm.hpp>

#include "NumType.hpp"
#include "RefType.hpp"

WASM_NAMESPACE_BEGIN

struct ConstType
{
    std::variant<NumType, VecType> valtype;
};

struct ValType
{
    std::variant<NumType, VecType, RefType> valtype;

    inline Error<void> validate(const Module &context) const
    {
        (void)context;
        return Error<void>(std::unexpected("ValType::validate(): not implemented\n"));
    };
};

WASM_NAMESPACE_END

#endif