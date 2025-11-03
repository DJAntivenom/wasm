/**
 * \file Expression.hpp
 * \brief Declaration of the Expression class.
 */

#ifndef __WASM_EXPRESSION_HPP__
#define __WASM_EXPRESSION_HPP__

#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

class Expression final
{
private:
    Instructions instructions;

public:
    Expression() = default;
    ~Expression() = default;
    Expression(const Expression &other) = default;
    Expression(Expression &&other) noexcept = default;
    Expression &operator=(const Expression &other) = default;
    Expression &operator=(Expression &&other) noexcept = default;

    Error<void> execute(execute::State &state) const;

    inline void addInstruction(const std::shared_ptr<Instruction> &elem)
    {
        instructions.push_back(elem);
    }

    inline constexpr std::size_t getInstructionCount() const { return instructions.size(); };
};

WASM_NAMESPACE_END

#endif