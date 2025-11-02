#ifndef __WASM_RESULT_TYPE_HPP__
#define __WASM_RESULT_TYPE_HPP__

#include <wasm/wasm.hpp>
#include <vector>

WASM_NAMESPACE_BEGIN

struct ResultType
{
    std::vector<ValType> resulttype;

    inline Error<void> validate(const Module &context) const
    {
        for (std::size_t i = 0; i < resulttype.size(); ++i)
        {
            auto result_valid = resulttype[i].validate(context);
            if (!result_valid)
            {
                std::stringstream msg;
                msg << "ResultType::validate(): type " << i << " is invalid\n"
                    << "  Reason: " << result_valid.error();
                return Error<void>(std::unexpected(msg.str()));
            }
        }

        return Error<void>();
    };
};

WASM_NAMESPACE_END

#endif
