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

#define WASM_EXECUTE_NAMESPACE_BEGIN \
    WASM_NAMESPACE_BEGIN             \
    namespace execute                \
    {
#define WASM_EXECUTE_NAMESPACE_END \
    WASM_NAMESPACE_END             \
    }

#define WASM_DECODE_NAMESPACE_BEGIN \
    WASM_NAMESPACE_BEGIN            \
    namespace decode                \
    {
#define WASM_DECODE_NAMESPACE_END \
    WASM_NAMESPACE_END            \
    }

WASM_NAMESPACE_BEGIN

/// Type declarations and forward definitions
/**
 * @brief An object of type Error can hold T or an error-message string.
 */
template <class T>
using Error = std::expected<T, std::string>;

class Module;

class Function;
class Global;
class LinearMemory;
struct TypeDefinition;
struct Export;
struct Expression;
class Instruction;

using Instructions = std::vector<std::shared_ptr<Instruction>>;

namespace options
{
    struct I32
    {
        int32_t value;
    };
    struct I64
    {
        int64_t value;
    };
    struct F32
    {
        float value;
    };
    struct F64
    {
        double value;
    };

    using FuncArg = std::variant<I32, I64, F32, F64>;
    struct RunFunction
    {
        std::string name;
        std::vector<FuncArg> args;
    };

    extern std::string cli_filename;
    extern bool is_verbose;
    extern std::vector<RunFunction> functions_to_run;
}

std::string getName(const std::u32string &);

WASM_NAMESPACE_END

WASM_DECODE_NAMESPACE_BEGIN

/// forward declarations and types

/**
 * @brief A byte can be represented directly.
 */
using Byte = std::uint8_t;

template <class DecodedType>
class Decoder;
class BinaryDecoder;
class SectionDecoder;

WASM_DECODE_NAMESPACE_END

WASM_EXECUTE_NAMESPACE_BEGIN

using Addr = std::size_t;
using FuncAddr = Addr;
using MemAddr = Addr;
using GlobalAddr = Addr;

class Frame;
class ExportInstance;
class FunctionInstance;
class GlobalInstance;
class MemoryInstance;
class ModuleInstance;
class State;
class Store;
class Value;

WASM_EXECUTE_NAMESPACE_END

#include "ast/instructions/instructions.hpp"
#include "ast/types/types.hpp"

#include "decode/decode.hpp"

#include "execute/execute.hpp"

#include "ast/Expression.hpp"
#include "ast/Module.hpp"

#include "ast/module_definitions/Export.hpp"
#include "ast/module_definitions/Function.hpp"
#include "ast/module_definitions/Global.hpp"
#include "ast/module_definitions/LinearMemory.hpp"
#include "ast/module_definitions/TypeDefinition.hpp"

#endif
