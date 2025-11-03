/**
 * \file State.hpp
 * \brief The state contains a reference to the store and current frame.
 */

#ifndef __WASM_STATE_HPP__
#define __WASM_STATE_HPP__

#include <wasm/wasm.hpp>

#include <stack>
#include <memory>

#include "Store.hpp"
#include "Frame.hpp"

WASM_EXECUTE_NAMESPACE_BEGIN

class State
{
public:
    State(Store *store) : store(store) {};
    State(const State &other) = default;

    Error<Value> popOperand()
    {
        if (operand_stack.empty())
        {
            return Error<Value>(std::unexpected("Operand stack is empty"));
        }

        Error<Value> ret(std::move(operand_stack.top()));
        operand_stack.pop();
        return ret;
    }

    inline void pushOperand(const Value &v)
    {
        operand_stack.push(v);
    }

    inline void pushOperand(Value &&v)
    {
        operand_stack.emplace(std::move(v));
    }

    bool checkTopOperandType(const ValType &type) const
    {
        if (operand_stack.empty())
            return false;

        return operand_stack.top().getType() == type;
    }

    Error<std::size_t> getMemorySize(MemoryIndex memidx);

    Error<void> writeBytesToMemory(MemAddr memaddr, std::size_t offset, const std::vector<std::uint8_t> &bytes);
    std::vector<std::uint8_t> readBytesFromMemory(MemAddr memaddr, std::size_t offset, std::size_t count);

    inline std::size_t getOperandStackHeight() const { return operand_stack.size(); };

    inline bool topOperandIsNum() const { return !operand_stack.empty() && operand_stack.top().getType().isNum(); };

private:
    Store *store;
    Frame frame;

    std::stack<Value> operand_stack;
};

WASM_EXECUTE_NAMESPACE_END

#endif
