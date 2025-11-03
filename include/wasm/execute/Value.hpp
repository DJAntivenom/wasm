/**
 * \file Value.hpp
 * \brief Represents a runtime value.
 */

#ifndef __WASM_VALUE_HPP__
#define __WASM_VALUE_HPP__

#include <wasm/wasm.hpp>

#include <cstdint>
#include <variant>

#include "Value.hpp"

WASM_EXECUTE_NAMESPACE_BEGIN

class Value
{
    using UnderlyingType = std::variant<std::int32_t, std::int64_t>;

public:
    Value(std::int32_t i) : type{NumType::I32}, value(i) {}
    Value(std::int64_t i) : type{NumType::I64}, value(i) {}
    Value(float i) : type{NumType::I32}, value() { throw std::runtime_error("float not supported"); }
    Value(double i) : type{NumType::I64}, value() { throw std::runtime_error("float not supported"); }

    inline constexpr const ValType &getType() const { return type; };
    inline constexpr const UnderlyingType &getValue() const { return value; };

    inline constexpr std::int32_t I32() const { return std::get<0>(value); };
    inline constexpr std::int64_t I64() const { return std::get<1>(value); };

    friend std::ostream &operator<<(std::ostream &os, const Value &obj)
    {
        os << "ValType(\n"
           << "  type: " << obj.type << "\n"
           << "  value: ";
        switch (obj.value.index())
        {
        case 0:
            os << obj.I32();
            break;
        default:
            os << obj.I64();
            break;
        }
        os << "\n)";
        return os;
    }

private:
    ValType type;
    UnderlyingType value;
};

WASM_EXECUTE_NAMESPACE_END

#endif