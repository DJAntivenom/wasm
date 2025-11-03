/**
 * \file Global.hpp
 * \brief Declaration of the GlobalVariable class.
 */

#ifndef __WASM_GLOBAL_VARIABLE_HPP__
#define __WASM_GLOBAL_VARIABLE_HPP__

#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

/**
 * @brief Abstract representation of global variable
 *
 * The spec can be found here: https://webassembly.github.io/spec/core/syntax/modules.html#globals
 */
class Global
{
public:
    Global(const GlobalType &gt, const Expression &expr) : gt(gt), expr(std::move(expr)) {};

    const inline constexpr GlobalType &getGlobalType() const { return gt; };
    const inline constexpr Expression &getExpr() const { return expr; };

private:
    GlobalType gt;
    Expression expr;
};

WASM_NAMESPACE_END

#endif