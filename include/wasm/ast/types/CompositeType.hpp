/**
 * \file CompositeType.hpp
 */

#ifndef __WASM_COMPOSITE_TYPE_HPP__
#define __WASM_COMPOSITE_TYPE_HPP__

#include <wasm/wasm.hpp>

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
    std::variant<ValType, PackType> storagetype;
};

struct FieldType
{
    bool is_mutable;
    StorageType fieldtype;
};

struct CompositeType
{
    struct Struct
    {
        std::vector<FieldType> fields;
    };

    struct Array
    {
        FieldType elements;
    };

    struct Func
    {
        ResultType parameters;
        ResultType results;
    };

    std::variant<Struct, Array, Func> comptype;
};

WASM_NAMESPACE_END

#endif
