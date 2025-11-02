#include <wasm/wasm.hpp>

#include <iterator>
#include <sstream>
#include <utility>
#include <memory>

WASM_DECODE_NAMESPACE_BEGIN

static Error<std::unique_ptr<Instruction>> decodeInstruction(CodeSectionDecoder::InputStream &stream,
                                                             bool &reached_end,
                                                             bool &reached_else)
{
    using RetType = Error<std::unique_ptr<Instruction>>;

    reached_end = false;
    reached_else = false;

    Byte op_code = 0x00;
    {
        auto success = CodeSectionDecoder::decodeByte(stream);
        if (!success)
        {
            std::stringstream msg;
            msg << "decodeInstruction: Can't decode opcode of instruction\n"
                << "  Reason: " << success.error();
            return RetType(std::unexpected(msg.str()));
        }

        op_code = success.value();
    }

    auto decode_mem_arg = [](CodeSectionDecoder::InputStream &stream)
    {
        using RetType = Error<std::pair<MemoryIndex, MemArg>>;

        auto n = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!n)
        {
            std::stringstream msg;
            msg << "decodeInstruction: Can't read n for memarg\n"
                << "  Reason: " << n.error();
            return RetType(std::unexpected(msg.str()));
        }

        std::uint32_t align = n.value();

        if (n.value() >= (1u << 7))
        {
            std::stringstream msg;
            msg << "decodeInstruction: n too large for memarg\n"
                << "  n = " << n.value() << " >= " << (1u << 7);
            return RetType(std::unexpected(msg.str()));
        }

        MemoryIndex x = 0;
        if (n.value() >= (1u << 6))
        {
            auto x_success = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
            if (!x_success)
            {
                std::stringstream msg;
                msg << "decodeInstruction: Can't read x for memarg\n"
                    << "  Reason: " << x_success.error();
                return RetType(std::unexpected(msg.str()));
            }
            x = x_success.value();

            align = n.value() - (1 << 6);
        }

        auto m = CodeSectionDecoder::decodeLEB128<std::uint64_t>(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: Can't read m for memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }

        return RetType(std::make_pair(x, MemArg{
                                             .align = align,
                                             .offset = m.value(),
                                         }));
    };

    auto decode_blocktype = [](CodeSectionDecoder::InputStream &stream)
    {
        const auto stream_starting_pos = stream.tellg();

        Byte leading_byte = 0;
        {
            auto success = CodeSectionDecoder::decodeByte(stream);
            if (!success)
            {
                std::stringstream msg;
                msg << "decodeInstruction: Can't decode leading byte of block type\n"
                    << "  Reason: " << success.error();
                return Error<BlockType>(std::unexpected(msg.str()));
            }

            leading_byte = success.value();
        }

        if (leading_byte == 0x40)
            return Error<BlockType>(BlockType{.blocktype{std::optional<ValType>()}});

        stream.seekg(stream_starting_pos);

        auto t = CodeSectionDecoder::decodeValType(stream);
        if (!t)
        {
            stream.seekg(stream_starting_pos);
            auto i = CodeSectionDecoder::decodeLEB128<std::int64_t, 33>(stream);
            if (!i)
            {
                std::stringstream msg;
                msg << "decodeInstruction: Can't decode type index of block type\n"
                    << "  Reason: " << i.error();
                return Error<BlockType>(std::unexpected(msg.str()));
            }

            return Error<BlockType>(BlockType{.blocktype{TypeIndex{static_cast<TypeIndex>(i.value())}}});
        }

        return Error<BlockType>(BlockType{.blocktype{std::optional(t.value())}});
    };

    switch (op_code)
    {
    case 0x00:
        return RetType(std::move(std::make_unique<Unreachable>()));
    case 0x01:
        return RetType(std::move(std::make_unique<Nop>()));
    case 0x02: /* block instruction */
    case 0x03: /* loop instruction */
    {
        auto bt = decode_blocktype(stream);
        if (!bt)
        {
            std::stringstream msg;
            msg << "decodeInstruction: block: Can't decode block type\n"
                << "  Reason: " << bt.error();
            return RetType(std::unexpected(msg.str()));
        }

        Instructions block;
        bool reached_else = false;
        while (true)
        {
            bool reached_end = false;
            auto instr = decodeInstruction(stream, reached_end, reached_else);
            if (reached_end || reached_else)
                break;

            if (!instr)
            {
                std::stringstream msg;
                msg << "decodeInstruction: block: Can't decode instruction " << block.size() << " in block\n"
                    << "  Reason: " << instr.error();
                return RetType(std::unexpected(msg.str()));
            }

            block.emplace_back(std::move(instr.value()));
        }

        if (reached_else)
            return RetType(std::unexpected("decodeInstruction: block: Block terminated with else opcode"));

        if (op_code == 0x02)
            return RetType(std::move(std::make_unique<Block>(bt.value(), std::move(block))));
        else
            return RetType(std::move(std::make_unique<Loop>(bt.value(), std::move(block))));
    }
    case 0x04: /* if instruction */
    {
        auto bt = decode_blocktype(stream);
        if (!bt)
        {
            std::stringstream msg;
            msg << "decodeInstruction: if: Can't decode first block type\n"
                << "  Reason: " << bt.error();
            return RetType(std::unexpected(msg.str()));
        }

        Instructions true_block;
        bool reached_else = false;
        while (true)
        {
            bool reached_end = false;
            auto instr = decodeInstruction(stream, reached_end, reached_else);
            if (reached_end || reached_else)
                break;

            if (!instr)
            {
                std::stringstream msg;
                msg << "decodeInstruction: if: Can't decode instruction " << true_block.size() << " in true block\n"
                    << "  Reason: " << instr.error();
                return RetType(std::unexpected(msg.str()));
            }

            true_block.emplace_back(std::move(instr.value()));
        }

        if (!reached_else)
            return RetType(std::move(std::make_unique<If>(bt.value(), std::move(true_block))));

        Instructions false_block;
        while (true)
        {
            bool reached_end = false;
            auto instr = decodeInstruction(stream, reached_end, reached_else);
            if (reached_end || reached_else)
                break;

            if (!instr)
            {
                std::stringstream msg;
                msg << "decodeInstruction: if: Can't decode instruction " << false_block.size() << " in else block\n"
                    << "  Reason: " << instr.error();
                return RetType(std::unexpected(msg.str()));
            }

            false_block.emplace_back(std::move(instr.value()));
        }

        if (reached_else)
            return RetType(std::unexpected("decodeInstruction: if: If instruction has multiple else blocks"));

        return RetType(
            std::move(
                std::make_unique<If>(bt.value(),
                                     std::move(true_block),
                                     std::move(false_block))));
    }
    case 0x05:
    {
        reached_else = true;
        /* doesn't matter what we return in this case */
        return RetType(std::unexpected(""));
    }
    case 0x0B:
    {
        reached_end = true;
        /* doesn't matter what we return in this case */
        return RetType(std::unexpected(""));
    }
    case 0x0C:
    case 0x0D:
    {
        auto l = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!l)
        {
            std::stringstream msg;
            msg << "decodeInstruction: br_if: Can't decode label\n"
                << "  Reason: " << l.error();
            return RetType(std::unexpected(msg.str()));
        }

        if (op_code == 0x0C)
            return RetType(std::move(std::make_unique<Br>(static_cast<LabelIndex>(l.value()))));
        else
            return RetType(std::move(std::make_unique<Br_If>(static_cast<LabelIndex>(l.value()))));
    }
    case 0x0E:
    {
        auto labels = CodeSectionDecoder::decodeList(stream, std::function(CodeSectionDecoder::decodeLEB128<std::uint32_t>));
        if (!labels)
        {
            std::stringstream msg;
            msg << "decodeInstruction: br_table: Can't decode labels\n"
                << "  Reason: " << labels.error();
            return RetType(std::unexpected(msg.str()));
        }

        auto l = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!l)
        {
            std::stringstream msg;
            msg << "decodeInstruction: br_if: Can't decode label\n"
                << "  Reason: " << l.error();
            return RetType(std::unexpected(msg.str()));
        }

        std::vector<LabelIndex> cast_labels(labels.value().size());
        for (uint64_t i = 0; i < labels.value().size(); ++i)
            cast_labels[i] = static_cast<LabelIndex>(labels.value()[i]);

        return RetType(
            std::move(
                std::make_unique<Br_Table>(
                    static_cast<LabelIndex>(l.value()),
                    std::move(cast_labels))));
    }
    case 0x1A:
        return RetType(std::move(std::make_unique<Drop>()));
    case 0x1B:
        return RetType(std::move(std::make_unique<Select>()));
    case 0x1C:
    {
        auto t = CodeSectionDecoder::decodeList(stream, std::function(CodeSectionDecoder::decodeValType));
        if (!t)
        {
            std::stringstream msg;
            msg << "decodeInstruction: select t*: Can't decode value types\n"
                << "  Reason: " << t.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(
            std::move(
                std::make_unique<Select>(
                    std::move(t.value()))));
    }
    case 0x20:
    {
        auto x = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!x)
        {
            std::stringstream msg;
            msg << "decodeInstruction: local.get: Can't read index\n"
                << "  Reason: " << x.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<LocalGet>(x.value())));
    }
    case 0x21:
    {
        auto x = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!x)
        {
            std::stringstream msg;
            msg << "decodeInstruction: local.set: Can't read index\n"
                << "  Reason: " << x.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<LocalSet>(x.value())));
    }
    case 0x22:
    {
        auto x = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!x)
        {
            std::stringstream msg;
            msg << "decodeInstruction: local.tee: Can't read index\n"
                << "  Reason: " << x.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<LocalTee>(x.value())));
    }
    case 0x23:
    {
        auto x = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!x)
        {
            std::stringstream msg;
            msg << "decodeInstruction: global.get: Can't read index\n"
                << "  Reason: " << x.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<GlobalGet>(x.value())));
    }
    case 0x24:
    {
        auto x = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!x)
        {
            std::stringstream msg;
            msg << "decodeInstruction: local.set: Can't read index\n"
                << "  Reason: " << x.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<GlobalSet>(x.value())));
    }
    case 0x28: /* i32.load */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.load: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Load<32, false>>(
            m.value().first,
            m.value().second)));
    }
    case 0x29: /* i64.load */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.load: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Load<64, false>>(
            m.value().first,
            m.value().second)));
    }
    case 0x2C: /* i32.load8_s */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.load8_s: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Load<8, true>>(
            m.value().first,
            m.value().second)));
    }
    case 0x2D: /* i32.load8_u */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.load8_u: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Load<8, false>>(
            m.value().first,
            m.value().second)));
    }
    case 0x2E: /* i32.load16_s */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.load16_s: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Load<16, true>>(
            m.value().first,
            m.value().second)));
    }
    case 0x2F: /* i32.load16_u */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.load16_u: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Load<16, false>>(
            m.value().first,
            m.value().second)));
    }
    case 0x30: /* i64.load8_s */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.load8_s: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Load<8, true>>(
            m.value().first,
            m.value().second)));
    }
    case 0x31: /* i64.load8_u */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.load8_u: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Load<8, false>>(
            m.value().first,
            m.value().second)));
    }
    case 0x32: /* i64.load16_s */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.load16_s: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Load<16, true>>(
            m.value().first,
            m.value().second)));
    }
    case 0x33: /* i64.load16_u */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.load16_u: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Load<16, false>>(
            m.value().first,
            m.value().second)));
    }
    case 0x34: /* i64.load32_s */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.load32_s: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Load<32, true>>(
            m.value().first,
            m.value().second)));
    }
    case 0x35: /* i64.load32_u */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.load32_u: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Load<32, false>>(
            m.value().first,
            m.value().second)));
    }
    case 0x36: /* i32.store */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.store: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Store<32>>(
            m.value().first,
            m.value().second)));
    }
    case 0x37: /* i64.store */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.store: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Store<64>>(
            m.value().first,
            m.value().second)));
    }
    case 0x3A: /* i32.store8 */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.store8: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Store<8>>(
            m.value().first,
            m.value().second)));
    }
    case 0x3B: /* i32.store16 */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i32.store16: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Store<16>>(
            m.value().first,
            m.value().second)));
    }
    case 0x3C: /* i64.store8 */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.store8: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Store<8>>(
            m.value().first,
            m.value().second)));
    }
    case 0x3D: /* i64.store16 */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i64.store16: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Store<16>>(
            m.value().first,
            m.value().second)));
    }
    case 0x3E: /* i64.store32 */
    {
        auto m = decode_mem_arg(stream);
        if (!m)
        {
            std::stringstream msg;
            msg << "decodeInstruction: i54.store32: can't read memarg\n"
                << "  Reason: " << m.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Store<32>>(
            m.value().first,
            m.value().second)));
    }
    case 0x41: /* i32.const */
    {
        auto n = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!n)
        {
            std::stringstream msg;
            msg << "decodeInstruction: Can't read n for i32.const\n"
                << "  Reason: " << n.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I32Const>(n.value())));
    }
    case 0x42: /* i64.const */
    {
        auto n = CodeSectionDecoder::decodeLEB128<std::uint64_t>(stream);
        if (!n)
        {
            std::stringstream msg;
            msg << "decodeInstruction: Can't read n for i64.const\n"
                << "  Reason: " << n.error();
            return RetType(std::unexpected(msg.str()));
        }
        return RetType(std::move(std::make_unique<I64Const>(n.value())));
    }
    case 0x45:
        return RetType(std::move(std::make_unique<I32Eqz>()));
    case 0x46:
        return RetType(std::move(std::make_unique<I32Eq>()));
    case 0x47:
        return RetType(std::move(std::make_unique<I32Ne>()));
    case 0x48:
        return RetType(std::move(std::make_unique<I32Lt_s>()));
    case 0x49:
        return RetType(std::move(std::make_unique<I32Lt_u>()));
    case 0x4A:
        return RetType(std::move(std::make_unique<I32Gt_s>()));
    case 0x4B:
        return RetType(std::move(std::make_unique<I32Gt_u>()));
    case 0x4C:
        return RetType(std::move(std::make_unique<I32Le_s>()));
    case 0x4D:
        return RetType(std::move(std::make_unique<I32Le_u>()));
    case 0x4E:
        return RetType(std::move(std::make_unique<I32Ge_s>()));
    case 0x4F:
        return RetType(std::move(std::make_unique<I32Ge_u>()));
    case 0x50:
        return RetType(std::move(std::make_unique<I64Eqz>()));
    case 0x51:
        return RetType(std::move(std::make_unique<I64Eq>()));
    case 0x52:
        return RetType(std::move(std::make_unique<I64Ne>()));
    case 0x53:
        return RetType(std::move(std::make_unique<I64Lt_s>()));
    case 0x54:
        return RetType(std::move(std::make_unique<I64Lt_u>()));
    case 0x55:
        return RetType(std::move(std::make_unique<I64Gt_s>()));
    case 0x56:
        return RetType(std::move(std::make_unique<I64Gt_u>()));
    case 0x57:
        return RetType(std::move(std::make_unique<I64Le_s>()));
    case 0x58:
        return RetType(std::move(std::make_unique<I64Le_u>()));
    case 0x59:
        return RetType(std::move(std::make_unique<I64Ge_s>()));
    case 0x5A:
        return RetType(std::move(std::make_unique<I64Ge_u>()));
    case 0x67:
        return RetType(std::move(std::make_unique<I32Clz>()));
    case 0x68:
        return RetType(std::move(std::make_unique<I32Ctz>()));
    case 0x69:
        return RetType(std::move(std::make_unique<I32Popcnt>()));
    case 0x6A:
        return RetType(std::move(std::make_unique<I32Add>()));
    case 0x6B:
        return RetType(std::move(std::make_unique<I32Sub>()));
    case 0x6C:
        return RetType(std::move(std::make_unique<I32Mul>()));
    case 0x6D:
        return RetType(std::move(std::make_unique<I32Div_s>()));
    case 0x6E:
        return RetType(std::move(std::make_unique<I32Div_u>()));
    case 0x6F:
        return RetType(std::move(std::make_unique<I32Rem_s>()));
    case 0x70:
        return RetType(std::move(std::make_unique<I32Rem_u>()));
    case 0x71:
        return RetType(std::move(std::make_unique<I32And>()));
    case 0x72:
        return RetType(std::move(std::make_unique<I32Or>()));
    case 0x73:
        return RetType(std::move(std::make_unique<I32Xor>()));
    case 0x74:
        return RetType(std::move(std::make_unique<I32Shl>()));
    case 0x75:
        return RetType(std::move(std::make_unique<I32Shr_s>()));
    case 0x76:
        return RetType(std::move(std::make_unique<I32Shr_u>()));
    case 0x77:
        return RetType(std::move(std::make_unique<I32Rotl>()));
    case 0x78:
        return RetType(std::move(std::make_unique<I32Rotr>()));
    case 0x79:
        return RetType(std::move(std::make_unique<I64Clz>()));
    case 0x7A:
        return RetType(std::move(std::make_unique<I64Ctz>()));
    case 0x7B:
        return RetType(std::move(std::make_unique<I64Popcnt>()));
    case 0x7C:
        return RetType(std::move(std::make_unique<I64Add>()));
    case 0x7D:
        return RetType(std::move(std::make_unique<I64Sub>()));
    case 0x7E:
        return RetType(std::move(std::make_unique<I64Mul>()));
    case 0x7F:
        return RetType(std::move(std::make_unique<I64Div_s>()));
    case 0x80:
        return RetType(std::move(std::make_unique<I64Div_u>()));
    case 0x81:
        return RetType(std::move(std::make_unique<I64Rem_s>()));
    case 0x82:
        return RetType(std::move(std::make_unique<I64Rem_u>()));
    case 0x83:
        return RetType(std::move(std::make_unique<I64And>()));
    case 0x84:
        return RetType(std::move(std::make_unique<I64Or>()));
    case 0x85:
        return RetType(std::move(std::make_unique<I64Xor>()));
    case 0x86:
        return RetType(std::move(std::make_unique<I64Shl>()));
    case 0x87:
        return RetType(std::move(std::make_unique<I64Shr_s>()));
    case 0x88:
        return RetType(std::move(std::make_unique<I64Shr_u>()));
    case 0x89:
        return RetType(std::move(std::make_unique<I64Rotl>()));
    case 0x8A:
        return RetType(std::move(std::make_unique<I64Rotr>()));
    default:
    {
        std::stringstream msg;
        msg << "decodeInstruction: Unknown opcode "
            << std::hex << std::showbase << static_cast<std::uint32_t>(op_code);
        return RetType(std::unexpected(msg.str()));
    }
    }
}

