#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

Error<void> Expression::execute(execute::State &state) const
{
    using RetType = Error<void>;

    for (const auto &instr : instructions)
    {
        auto execution_valid = instr->execute(state);
        if (!execution_valid)
        {
            std::stringstream msg;
            msg << "Expression::execute(): instruction failed to execute\n"
                << "  Reason: " << execution_valid.error();
            return RetType(std::unexpected(msg.str()));
        }
    }

    return RetType();
}

WASM_NAMESPACE_END
