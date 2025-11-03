#include <wasm/wasm.hpp>

#include <algorithm>
#include <iterator>

WASM_NAMESPACE_BEGIN

namespace options
{
    std::string cli_filename = "";
    bool is_verbose = false;
    std::vector<RunFunction> functions_to_run;
}

std::string getName(const std::u32string &s)
{
    std::string s_;
    s_.reserve(s.size());
    std::transform(s.cbegin(), s.cend(), std::back_inserter(s_), [](char32_t c)
                   { return c > 127 ? '_' : static_cast<char>(c); });
    return s_;
}

WASM_NAMESPACE_END
