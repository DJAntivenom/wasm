/**
 * \file Store.hpp
 * \brief The Store contains all global state that can be manipulated.
 */

#ifndef __WASM_STORE_HPP__
#define __WASM_STORE_HPP__

#include <wasm/wasm.hpp>

#include <iostream>
#include <vector>

WASM_EXECUTE_NAMESPACE_BEGIN

class Store
{
public:
    inline GlobalAddr allocateGlobal(GlobalInstance &&global_instance)
    {
        std::cout << "[WARNING] Store::allocateGlobal(): Type should be replaced with defined type\n";
        globalinst.emplace_back(std::move(global_instance));
        return globalinst.size() - 1;
    }

    Error<MemAddr> translateMemoryIndex(MemoryIndex idx);

    inline std::size_t memoryCount() const { return meminst.size(); };
    inline MemoryInstance &getMemoryInstance(MemAddr index) { return meminst[index]; };

    void addMemoryInstance(const MemoryInstance &m);
    inline void addFunctionInstance(const FunctionInstance &f) { funcinst.push_back(f); };

    inline void replaceModuleInstance(const std::shared_ptr<ModuleInstance> &m) { module_instance = m; };

    Error<State> runFunction(const std::u32string &name, const std::vector<Value> &args);

private:
    std::vector<GlobalInstance> globalinst;
    std::vector<MemoryInstance> meminst;
    std::vector<FunctionInstance> funcinst;

    std::shared_ptr<ModuleInstance> module_instance;
};

WASM_EXECUTE_NAMESPACE_END

#endif