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

    inline constexpr bool isNum() const { return valtype.index() == 0; };
    inline constexpr bool isVec() const { return valtype.index() == 1; };
    inline constexpr bool isRef() const { return valtype.index() == 2; };

    inline constexpr const NumType &asNum() const { return std::get<0>(valtype); };
    inline constexpr const VecType &asVec() const { return std::get<1>(valtype); };
    inline constexpr const RefType &asRef() const { return std::get<2>(valtype); };

    bool operator==(const ValType &other) const
    {
        return valtype == other.valtype;
    }

    bool operator!=(const ValType &other) const
    {
        return !(*this == other);
    }

    friend std::ostream &operator<<(std::ostream &os, const ValType &obj)
    {
        switch (obj.valtype.index())
        {
        case 0:
            switch (std::get<0>(obj.valtype).numtype)
            {
            case NumType::I32:
                os << "I32";
                break;
            case NumType::I64:
                os << "I64";
                break;
            case NumType::F32:
                os << "F32";
                break;
            default:
                os << "F64";
                break;
            }
            break;
        case 1:
            os << "V128";
            break;
        case 2:
            os << std::get<2>(obj.valtype);
            break;
        default:
            break;
        }
        return os;
    }
};

WASM_NAMESPACE_END

#endif