#include <wasm/wasm.hpp>

WASM_NAMESPACE_BEGIN

Error<SubType> SubType::roll(std::size_t x, std::size_t n) const
{
    if (super_types.size() > 0)
    {
        return Error<SubType>(std::unexpected("SubType::roll(size_t, size_t): Subtyping not Implemented"));
    }

    SubType s(*this);
    s.is_rolled_up = true;

    return Error<SubType>(std::move(s));
}

Error<void> SubType::validate(const Module &context, std::size_t type_index) const
{
    if (super_types.size() > 1)
        return Error<void>(std::unexpected("SubType::validate(size_t): Wasm doesn't allow multiple inheritance"));

    auto comptype_valid = comptype.validate(context);
    if (!comptype_valid)
    {
        std::stringstream msg;
        msg << "SubType::validate(size_t): Represented composite-type is invalid\n"
            << "  Reason: " << comptype_valid.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    for (const auto &index : super_types)
    {
        if (index >= type_index)
        {
            std::stringstream msg;
            msg << "SubType::validate(size_t): Super-type index "
                << index << " is invalid for recursive group index " << type_index;
            return Error<void>(std::unexpected(msg.str()));
        }

        /* Here the check for an existing subtype in the context plus the matchin would occur */
        return Error<void>(std::unexpected("This implementation does not support subtyping yet"));
    }

    return Error<void>();
}

Error<std::vector<DefinedType>> RecType::roll(std::size_t index_of_recursive_group)
{
    std::vector<DefinedType> deftypes;
    const std::size_t n = subtypes.size();
    deftypes.reserve(n);

    std::shared_ptr<RecType> rolled_type(new RecType());

    for (std::size_t i = 0; i < n; ++i)
    {
        auto roll_subtype_success = subtypes[i].roll(index_of_recursive_group, n);
        if (!roll_subtype_success)
        {
            std::stringstream msg;
            msg << "RecType::validate(size_t): Invalid subtype with index " << i << "\n"
                << "  Reason: " << roll_subtype_success.error();
            return Error<std::vector<DefinedType>>(std::unexpected(msg.str()));
        }

        rolled_type->subtypes.push_back(std::move(roll_subtype_success.value()));

        deftypes.push_back(
            std::move(
                DefinedType{
                    .n = i,
                    .rectype = rolled_type,
                }));
    }

    return Error<std::vector<DefinedType>>(std::move(deftypes));
}

WASM_NAMESPACE_END
