#ifndef __WASM_RESULT_TYPE_HPP__
#define __WASM_RESULT_TYPE_HPP__

#include <wasm/wasm.hpp>
#include <vector>

WASM_NAMESPACE_BEGIN

struct ResultType
{
    std::vector<ValType> resulttype;
};

WASM_NAMESPACE_END

#endif
