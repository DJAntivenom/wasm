#include <wasm/wasm.hpp>

#include <iterator>
#include <iostream>
#include <sstream>

WASM_NAMESPACE_BEGIN

Error<void> Module::validateSingleType(RecType &rectype)
{
    const std::size_t x = defined_types.size();

    auto roll_success = rectype.roll(x);
    if (!roll_success)
    {
        std::stringstream msg;
        msg << "Module::validateSingleType(RecType&): Can't roll type\n"
            << "  Reason: " << roll_success.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    defined_types.insert(defined_types.end(),
                         std::make_move_iterator(roll_success.value().begin()),
                         std::make_move_iterator(roll_success.value().end()));

    auto rectype_valid = rectype.validate(*this, x);
    if (!rectype_valid)
    {
        std::stringstream msg;
        msg << "Module::validateSingleType(RecType&): Type is invalid\n"
            << "  Reason: " << rectype_valid.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    return Error<void>();
}

Error<void> Module::validateTypes()
{
    if (options::is_verbose)
        std::cout << "Module::validateTypes(): Validating types\n";

    const std::size_t num_types = types.size();
    for (uint32_t i = 0; i < num_types; ++i)
    {
        auto type_success = validateSingleType(types[i]);
        if (!type_success)
        {
            std::stringstream msg;
            msg << "Module::validateTypes(): Can't validate type with index " << i << "\n"
                << "  Reason: " << type_success.error();
            return Error<void>(std::unexpected(msg.str()));
        }
    }

    return Error<void>();
}

Error<void> Module::validate()
{
    if (options::is_verbose)
        std::cout << "Module::validate(): Starting validation\n";

    auto types_success = validateTypes();
    if (!types_success)
    {
        std::stringstream msg;
        msg << "Module::validate(): Can't validate types\n"
            << "  Reason: " << types_success.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    std::cout << "[WARNING] Globals, memories and code is not typechecked!\n";

    return Error<void>();
};

WASM_NAMESPACE_END
