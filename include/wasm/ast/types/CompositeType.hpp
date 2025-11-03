/**
 * \file CompositeType.hpp
 */

#ifndef __WASM_COMPOSITE_TYPE_HPP__
#define __WASM_COMPOSITE_TYPE_HPP__

#include <wasm/wasm.hpp>

#include <sstream>
#include <variant>
#include <vector>

#include "ResultType.hpp"
#include "ValueType.hpp"

WASM_NAMESPACE_BEGIN

struct I8
{
};
struct I16
{
};

struct StorageType
{
    StorageType(const ValType &v) : storagetype(v) {};
    StorageType(ValType &&v) : storagetype(std::move(v)) {};
    StorageType(const PackType &v) : storagetype(v) {};
    StorageType(PackType &&v) : storagetype(std::move(v)) {};

    std::variant<ValType, PackType> storagetype;
};

struct FieldType
{
    FieldType(bool mut, const ValType &fieldtype) : is_mutable(mut), fieldtype(fieldtype) {};
    FieldType(bool mut, const PackType &fieldtype) : is_mutable(mut), fieldtype(fieldtype) {};
    FieldType(bool mut, ValType &&fieldtype) : is_mutable(mut), fieldtype(std::move(fieldtype)) {};
    FieldType(bool mut, PackType &&fieldtype) : is_mutable(mut), fieldtype(std::move(fieldtype)) {};

    Error<void> validate(const Module &context) const
    {
        (void)context;
        return Error<void>(std::unexpected("FieldType::validate(): not implemented"));
    }

    bool is_mutable;
    StorageType fieldtype;
};

struct CompositeType
{
    Error<void> validate(const Module &context) const;

    struct Struct
    {
        std::vector<FieldType> fields;

        inline Error<void> validate(const Module &context) const
        {
            for (std::size_t i = 0; i < fields.size(); ++i)
            {
                auto field_valid = fields[i].validate(context);
                if (!field_valid)
                {
                    std::stringstream msg;
                    msg << "CompositeType::Struct::validate(): Field " << i << " is invalid\n"
                        << "  Reason: " << field_valid.error();
                    return Error<void>(std::unexpected(msg.str()));
                }
            }

            return Error<void>();
        }
    };

    struct Array
    {
        FieldType elements;

        inline Error<void> validate(const Module &context) const
        {
            auto field_valid = elements.validate(context);
            if (!field_valid)
            {
                std::stringstream msg;
                msg << "CompositeType::Array::validate(): element type is invalid\n"
                    << "  Reason: " << field_valid.error();
                return Error<void>(std::unexpected(msg.str()));
            }

            return Error<void>();
        }
    };

    struct Func
    {
        ResultType parameters;
        ResultType results;

        inline Error<void> validate(const Module &context) const
        {
            auto p = parameters.validate(context);
            if (!p)
            {
                std::stringstream msg;
                msg << "CompositeType::Function::validate(): parameter type is invalid\n"
                    << "  Reason: " << p.error();
                return Error<void>(std::unexpected(msg.str()));
            }

            auto r = parameters.validate(context);
            if (!r)
            {
                std::stringstream msg;
                msg << "CompositeType::Function::validate(): parameter type is invalid\n"
                    << "  Reason: " << r.error();
                return Error<void>(std::unexpected(msg.str()));
            }

            return Error<void>();
        };
    };

    std::variant<Struct, Array, Func> comptype;
};

WASM_NAMESPACE_END

#endif
