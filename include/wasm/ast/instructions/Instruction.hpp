/**
 * \file Instruction.hpp
 * \brief Declaration of the Instruction class.
 */

#ifndef __WASM_INSTRUCTION_HPP__
#define __WASM_INSTRUCTION_HPP__

#include <wasm/wasm.hpp>
#include <wasm/ast/types/types.hpp>

#include <cstdint>

WASM_NAMESPACE_BEGIN

class Instruction
{
public:
    virtual ~Instruction() {};
};

struct MemArg
{
    std::uint32_t align;
    std::uint64_t offset;
};

///////////////////////////////
///////////////////////////////
/// PARAMETRIC INSTRUCTIONS ///
///////////////////////////////
///////////////////////////////
class Unreachable : public Instruction
{
};
class Nop : public Instruction
{
};
class Drop : public Instruction
{
};
class Select : public Instruction
{
public:
    Select() = default;
    Select(const std::vector<ValType> &v) : valtypes(v) {};
    Select(const Select &o) = default;
    Select(Select &&o) = default;

private:
    /** Can have length 0 */
    std::vector<ValType> valtypes;
};

////////////////////////////
////////////////////////////
/// CONTROL INSTRUCTIONS ///
////////////////////////////
////////////////////////////
class Block : public Instruction
{
public:
    Block(const BlockType &bt) : bt(bt) {}
    Block(const BlockType &bt, Instructions &&instr)
        : bt(bt), instr(std::move(instr)) {}

    // Destructor - default is fine as unique_ptr cleans up
    ~Block() = default;

    // Copy constructor - deleted since unique_ptr is non-copyable
    Block(const Block &other) = delete;

    // Copy assignment operator - deleted for same reason
    Block &operator=(const Block &other) = delete;

    // Move constructor - move vector of unique_ptr
    Block(Block &&other) noexcept = default;

    // Move assignment operator
    Block &operator=(Block &&other) noexcept = default;

private:
    BlockType bt;
    Instructions instr;
};

class Loop : public Instruction
{
public:
    Loop(const BlockType &bt) : bt(bt) {}
    Loop(const BlockType &bt, Instructions &&instr)
        : bt(bt), instr(std::move(instr)) {}

    // Destructor - default is fine as unique_ptr cleans up
    ~Loop() = default;

    // Copy constructor - deleted since unique_ptr is non-copyable
    Loop(const Loop &other) = delete;

    // Copy assignment operator - deleted for same reason
    Loop &operator=(const Loop &other) = delete;

    // Move constructor - move vector of unique_ptr
    Loop(Loop &&other) noexcept = default;

    // Move assignment operator
    Loop &operator=(Loop &&other) noexcept = default;

private:
    BlockType bt;
    Instructions instr;
};

class If : public Instruction
{
public:
    If(const BlockType &bt) : bt(bt) {}
    If(const BlockType &bt, Instructions &&true_instr)
        : bt(bt), true_instructions(std::move(true_instr)) {}
    If(const BlockType &bt, Instructions &&true_instr, Instructions &&false_instr)
        : bt(bt), true_instructions(std::move(true_instr)), false_instructions(std::move(false_instr)) {}

    // Destructor - default is fine as unique_ptr cleans up
    ~If() = default;

    // Copy constructor - deleted since unique_ptr is non-copyable
    If(const If &other) = delete;

    // Copy assignment operator - deleted for same reason
    If &operator=(const If &other) = delete;

    // Move constructor - move vector of unique_ptr
    If(If &&other) noexcept = default;

    // Move assignment operator
    If &operator=(If &&other) noexcept = default;

private:
    BlockType bt;
    Instructions true_instructions;
    Instructions false_instructions;
};

class Br : public Instruction
{
public:
    Br(const LabelIndex &l) : l(l) {}
    ~Br() = default;
    Br(const Br &other) = default;
    Br &operator=(const Br &other) = default;
    Br(Br &&other) noexcept = default;
    Br &operator=(Br &&other) noexcept = default;

private:
    LabelIndex l;
};

