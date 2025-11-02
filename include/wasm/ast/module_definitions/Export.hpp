/**
 * \file Export.hpp
 * \brief Declaration of the Export class.
 */

#ifndef __WASM_EXPORT_HPP__
#define __WASM_EXPORT_HPP__

#include <wasm/wasm.hpp>

#include <string>
#include <variant>

WASM_NAMESPACE_BEGIN

struct Export
{
    struct Func
    {
        FuncIndex index;
    };
    struct Global
    {
        GlobalIndex index;
    };
    struct Table
    {
        TableIndex index;
    };
    struct Memory
    {
        MemoryIndex index;
    };
    struct Tag
    {
        TagIndex index;
    };

    std::u32string name;

    std::variant<Func, Global, Table, Memory, Tag> index;
};

WASM_NAMESPACE_END

#endif