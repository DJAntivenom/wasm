/**
 * \file Instruction.hpp
 * \brief Declaration of the Instruction class.
 */

#ifndef __WASM_INSTRUCTION_HPP__
#define __WASM_INSTRUCTION_HPP__

#include <wasm/wasm.hpp>
#include <wasm/ast/types/types.hpp>
#include <wasm/execute/State.hpp>
#include <wasm/execute/Value.hpp>

#include <cstdint>
#include <type_traits>

template <typename Integer,
          typename = std::enable_if_t<std::is_integral_v<Integer> && !std::is_same_v<Integer, bool>>>
static std::vector<uint8_t> toLittleEndianBytes(Integer value)
{
    std::vector<uint8_t> bytes(sizeof(Integer));
    for (size_t i = 0; i < sizeof(Integer); ++i)
    {
        bytes[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
    }
    return bytes;
}

WASM_NAMESPACE_BEGIN

class Instruction
{
public:
    virtual ~Instruction() {};

    virtual Error<void> execute(execute::State &state) const = 0;

protected:
    template <class S>
    static inline Error<void> E(const S &s) { return Error<void>(std::unexpected(s)); };
    static inline Error<void> S() { return Error<void>(); };
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
    Error<void> execute(execute::State &state) const override { return E("Unreachable::execute(): trap"); }
};
class Nop : public Instruction
{
    Error<void> execute(execute::State &state) const override { return S(); }
};
class Drop : public Instruction
{
    Error<void> execute(execute::State &state) const override
    {
        auto pop_success = state.popOperand();
        if (!pop_success)
            return E("Drop::execute(): Empty operand stack");

        return S();
    }
};
class Select : public Instruction
{
public:
    Select() = default;
    Select(const std::vector<ValType> &v) : valtypes(v) {};
    Select(const Select &o) = default;
    Select(Select &&o) = default;

    Error<void> execute(execute::State &state) const override
    {
        if (!state.checkTopOperandType(ValType{.valtype{NumType::I32}}))
            return E("Select::execute(): Top of stack is not I32");

        /* don't need to check error, because stack can't be empty */
        std::int32_t c = state.popOperand().value().I32();

        auto v1 = state.popOperand();
        if (!v1)
            return E("Select::execute(): Stack is empty after c");

        auto v2 = state.popOperand();
        if (!v2)
            return E("Select::execute(): Stack is empty after val2");

        state.pushOperand(std::move(c != 0 ? v1.value() : v2.value()));
        return S();
    }

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
    ~Block() = default;
    Block(const Block &other) = default;
    Block(Block &&other) noexcept = default;
    Block &operator=(const Block &other) = default;
    Block &operator=(Block &&other) noexcept = default;

    Error<void> execute(execute::State &state) const override
    {
        return E("Block::execute(): not implemented");
    }

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
    ~Loop() = default;
    Loop(const Loop &other) = default;
    Loop(Loop &&other) noexcept = default;
    Loop &operator=(const Loop &other) = default;
    Loop &operator=(Loop &&other) noexcept = default;

    Error<void> execute(execute::State &state) const override
    {
        return E("Loop::execute(): not implemented");
    }

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
    ~If() = default;
    If(const If &other) = default;
    If &operator=(const If &other) = default;
    If(If &&other) noexcept = default;
    If &operator=(If &&other) noexcept = default;

    Error<void> execute(execute::State &state) const override
    {
        return E("If::execute(): not implemented");
    }

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

    Error<void> execute(execute::State &state) const override
    {
        return E("Br::execute(): not implemented");
    }

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

    Error<void> execute(execute::State &state) const override
    {
        return E("Br_If::execute(): not implemented");
    }

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

    Error<void> execute(execute::State &state) const override
    {
        return E("Br_Table::execute(): not implemented");
    }

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

    Error<void> execute(execute::State &state) const override
    {
        auto c = state.popOperand();
        if (!c)
            return E("I32Store::execute(): No byte to store on stack");

        if (!c.value().getType().isNum() || !c.value().getType().asNum().isI32())
        {
            std::stringstream msg;
            msg << "I32Store::execute(): Expected value to store to be I32 but got"
                << c.value().getType();
            return E(msg.str());
        }

        auto i_success = state.popOperand();
        if (!i_success)
            return E("I32Store::execute(): No index on stack");

        if (!i_success.value().getType().isNum())
        {
            std::stringstream msg;
            msg << "I32Store::execute(): Expected index to be an address but got"
                << i_success.value().getType();
            return E(msg.str());
        }
        auto i = i_success.value().I32();

        std::size_t memory_size = 0;
        {
            auto mem_success = state.getMemorySize(memindex);
            if (!mem_success)
            {
                std::stringstream msg;
                msg << "I32Store::execute(): Illegal memory address " << memindex << "\n"
                    << "  Reason: " << mem_success.error();
                return E(msg.str());
            }
            memory_size = mem_success.value();
        }

        if (i + memarg.offset + bit_width / 8 > memory_size)
        {
            std::stringstream msg;
            msg << "I32Store::execute(): Index " << i << " exceeds memory of size " << memory_size;
            return E(msg.str());
        }

        auto write_success = state.writeBytesToMemory(memindex,
                                                      memarg.offset,
                                                      toLittleEndianBytes(c.value().I32()));

        if (!write_success)
        {
            std::stringstream msg;
            msg << "I32Store::execute(): Can't write bytes to memory\n"
                << "  Reason: " << write_success.error();
            return E(msg.str());
        }

        return S();
    }
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

    Error<void> execute(execute::State &state) const override
    {
        return E("i32load not implemented");
    }
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

    Error<void> execute(execute::State &state) const override
    {
        auto c = state.popOperand();
        if (!c)
            return E("I64Store::execute(): No byte to store on stack");

        if (!c.value().getType().isNum() || !c.value().getType().asNum().isI32())
        {
            std::stringstream msg;
            msg << "I64Store::execute(): Expected value to store to be I64 but got"
                << c.value().getType();
            return E(msg.str());
        }

        auto i = state.popOperand();
        if (!i)
            return E("I64Store::execute(): No index on stack");

        if (!c.value().getType().isNum())
        {
            std::stringstream msg;
            msg << "I64Store::execute(): Expected index to be an address but got"
                << c.value().getType();
            return E(msg.str());
        }

        return E("I64Store::execute(): Not implementede");
    }
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

    Error<void> execute(execute::State &state) const override
    {
        return E("i64load not implemented");
    }
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

    Error<void> execute(execute::State &state) const override
    {
        return E("load.get not implemented");
    }
};

class LocalSet : public Instruction
{
private:
    const LocalIndex x;

public:
    LocalSet(const LocalIndex &x) : x(x) {};
    LocalSet(const LocalSet &other) = default;
    LocalSet(LocalSet &&other) = default;

    Error<void> execute(execute::State &state) const override
    {
        return E("load.set not implemented");
    }
};

class LocalTee : public Instruction
{
private:
    const LocalIndex x;

public:
    LocalTee(const LocalIndex &x) : x(x) {};
    LocalTee(const LocalTee &other) = default;
    LocalTee(LocalTee &&other) = default;

    Error<void> execute(execute::State &state) const override
    {
        return E("load.tee not implemented");
    }
};

class GlobalGet : public Instruction
{
private:
    const GlobalIndex x;

public:
    GlobalGet(const LocalIndex &x) : x(x) {};
    GlobalGet(const GlobalGet &other) = default;
    GlobalGet(GlobalGet &&other) = default;

    Error<void> execute(execute::State &state) const override
    {
        return E("load.get not implemented");
    }
};

class GlobalSet : public Instruction
{
private:
    const GlobalIndex x;

public:
    GlobalSet(const LocalIndex &x) : x(x) {};
    GlobalSet(const GlobalSet &other) = default;
    GlobalSet(GlobalSet &&other) = default;

    Error<void> execute(execute::State &state) const override
    {
        return E("global.set not implemented");
    }
};

//////////////////////////
//////////////////////////
/// CONST INSTRUCTIONS ///
//////////////////////////
//////////////////////////
class I32Const : public Instruction
{
private:
    const std::int32_t n;

public:
    I32Const(std::int32_t n) : n(n) {};
    I32Const(const I32Const &other) = default;

    Error<void> execute(execute::State &state) const override
    {
        state.pushOperand(execute::Value(n));
        return S();
    }
};

class I64Const : public Instruction
{
private:
    const std::int64_t n;

public:
    I64Const(std::int64_t n) : n(n) {};
    I64Const(const I64Const &other) = default;

    Error<void> execute(execute::State &state) const override
    {
        state.pushOperand(execute::Value(n));
        return S();
    }
};

///////////////////////////////
/// COMPARISON INSTRUCTIONS ///
///////////////////////////////
class I32Eqz : public Instruction
{
    Error<void> execute(execute::State &state) const override
    {
        if (!state.topOperandIsNum())
            return E("I32Eqz: expected top operand to be numbertype");

        auto v = state.popOperand().value();
        if (!v.getType().isNum() || !v.getType().asNum().isI32())
        {
            std::stringstream msg;
            msg << "I32Eqz::execute(): Expected top of stack to be I32 but got "
                << v.getType();
            return E(msg.str());
        }

        execute::Value c(v.I32() == 0);
        state.pushOperand(std::move(c));

        return S();
    }
};
class I32Eq : public Instruction
{
    Error<void> execute(execute::State &state) const override
    {
        if (!state.topOperandIsNum())
            return E("I32Eq: expected top operand to be numbertype");

        auto v = state.popOperand().value();
        if (!v.getType().isNum() || !v.getType().asNum().isI32())
        {
            std::stringstream msg;
            msg << "I32Eq::execute(): Expected top of stack to be I32 but got "
                << v.getType();
            return E(msg.str());
        }

        auto v1 = state.popOperand().value();
        if (!v1.getType().isNum() || !v1.getType().asNum().isI32())
        {
            std::stringstream msg;
            msg << "I32Eq::execute(): Expected second operand to be I32 but got "
                << v1.getType();
            return E(msg.str());
        }

        execute::Value c(v.I32() == v1.I32());
        state.pushOperand(std::move(c));

        return S();
    }
};
class I32Ne : public Instruction
{
    Error<void> execute(execute::State &state) const override
    {
        if (!state.topOperandIsNum())
            return E("I32Ne: expected top operand to be numbertype");

        auto v = state.popOperand().value();
        if (!v.getType().isNum() || !v.getType().asNum().isI32())
        {
            std::stringstream msg;
            msg << "I32Ne::execute(): Expected top of stack to be I32 but got "
                << v.getType();
            return E(msg.str());
        }

        auto v1 = state.popOperand().value();
        if (!v1.getType().isNum() || !v1.getType().asNum().isI32())
        {
            std::stringstream msg;
            msg << "I32Ne::execute(): Expected second operand to be I32 but got "
                << v1.getType();
            return E(msg.str());
        }

        execute::Value c(v.I32() != v1.I32());
        state.pushOperand(std::move(c));

        return S();
    }
};
class I32Lt_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Lt_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Gt_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Gt_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Le_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Le_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Ge_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Ge_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Eqz : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Eq : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Ne : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Lt_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Lt_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Gt_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Gt_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Le_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Le_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Ge_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Ge_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};

/////////////////////////////////////
/// UNARY AND BINARY INSTRUCTIONS ///
/////////////////////////////////////
class I32Clz : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Ctz : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Popcnt : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Add : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Sub : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Mul : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Div_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Div_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Rem_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Rem_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32And : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Or : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Xor : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Shl : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Shr_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Shr_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Rotl : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I32Rotr : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
//////// 64 bit /////////
class I64Clz : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Ctz : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Popcnt : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Add : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Sub : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Mul : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Div_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Div_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Rem_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Rem_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64And : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Or : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Xor : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Shl : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Shr_s : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Shr_u : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Rotl : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};
class I64Rotr : public Instruction
{
    Error<void> execute(execute::State &state) const override { return E("Not implemented"); };
};

WASM_NAMESPACE_END

#endif