class Br_If : public Instruction
{
public:
    Br_If(const LabelIndex &l) : l(l) {}
    ~Br_If() = default;
    Br_If(const Br_If &other) = default;
    Br_If &operator=(const Br_If &other) = default;
    Br_If(Br_If &&other) noexcept = default;
    Br_If &operator=(Br_If &&other) noexcept = default;

private:
    LabelIndex l;
};

class Br_Table : public Instruction
{
public:
    Br_Table(const LabelIndex &l, const std::vector<LabelIndex> &labels)
        : labels(labels), default_label(l) {}
    Br_Table(const LabelIndex &l, std::vector<LabelIndex> &&labels)
        : labels(labels), default_label(l) {}
    Br_Table(const LabelIndex &l) : default_label(l) {}
    ~Br_Table() = default;
    Br_Table(const Br_Table &other) = default;
    Br_Table &operator=(const Br_Table &other) = default;
    Br_Table(Br_Table &&other) noexcept = default;
    Br_Table &operator=(Br_Table &&other) noexcept = default;

private:
    std::vector<LabelIndex> labels;
    LabelIndex default_label;
};

///////////////////////////
///////////////////////////
/// MEMORY INSTRUCTIONS ///
///////////////////////////
///////////////////////////
template <unsigned bit_width = 32>
class I32Store : public Instruction
{
private:
    static_assert(bit_width <= 32);

    const MemoryIndex memindex;
    const MemArg memarg;

public:
    I32Store(const MemoryIndex &memindex, const MemArg &memarg)
        : memindex(memindex), memarg(memarg) {};
    I32Store(const I32Store &other) = default;
};

template <unsigned bit_width = 32, bool is_signed = false>
class I32Load : public Instruction
{
private:
    static_assert(bit_width <= 32);

    const MemoryIndex memindex;
    const MemArg memarg;

public:
    I32Load(const MemoryIndex &memindex, const MemArg &memarg)
        : memindex(memindex), memarg(memarg) {};
    I32Load(const I32Load &other) = default;
};

template <unsigned bit_width = 64>
class I64Store : public Instruction
{
private:
    static_assert(bit_width <= 64);

    const MemoryIndex memindex;
    const MemArg memarg;

public:
    I64Store(const MemoryIndex &memindex, const MemArg &memarg)
        : memindex(memindex), memarg(memarg) {};
    I64Store(const I64Store &other) = default;
};

template <unsigned bit_width = 64, bool is_signed = false>
class I64Load : public Instruction
{
private:
    static_assert(bit_width <= 64);

    const MemoryIndex memindex;
    const MemArg memarg;

public:
    I64Load(const MemoryIndex &memindex, const MemArg &memarg)
        : memindex(memindex), memarg(memarg) {};
    I64Load(const I64Load &other) = default;
};

/////////////////////////////
/////////////////////////////
/// VARIABLE INSTRUCTIONS ///
/////////////////////////////
/////////////////////////////
class LocalGet : public Instruction
{
private:
    const LocalIndex x;

public:
    LocalGet(const LocalIndex &x) : x(x) {};
    LocalGet(const LocalGet &other) = default;
    LocalGet(LocalGet &&other) = default;
};

class LocalSet : public Instruction
{
private:
    const LocalIndex x;

public:
    LocalSet(const LocalIndex &x) : x(x) {};
    LocalSet(const LocalSet &other) = default;
    LocalSet(LocalSet &&other) = default;
};

class LocalTee : public Instruction
{
private:
    const LocalIndex x;

public:
    LocalTee(const LocalIndex &x) : x(x) {};
    LocalTee(const LocalTee &other) = default;
    LocalTee(LocalTee &&other) = default;
};

