/**
 * \file ExportInstance.hpp
 * \brief Represents the runtime value of a function.
 */

#ifndef __WASM_EXPORT_INSTANCE_HPP__
#define __WASM_EXPORT_INSTANCE_HPP__

#include <wasm/wasm.hpp>
#include <wasm/ast/module_definitions/Export.hpp>

WASM_EXECUTE_NAMESPACE_BEGIN

class ExportInstance
{
public:
    enum ExportType
    {
        FUNC,
        GLOBAL,
        TABLE,
        MEM,
        TAG
    };

    ExportInstance(const Export &e)
        : name(e.name),
          type(static_cast<ExportType>(e.index.index())),
          address(std::visit([](auto &v)
                             { return static_cast<Addr>(v.index); },
                             e.index)) {}

    inline bool isName(const std::u32string &name) const { return name == this->name; };
    inline Addr getAddress() const { return address; };

private:
    std::u32string name;
    ExportType type;
    Addr address;
};

WASM_EXECUTE_NAMESPACE_END

#endif