static Error<Expression> decodeExpression(CodeSectionDecoder::InputStream &stream)
{
    Expression expr;

    while (true)
    {
        bool reached_end = false;
        bool reached_else = false;
        auto instr = decodeInstruction(stream, reached_end, reached_else);
        if (reached_end)
            break;

        if (!instr)
        {
            std::stringstream msg;
            msg << "decodeExpression: Can't decode instruction " << expr.getInstructionCount() << "\n"
                << "  Reason: " << instr.error();
            return Error<Expression>(std::unexpected(msg.str()));
        }

        expr.addInstruction(std::move(instr.value()));
    }

    return Error<Expression>(std::move(expr));
}

static Error<std::pair<std::uint32_t, ValType>> decodeLocals(CodeSectionDecoder::InputStream &stream)
{
    using RetType = Error<std::pair<std::uint32_t, ValType>>;

    std::uint32_t n = 0;
    {
        auto n_success = CodeSectionDecoder::decodeLEB128<std::uint32_t>(stream);
        if (!n_success)
        {
            std::stringstream msg;
            msg << "decodeLocals: Can't decode number of locals\n"
                << "  Reason: " << n_success.error();
            return RetType(std::unexpected(msg.str()));
        }
        n = n_success.value();
    }

    auto t = CodeSectionDecoder::decodeValType(stream);
    if (!t)
    {
        std::stringstream msg;
        msg << "decodeLocals: Can't decode ValType of locals\n"
            << "  Reason: " << t.error();
        return RetType(std::unexpected(msg.str()));
    }

    return RetType(std::make_pair(n, t.value()));
}