class GlobalGet : public Instruction
{
private:
    const GlobalIndex x;

public:
    GlobalGet(const LocalIndex &x) : x(x) {};
    GlobalGet(const GlobalGet &other) = default;
    GlobalGet(GlobalGet &&other) = default;
};

class GlobalSet : public Instruction
{
private:
    const GlobalIndex x;

public:
    GlobalSet(const LocalIndex &x) : x(x) {};
    GlobalSet(const GlobalSet &other) = default;
    GlobalSet(GlobalSet &&other) = default;
};

//////////////////////////
//////////////////////////
/// CONST INSTRUCTIONS ///
//////////////////////////
//////////////////////////
class I32Const : public Instruction
{
private:
    const std::uint32_t n;

public:
    I32Const(std::uint32_t n) : n(n) {};
    I32Const(const I32Const &other) = default;
};

class I64Const : public Instruction
{
private:
    const std::uint64_t n;

public:
    I64Const(std::uint64_t n) : n(n) {};
    I64Const(const I64Const &other) = default;
};

///////////////////////////////
/// COMPARISON INSTRUCTIONS ///
///////////////////////////////
class I32Eqz : public Instruction
{
};
class I32Eq : public Instruction
{
};
class I32Ne : public Instruction
{
};
class I32Lt_s : public Instruction
{
};
class I32Lt_u : public Instruction
{
};
class I32Gt_s : public Instruction
{
};
class I32Gt_u : public Instruction
{
};
class I32Le_s : public Instruction
{
};
class I32Le_u : public Instruction
{
};
class I32Ge_s : public Instruction
{
};
class I32Ge_u : public Instruction
{
};
class I64Eqz : public Instruction
{
};
class I64Eq : public Instruction
{
};
class I64Ne : public Instruction
{
};
class I64Lt_s : public Instruction
{
};
class I64Lt_u : public Instruction
{
};
class I64Gt_s : public Instruction
{
};
class I64Gt_u : public Instruction
{
};
class I64Le_s : public Instruction
{
};
class I64Le_u : public Instruction
{
};
class I64Ge_s : public Instruction
{
};
class I64Ge_u : public Instruction
{
};

/////////////////////////////////////
/// UNARY AND BINARY INSTRUCTIONS ///
/////////////////////////////////////
class I32Clz : public Instruction
{
};
class I32Ctz : public Instruction
{
};
class I32Popcnt : public Instruction
{
};
class I32Add : public Instruction
{
};
class I32Sub : public Instruction
{
};
class I32Mul : public Instruction
{
};
class I32Div_s : public Instruction
{
};
class I32Div_u : public Instruction
{
};
class I32Rem_s : public Instruction
{
};
class I32Rem_u : public Instruction
{
};
class I32And : public Instruction
{
};
class I32Or : public Instruction
{
};
class I32Xor : public Instruction
{
};
class I32Shl : public Instruction
{
};
class I32Shr_s : public Instruction
{
};
class I32Shr_u : public Instruction
{
};
class I32Rotl : public Instruction
{
};
class I32Rotr : public Instruction
{
};
//////// 64 bit /////////
class I64Clz : public Instruction
{
};
class I64Ctz : public Instruction
{
};
class I64Popcnt : public Instruction
{
};
class I64Add : public Instruction
{
};
class I64Sub : public Instruction
{
};
class I64Mul : public Instruction
{
};
class I64Div_s : public Instruction
{
};
class I64Div_u : public Instruction
{
};
class I64Rem_s : public Instruction
{
};
class I64Rem_u : public Instruction
{
};
class I64And : public Instruction
{
};
class I64Or : public Instruction
{
};
class I64Xor : public Instruction
{
};
class I64Shl : public Instruction
{
};
class I64Shr_s : public Instruction
{
};
class I64Shr_u : public Instruction
{
};
class I64Rotl : public Instruction
{
};
class I64Rotr : public Instruction
{
};

WASM_NAMESPACE_END

#endif