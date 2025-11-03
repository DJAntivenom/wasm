/**
 * \file Frame.hpp
 * \brief A frame consists of locals and a module instance.
 */

#ifndef __WASM_FRAME_HPP__
#define __WASM_FRAME_HPP__

#include <wasm/wasm.hpp>

#include <memory>
#include <vector>

#include "ModuleInstance.hpp"
#include "Value.hpp"

WASM_EXECUTE_NAMESPACE_BEGIN

class Frame
{
private:
    std::vector<Value> values;
    std::shared_ptr<ModuleInstance> moduleinst;
};

WASM_EXECUTE_NAMESPACE_END

#endif