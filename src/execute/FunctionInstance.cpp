#include <wasm/wasm.hpp>

#include <iterator>
#include <iostream>
#include <sstream>

WASM_EXECUTE_NAMESPACE_BEGIN

Error<std::vector<Value>> FunctionInstance::execute(State &state, const std::vector<Value> &arguments) const
{
    using RetType = Error<std::vector<Value>>;

    for (const auto &e : code)
    {
        auto e_success = e.execute(state);
        if (!e_success)
        {
            std::stringstream msg;
            msg << "FunctionInstance::execute(): expression failed\n"
                << "  Reason: " << e_success.error();
            return RetType(std::unexpected(msg.str()));
        }
    }

    const auto expected_return_count = std::get<2>(type.subtypes[0].comptype.comptype).results.resulttype.size();
    const auto actual_return_count = state.getOperandStackHeight();
    if (expected_return_count != actual_return_count)
    {
        std::stringstream msg;
        msg << "FunctionInstance::execute(): number of values on the operand stack does not match number indicated by function type"
            << "  Expected = " << expected_return_count << ", actual = " << actual_return_count;
        return RetType(std::unexpected(msg.str()));
    }

    std::vector<Value> return_values;
    return_values.reserve(actual_return_count);
    auto top = state.popOperand();
    while (top)
    {
        return_values.push_back(top.value());
        top = state.popOperand();
    }

    return RetType(std::move(return_values));
};

WASM_EXECUTE_NAMESPACE_END
