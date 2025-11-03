/**
 * \file FunctionInstance.hpp
 * \brief Represents the runtime value of a function.
 */

#ifndef __WASM_FUNCTION_INSTANCE_HPP__
#define __WASM_FUNCTION_INSTANCE_HPP__

#include <wasm/wasm.hpp>
#include <wasm/ast/module_definitions/Function.hpp>

#include "Value.hpp"

WASM_EXECUTE_NAMESPACE_BEGIN

class FunctionInstance
{
public:
    FunctionInstance(const RecType &type,
                     const std::shared_ptr<ModuleInstance> &instance,
                     const std::vector<Expression> &code)
        : type(type), module_instance(instance), code(code) {}

    Error<std::vector<Value>> execute(State &state, const std::vector<Value> &arguments) const;

private:
    RecType type;
    std::shared_ptr<ModuleInstance> module_instance;
    const std::vector<Expression> &code;
};

WASM_EXECUTE_NAMESPACE_END

#endif