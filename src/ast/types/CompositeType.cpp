#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

Error<void> CompositeType::validate(const Module &context) const
{
    auto type_valid = std::visit([&context](const auto &t)
                                 { return t.validate(context); }, comptype);

    if (!type_valid)
    {
        std::stringstream msg;
        msg << "CompositeType::validate(): type is invalid\n"
            << "  Reason: " << type_valid.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    return Error<void>();
}

WASM_NAMESPACE_END
