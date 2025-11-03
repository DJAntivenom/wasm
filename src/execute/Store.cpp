#include <wasm/wasm.hpp>

WASM_EXECUTE_NAMESPACE_BEGIN

void Store::addMemoryInstance(const MemoryInstance &m)
{
    module_instance->memaddr.push_back(meminst.size());
    meminst.push_back(m);
};

Error<MemAddr> Store::translateMemoryIndex(MemoryIndex idx)
{
    if (idx >= module_instance->memaddr.size())
    {
        std::stringstream msg;
        msg << "Store::translateMemoryIndex(): index " << idx << " is not a valid memory in module";
        return Error<MemAddr>(std::unexpected(msg.str()));
    }

    return Error<MemAddr>(module_instance->memaddr[idx]);
}

Error<State> Store::runFunction(const std::u32string &name, const std::vector<Value> &args)
{
    using RetType = Error<State>;

    FuncAddr f = -1;
    for (const auto &e : module_instance->getExports())
    {
        if (e.isName(name))
        {
            f = e.getAddress();
            break;
        }
    }

    if (f == static_cast<FuncAddr>(-1))
    {
        std::stringstream msg;
        msg << "Store::runFunction(): No function named " << getName(name);
        return RetType(std::unexpected(msg.str()));
    }

    if (options::is_verbose)
        std::cout << "Running function with address " << f << "\n";

    if (f >= funcinst.size())
    {
        std::stringstream msg;
        msg << "Store::runFunction(): No function with address " << f;
        return RetType(std::unexpected(msg.str()));
    }

    FunctionInstance &function = funcinst.at(f);
    State state(this);
    auto function_success = function.execute(state, args);
    if (!function_success)
    {
        return RetType(std::unexpected(function_success.error()));
    }

    return RetType(state);
}

WASM_EXECUTE_NAMESPACE_END
