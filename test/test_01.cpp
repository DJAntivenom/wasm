#include <iostream>
#include <vector>
#include <string>

#include <wasm/wasm.hpp>

template <typename Integer,
          typename = std::enable_if_t<std::is_integral_v<Integer> && !std::is_same_v<Integer, bool>>>
static Integer fromLittleEndianBytes(const std::vector<uint8_t> &bytes)
{
    if (bytes.size() != sizeof(Integer))
    {
        throw std::invalid_argument("Byte vector size does not match integer size");
    }
    Integer value = 0;
    for (size_t i = 0; i < sizeof(Integer); ++i)
    {
        value |= static_cast<Integer>(bytes[i]) << (8 * i);
    }
    return value;
}

int main()
{
    using namespace wasm;

    std::vector<std::string> data = {
        "_start", "_test_store", "_test_addition",
        "_test_subtraction", "_test_multiplication",
        "_test_division_signed", "_test_division_unsigned",
        "_test_remainder", "_test_and", "_test_or", "_test_xor",
        "_test_shift_left", "_test_shift_right_signed",
        "_test_shift_right_unsigned", "_test_store_load",
        "_test_store_load_byte_unsigned", "_test_store_load_byte_signed",
        "_test_locals_arithmetic", "_test_locals_tee",
        "_test_global_increment", "_test_global_constant",
        "_test_global_multiple", "_test_combined", "_test_eq",
        "_test_ne", "_test_lt_s", "_test_lt_u", "_test_gt_s",
        "_test_gt_u", "_test_le_s", "_test_ge_s", "_test_eqz_zero",
        "_test_eqz_nonzero", "_test_clz", "_test_ctz", "_test_popcnt",
        "_test_popcnt_all", "_test_rotl", "_test_rotr", "_test_rotl_wrap",
        "_test_load16_u", "_test_load16_s", "_test_load16_32768",
        "_test_select_true", "_test_select_false", "_test_if_true",
        "_test_if_false", "_test_if_no_else", "_test_nested_if",
        "_test_block_break", "_test_block_no_break", "_test_loop_sum",
        "_test_loop_early_break", "_test_br_table_case0", "_test_br_table_case2"};

    std::vector<std::int32_t> results = {
        42, 15, 12, 42, 5, 6, 2, 10, 14, 6, 20,
        -4, 4, 99, 255, -1, 35, 15, 1, 100, 10,
        142, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 28,
        3, 3, 32, 16, 1, 1, 65535, -1, 32768, 10,
        20, 100, 200, 50, 1, 10, 20, 15, 15, 100, 102};

    std::unique_ptr<decode::BinaryDecoder> decoder;
    {
        auto error_decoder = decode::BinaryDecoder::createBinaryDecoder("wasm/01_test.wasm");

        if (!error_decoder)
        {
            std::cerr << "Can't create decoder\n"
                      << "  Reason: " << error_decoder.error()
                      << std::endl;
            return 1;
        }

        decoder = std::move(error_decoder.value());
    }

    Module module;
    {
        auto decode_error = decoder->decode();
        if (!decode_error)
        {
            std::cerr << "Can't decode module\n"
                      << "  Reason: " << decode_error.error()
                      << std::endl;
            return 1;
        }

        module = std::move(decode_error.value());
    }

    std::shared_ptr<execute::Store> store;
    {
        auto instantiation_success = module.instantiate();
        if (!instantiation_success)
        {
            std::cerr << "Instantiation of module failed\n"
                      << "  Reason: " << instantiation_success.error() << "\n";
            return 1;
        }
        store = std::move(instantiation_success.value());
    }

    for (std::size_t i = 0; i < data.size(); ++i)
    {
        const auto &function = data[i];
        std::vector<execute::Value> args;
        auto state = store->runFunction(std::u32string(function.begin(),
                                                       function.end()),
                                        args);
        if (!state)
        {
            std::cerr << "Running function \"" << function << "\" failed\n"
                      << "  Reason: " << state.error() << "\n";
            return 1;
        }

        std::vector<std::uint8_t> bytes = state.value().readBytesFromMemory(0, 0, 4);
        if (fromLittleEndianBytes<std::int32_t>(bytes) != results[i])
        {
            std::cerr << "result of function " << function << " incorrect\n";
        }
    }

    return 0; // success
}