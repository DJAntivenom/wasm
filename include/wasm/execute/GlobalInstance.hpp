/**
 * \file GlobalInstance.hpp
 * \brief Represents the runtime value of a global variable.
 */

#ifndef __WASM_GLOBAL_INSTANCE_HPP__
#define __WASM_GLOBAL_INSTANCE_HPP__

#include <wasm/wasm.hpp>

#include "Value.hpp"

WASM_EXECUTE_NAMESPACE_BEGIN

class GlobalInstance
{
public:
    GlobalInstance(const GlobalType &globaltype, const Value &val)
        : globaltype(globaltype), val(val) {};

private:
    GlobalType globaltype;
    Value val;
};

WASM_EXECUTE_NAMESPACE_END

#endif