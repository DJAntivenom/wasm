/**
 * \file wasm.hpp
 * \brief Global macros and definitions.
 */

#ifndef __WASM_WASM_HPP__
#define __WASM_WASM_HPP__

#include <string>
#include <cstddef>
#include <expected>
#include <memory>
#include <vector>

#define WASM_NAMESPACE_BEGIN \
    namespace wasm           \
    {
#define WASM_NAMESPACE_END }

WASM_NAMESPACE_BEGIN

/// Type declarations and forward definitions
/**
 * @brief An object of type Error can hold T or an error-message string.
 */
template <class T>
using Error = std::expected<T, std::string>;

class Module;

class Function;
class GlobalVariable;
class LinearMemory;
struct TypeDefinition;
struct Export;
struct Expression;
class Instruction;

using Instructions = std::vector<std::unique_ptr<Instruction>>;

namespace options
{
    extern std::string cli_filename;
    extern bool is_verbose;
}

WASM_NAMESPACE_END

#include "ast/instructions/instructions.hpp"

#include "ast/types/types.hpp"

#include "decode/decode.hpp"

#include "ast/Expression.hpp"
#include "ast/Module.hpp"

#include "ast/module_definitions/Export.hpp"
#include "ast/module_definitions/Function.hpp"
#include "ast/module_definitions/GlobalVariable.hpp"
#include "ast/module_definitions/LinearMemory.hpp"
#include "ast/module_definitions/TypeDefinition.hpp"

#endif
