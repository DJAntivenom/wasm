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
    // Default constructor
    Expression() = default;

    // Destructor - default is fine as unique_ptr cleans up
    ~Expression() = default;

    // Copy constructor - deleted since unique_ptr is non-copyable
    Expression(const Expression &other) = delete;

    // Copy assignment operator - deleted for same reason
    Expression &operator=(const Expression &other) = delete;

    // Move constructor - move vector of unique_ptr
    Expression(Expression &&other) noexcept = default;

    // Move assignment operator
    Expression &operator=(Expression &&other) noexcept = default;

    // Add element (ownership transferred)
    inline void addInstruction(std::unique_ptr<Instruction> elem)
    {
        instructions.push_back(std::move(elem));
    }

    inline constexpr std::size_t getInstructionCount() const { return instructions.size(); };

    inline Instructions &&getInstructions() { return std::move(instructions); };
};

WASM_NAMESPACE_END

#endif