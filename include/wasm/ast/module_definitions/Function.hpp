/**
 * \file Function.hpp
 * \brief Declaration of the Function class.
 */

#ifndef __WASM_FUNCTION_HPP__
#define __WASM_FUNCTION_HPP__

#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

/**
 * @brief Abstract representation of a function
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/syntax/modules.html#functions
 */
class Function
{
public:
    Function(const TypeIndex &type) : type(type) {};

    std::vector<ValType> locals;

    void addExpression(Expression &&e) { body.push_back(std::move(e)); };

private:
    TypeIndex type;

    std::vector<Expression> body;
};

WASM_NAMESPACE_END

#endif