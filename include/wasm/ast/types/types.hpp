#ifndef __WASM_TYPES_HPP__
#define __WASM_TYPES_HPP__

#include <wasm/wasm.hpp>

#include <variant>

WASM_NAMESPACE_BEGIN

/** \brief The index of a type in the module's registered types */
using TypeIndex = std::size_t;
using ExternIndex = std::size_t;
using FuncIndex = std::size_t;
using LocalIndex = std::size_t;
using GlobalIndex = std::size_t;
using TableIndex = std::size_t;
using MemoryIndex = std::size_t;
using TagIndex = std::size_t;
using LabelIndex = std::size_t;
using TypeUse = TypeIndex;

/// forward declarations
struct NumType;
struct VecType;

struct AbsHeapType;
struct HeapType;

struct RefType;

struct ConstType;
struct ValType;

struct ResultType;

struct BlockType;

struct I8;
struct I16;
using PackType = std::variant<I8, I16>;
struct StorageType;
struct FieldType;
struct CompositeType;

struct SubType;
struct RecType;

struct AddressType;

struct Limits;

struct GlobalType;

struct MemoryType;

WASM_NAMESPACE_END

#include "AddressType.hpp"
#include "BlockType.hpp"
#include "CompositeType.hpp"
#include "GlobalType.hpp"
#include "HeapType.hpp"
#include "Limits.hpp"
#include "MemoryType.hpp"
#include "NumType.hpp"
#include "RecursiveType.hpp"
#include "RefType.hpp"
#include "ResultType.hpp"
#include "ValueType.hpp"

#endif
