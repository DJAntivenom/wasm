/**
 * \brief ModuleInstance.hpp
 * \brief Contains the definition of the runtime module instance class.
 */

#ifndef __WASM_MODULE_INSTANCE_HPP__
#define __WASM_MODULE_INSTANCE_HPP__

#include <wasm/wasm.hpp>

#include <vector>

WASM_EXECUTE_NAMESPACE_BEGIN

class ModuleInstance
{
public:
    friend class Store;

    ModuleInstance() = default;
    ModuleInstance(const ModuleInstance &other) = default;
    ModuleInstance(ModuleInstance &&other) = default;
    ~ModuleInstance() = default;
    ModuleInstance &operator=(const ModuleInstance &other) = default;
    ModuleInstance &operator=(ModuleInstance &&other) noexcept = default;

    inline void addGlobalAddr(const GlobalAddr &global) { globaladdr.push_back(global); };
    inline void addExport(const ExportInstance &e) { exports.push_back(e); };
    inline void addExport(ExportInstance &&e) { exports.push_back(std::move(e)); };

    inline constexpr const std::vector<ExportInstance> &getExports() const { return exports; };

private:
    std::vector<DefinedType> deftype;
    std::vector<GlobalAddr> globaladdr;
    std::vector<MemAddr> memaddr;
    std::vector<FuncAddr> funcaddr;
    std::vector<ExportInstance> exports;
};

WASM_EXECUTE_NAMESPACE_END

#endif