static Error<void> decodeCode(CodeSectionDecoder::InputStream &data_stream,
                              Module &module,
                              std::uint32_t index)
{
    std::uint32_t length = 0;
    {
        auto length_success = CodeSectionDecoder::decodeLEB128<std::uint32_t>(data_stream);
        if (!length_success)
        {
            std::stringstream msg;
            msg << "CodeSection: Can't decode length of code\n"
                << "  Reason: " << length_success.error();
            return Error<void>(std::unexpected(msg.str()));
        }
        length = length_success.value();
    }

    const auto position_at_beginnig = data_stream.tellg();

    auto decode_locals_wrapper = std::function(decodeLocals);
    auto locals_list = CodeSectionDecoder::decodeList(data_stream, decode_locals_wrapper);
    if (!locals_list)
    {
        std::stringstream msg;
        msg << "CodeSection: Can't decode locals\n"
            << "  Reason: " << locals_list.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    /* Check number of locals */
    {
        std::uint64_t sum = 0;
        for (const auto &p : locals_list.value())
            sum += static_cast<std::uint64_t>(p.first);

        if (sum >= (1ul << 32))
        {
            std::stringstream msg;
            msg << "CodeSection: More than 2^32-1 locals in function " << index << "\n";
            return Error<void>(std::unexpected(msg.str()));
        }

        if (options::is_verbose)
            std::cout << "Function " << index << " has " << sum << " locals\n";

        module.functions[index].locals.reserve(sum);
    }

    auto expr = decodeExpression(data_stream);
    if (!expr)
    {
        std::stringstream msg;
        msg << "CodeSection: Can't decode expressions\n"
            << "  Reason: " << expr.error();
        return Error<void>(std::unexpected(msg.str()));
    }

    auto position_at_end = data_stream.tellg();
    if (position_at_end == std::streampos(-1))
    {
        data_stream.clear();
        position_at_end = data_stream.tellg();
    }

    const auto read_length = static_cast<std::int64_t>(position_at_end - position_at_beginnig);
    if (read_length != length)
    {
        std::stringstream msg;
        msg << "CodeSection: Encoded length doesn't match actual read length\n"
            << "  Encoded length = " << length << ", Read length = " << read_length;
        return Error<void>(std::unexpected(msg.str()));
    }

    auto &existing_locals = module.functions[index].locals;
    for (auto &local_pair : locals_list.value())
    {
        existing_locals.insert(existing_locals.end(), local_pair.first, local_pair.second);
    }

    module.functions[index].addExpression(std::move(expr.value()));
    return Error<void>();
}

Error<void> CodeSectionDecoder::decode(Module &module) const
{
    if (options::is_verbose)
        std::cout << "CodeSection: Starting to decode\n";

    auto data_stream = getRawDataAsStream();

    /* decode length of list of functions */
    auto length = decodeLEB128<std::uint32_t>(data_stream);
    if (!length)
    {
        std::stringstream error_message;
        error_message << "CodeSection: Can't read number of functions\n"
                      << "  Reason: " << length.error();
        return Error<void>(std::unexpected(error_message.str()));
    }

    if (length.value() != module.functions.size())
    {
        std::stringstream error_message;
        error_message << "CodeSection: Number of encoded function bodies does not match number of functions in Function section\n"
                      << "  Here: " << length.value() << ", In function section: " << module.functions.size();
        return Error<void>(std::unexpected(error_message.str()));
    }

    for (std::uint32_t i = 0; i < length.value(); ++i)
    {
        if (options::is_verbose)
            std::cout << "CodeSection: Decoding function " << i + 1 << " of " << length.value() << "\n";

        auto element = decodeCode(data_stream, module, i);
        if (!element)
        {
            std::stringstream error_message;
            error_message << "CodeSection: Can't decode function body " << i << "\n"
                          << "  Reason: " << element.error();
            return Error<void>(std::unexpected(error_message.str()));
        }
    }

    return Error<void>();
}

WASM_DECODE_NAMESPACE_END
