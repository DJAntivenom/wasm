/**
 * \file Decoder.hpp
 * \brief Base class for all decoders
 */

#ifndef __WASM_DECODER_HPP__
#define __WASM_DECODER_HPP__

#include "decode.hpp"

#include <functional>
#include <iostream>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <wasm/wasm.hpp>

WASM_DECODE_NAMESPACE_BEGIN

/**
 * @brief A base class for all decoders.
 */
template <class DecodedType>
class Decoder
{
public:
    /** @brief Type of the stream used to decode from. */
    using InputStream = std::istream;

    virtual ~Decoder() {};

    /**
     * @brief Decode the file into `module`.
     *
     * If an error occurs, the optional return value will be set to a string
     * describing the error.
     * @param module Will be overwritten.
     * @return An error message if an error occurs.
     */
    [[nodiscard]]
    virtual Error<DecodedType> decode() const = 0;

    /**
     * @brief Decodes a single byte.
     * @param input_stream The stream will be advanced by one byte.
     * @return An error or the byte.
     */
    static inline Error<Byte> decodeByte(InputStream &input_stream)
    {
        Byte byte;
        input_stream.read(reinterpret_cast<char *>(&byte), 1);

        if (!input_stream)
            return Error<Byte>(std::unexpected("Reached end of stream while reading Byte"));

        return Error<Byte>(byte);
    }

    static Error<MemoryType> decodeLimits(InputStream &input_stream)
    {
        Byte leading_byte;
        {
            auto success = decodeByte(input_stream);
            if (!success)
                return Error<MemoryType>(std::unexpected(success.error()));
            leading_byte = success.value();
        }

        switch (leading_byte)
        {
        case 0x00:
        {
            auto n = decodeLEB128<std::uint64_t>(input_stream);

            if (!n)
            {
                std::stringstream msg;
                msg << "Can't decode lower bound\n"
                    << "  Reason: " << n.error();
                return Error<MemoryType>(std::unexpected(msg.str()));
            }

            MemoryType mem{};
            mem.limits.lower = n.value();
            mem.limits.upper.reset();
            return Error<MemoryType>(mem);
        }

        default:
        {
            std::stringstream msg;
            msg << "Unkown leading byte " << leading_byte;
            return Error<MemoryType>(std::unexpected(msg.str()));
        }
        }
    }

    /**
     * @brief Read a T value encoded as LEB128 from the stream.
     * @tparam T The type of integer that should be read.
     * @param input_stream Will be advanced to after the read integer.
     * @return The read value if no error occurs.
     */
    template <class T, int num_bits = sizeof(T) * 8>
    static inline Error<T> decodeLEB128(InputStream &input_stream)
    {
        static_assert(std::is_integral_v<T>, "LEB128 requires an integral type");

        if constexpr (std::is_signed_v<T>)
        {
            return decodeSLEB128<T, num_bits>(input_stream);
        }
        else
        {
            return decodeULEB128<T, num_bits>(input_stream);
        }
    };

    /**
     * @brief Decode a list of T values.
     * @tparam T The type of the objects of the list.
     * @param input_stream Where data is read from.
     * @param decoding_function Used to decode a single element of the list.
     * @return The list or an error.
     */
    template <class T>
    static Error<std::vector<T>> decodeList(InputStream &input_stream,
                                            const std::function<Error<T>(InputStream &, std::uint32_t)> &decoding_function)
    {
        using RetType = Error<std::vector<T>>;

        if (options::is_verbose)
            std::cout << "Decoding list of values\n";

        /* decode length of list */
        auto length = decodeLEB128<std::uint32_t>(input_stream);
        if (!length)
        {
            std::stringstream error_message;
            error_message << "Can't read length of list\n"
                          << "  Reason: " << length.error();
            return RetType(std::unexpected(error_message.str()));
        }

        /* go through length and encode individual elements using given function */
        std::vector<T> decoded;
        decoded.reserve(length.value());

        for (std::uint32_t i = 0; i < length.value(); ++i)
        {
            if (options::is_verbose)
                std::cout << "Decoding element " << i + 1 << " of " << length.value() << "\n";

            auto element = decoding_function(input_stream, i);
            if (!element)
            {
                std::stringstream error_message;
                error_message << "Can't read element " << i << " of list\n"
                              << "  Reason: " << element.error();
                return RetType(std::unexpected(error_message.str()));
            }

            decoded.push_back(std::move(element.value()));
        }

        return RetType(std::move(decoded));
    }

    /**
     * @brief Decode a list of T values.
     * @tparam T The type of the objects of the list.
     * @param input_stream Where data is read from.
     * @param decoding_function Used to decode a single element of the list.
     * @return The list or an error.
     */
    template <class T>
    static Error<std::vector<T>> decodeList(InputStream &input_stream,
                                            const std::function<Error<T>(InputStream &)> &decoding_function)
    {
        std::function<Error<T>(InputStream &, std::uint32_t)> wrapper = [&decoding_function](InputStream &s, std::uint32_t)
        {
            return decoding_function(s);
        };
        return decodeList(input_stream, wrapper);
    }

    static Error<ValType> decodeValType(InputStream &input_stream)
    {
        Byte leading_byte;
        {
            auto success = decodeByte(input_stream);
            if (!success)
                return Error<ValType>(std::unexpected(success.error()));
            leading_byte = success.value();
        }

        auto get_heaptype = [](InputStream &stream)
        {
            Byte leading_byte;
            {
                auto success = decodeByte(stream);
                if (!success)
                    return Error<HeapType>(std::unexpected(success.error()));
                leading_byte = success.value();
            }

            switch (leading_byte)
            {
            case 0x69:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::Exn()}}}});
            case 0x6A:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::Array()}}}});
            case 0x6B:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::Struct()}}}});
            case 0x6C:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::I31()}}}});
            case 0x6D:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::Eq()}}}});
            case 0x6E:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::Any()}}}});
            case 0x6F:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::Extern()}}}});
            case 0x70:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::Func()}}}});
            case 0x71:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::None()}}}});
            case 0x72:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::NoExtern()}}}});
            case 0x73:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::NoFunc()}}}});
            case 0x74:
                return Error<HeapType>(HeapType{.heaptype{AbsHeapType{.absheaptype{AbsHeapType::NoExn()}}}});
            default:
            {
                stream.seekg(stream.tellg() - std::streampos(1));

                auto x = decodeLEB128<std::int64_t, 33>(stream);
                if (!x)
                {
                    std::stringstream msg;
                    msg << "According to leading byte " << leading_byte
                        << " HeapType must be s33, but decoding failed\n"
                        << "  Reason:" << x.error();
                    return Error<HeapType>(std::unexpected(msg.str()));
                }

                return Error<HeapType>(HeapType{.heaptype{static_cast<TypeUse>(x.value())}});
            }
            }
        };

        switch (leading_byte)
        {
        case 0x63:
        case 0x64:
        {
            auto ht = get_heaptype(input_stream);
            if (!ht)
            {
                std::stringstream msg;
                msg << "Can't decode " << (leading_byte == 0x64 ? "non-" : "") << "nullable heap type\n"
                    << "  Reason: " << ht.error();
                return Error<ValType>(std::unexpected(msg.str()));
            }

            return Error<ValType>(ValType{
                .valtype{
                    RefType{
                        .is_nullable = leading_byte == 0x63,
                        .reftype = ht.value(),
                    }},
            });
        }
        case 0x7B:
            return Error<ValType>(ValType{
                .valtype{
                    VecType{},
                },
            });
        case 0x7C:
            return Error<ValType>(ValType{
                .valtype{
                    NumType{.numtype{NumType::F64()}},
                },
            });
        case 0x7D:
            return Error<ValType>(ValType{
                .valtype{
                    NumType{.numtype{NumType::F32()}},
                },
            });
        case 0x7E:
            return Error<ValType>(ValType{
                .valtype{
                    NumType{.numtype{NumType::I64()}},
                },
            });
        case 0x7F:
            return Error<ValType>(ValType{
                .valtype{
                    NumType{.numtype{NumType::I32()}},
                },
            });

        default:
            input_stream.seekg(input_stream.tellg() - std::streampos(1));
            auto ht = get_heaptype(input_stream);
            if (!ht)
            {
                std::stringstream msg;
                msg << "Can't decode nullable heap type\n"
                    << "  Reason: " << ht.error();
                return Error<ValType>(std::unexpected(msg.str()));
            }

            return Error<ValType>(ValType{
                RefType{
                    .is_nullable = true,
                    .reftype = ht.value(),
                },
            });
        }
    }

    static Error<std::u32string> decodeName(InputStream &stream)
    {
        std::uint32_t num_bytes = 0;
        {
            auto num_bytes_error = decodeLEB128<std::uint32_t>(stream);
            if (!num_bytes_error)
            {
                std::stringstream msg;
                msg << "decodeName: Can't decode number of bytes\n"
                    << "  Reason: " << num_bytes_error.error();
                return Error<std::u32string>(std::unexpected(msg.str()));
            }

            num_bytes = num_bytes_error.value();
        }

        std::vector<Byte> bytes;
        bytes.resize(num_bytes);
        stream.read(reinterpret_cast<char *>(bytes.data()), num_bytes);

        if (!stream)
        {
            return Error<std::u32string>(std::unexpected("decodeName: Reached end of data while decoding name"));
        }

        std::u32string result;
        size_t i = 0;
        while (i < bytes.size())
        {
            uint8_t b1 = bytes[i];
            if (b1 < 0x80)
            {
                /* 1-byte ASCII */
                result.push_back(b1);
                i++;
            }
            else if ((b1 & 0xE0) == 0xC0) /* b1 & 0b1110'0000 == 0b1100'0000 => b1 == 0b110x'xxxx*/
            {
                /* 2-byte sequence */
                if (i + 1 >= num_bytes)
                    return Error<std::u32string>(std::unexpected("decodeName: Final UTF-8 character is missing its second byte"));
                uint8_t b2 = bytes[i + 1];

                if ((b2 & 0xC0) != 0x80) /* Check 2 most significant bits to be 10 */
                {
                    std::stringstream msg;
                    msg << "decodeName: Second byte of UTF-8 string (byte "
                        << i << " in total) is invalid continuation";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }

                char32_t ch = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
                if (ch < 0x80)
                {
                    std::stringstream msg;
                    msg << "decodeName: Character \'" << static_cast<char>(ch)
                        << "\' is represented with 2 bytes";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }
                result.push_back(ch);
                i += 2;
            }
            else if ((b1 & 0xF0) == 0xE0)
            {
                /* 3-byte sequence */
                if (i + 2 >= bytes.size())
                    return Error<std::u32string>(std::unexpected("decodeName: Final UTF-8 character is missing its third (and maybe second) byte"));

                uint8_t b2 = bytes[i + 1];
                uint8_t b3 = bytes[i + 2];
                if ((b2 & 0xC0) != 0x80)
                {
                    std::stringstream msg;
                    msg << "decodeName: Second byte of UTF-8 string (byte "
                        << i << " in total) is invalid continuation";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }
                if ((b3 & 0xC0) != 0x80)
                {
                    std::stringstream msg;
                    msg << "decodeName: Third byte of UTF-8 string (byte "
                        << i << " in total) is invalid continuation";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }

                char32_t ch = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
                if (ch < 0x800)
                {
                    std::stringstream msg;
                    msg << "decodeName: Character starting at byte "
                        << i << "is represented overlong (with 3 bytes)";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }

                if (ch >= 0xD800 && ch <= 0xDFFF)
                    return Error<std::u32string>(std::unexpected("decodeName: Character falls in UTF-16 surrogate range and is invalid"));

                result.push_back(ch);
                i += 3;
            }
            else if ((b1 & 0xF8) == 0xF0)
            {
                // 4-byte sequence
                if (i + 3 >= bytes.size())
                    return Error<std::u32string>(std::unexpected("decodeName: Final UTF-8 character is missing its fourth (and maybe second and third) byte"));

                uint8_t b2 = bytes[i + 1];
                uint8_t b3 = bytes[i + 2];
                uint8_t b4 = bytes[i + 3];
                if ((b2 & 0xC0) != 0x80)
                {
                    std::stringstream msg;
                    msg << "decodeName: Second byte of UTF-8 string (byte "
                        << i << " in total) is invalid continuation";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }
                if ((b3 & 0xC0) != 0x80)
                {
                    std::stringstream msg;
                    msg << "decodeName: Third byte of UTF-8 string (byte "
                        << i << " in total) is invalid continuation";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }
                if ((b4 & 0xC0) != 0x80)
                {
                    std::stringstream msg;
                    msg << "decodeName: Fourth byte of UTF-8 string (byte "
                        << i << " in total) is invalid continuation";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }
                char32_t ch = ((b1 & 0x07) << 18) | ((b2 & 0x3F) << 12) | ((b3 & 0x3F) << 6) | (b4 & 0x3F);
                if (ch < 0x10000)
                {
                    std::stringstream msg;
                    msg << "decodeName: Character starting at byte "
                        << i << " is encoded overlong";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }

                if (ch > 0x10FFFF)
                {
                    std::stringstream msg;
                    msg << "decodeName: Character starting at byte "
                        << i << " is outside the UTF-8 range";
                    return Error<std::u32string>(std::unexpected(msg.str()));
                }

                result.push_back(ch);
                i += 4;
            }
            else
            {
                std::stringstream msg;
                msg << "decodeName: Character starting at byte "
                    << i << " has an invalid leading byte";
                return Error<std::u32string>(std::unexpected(msg.str()));
            }
        }

        return Error<std::u32string>(result);
    }

private:
    template <class T, int num_bits>
    static Error<T> decodeULEB128(InputStream &input_stream)
    {
        /* ceiling of division as stated in spec */
        constexpr std::size_t max_bytes = (num_bits + 6) / 7;

        T result = 0;
        for (std::size_t byte_index = 0, shift = 0;
             byte_index < max_bytes;
             ++byte_index, shift += 7)
        {
            /* read one byte */
            auto byte_error = decodeByte(input_stream);

            /* return if EOF is reached*/
            if (!byte_error)
                return Error<T>(std::unexpected(byte_error.error()));

            Byte byte = byte_error.value();

            /* decode byte */
            result |= (T(byte & 0x7F) << shift);

            /* if end is reached return result*/
            if ((byte & 0x80) == 0)
                return Error<T>(result);
        }

        /* encoding is too long */
        std::stringstream error_message;
        error_message << "Encoding of integer is longer than " << max_bytes << " bytes";
        return Error<T>(std::unexpected(error_message.str()));
    };

    template <class T, int num_bits>
    static Error<T> decodeSLEB128(InputStream &input_stream)
    {
        /* ceiling of division as stated in spec */
        constexpr std::size_t max_bytes = (num_bits + 6) / 7;

        T result = 0;
        for (std::size_t byte_index = 0, shift = 0;
             byte_index < max_bytes;
             ++byte_index, shift += 7)
        {
            /* read one byte */
            auto byte_error = decodeByte(input_stream);

            /* return if EOF is reached*/
            if (!byte_error)
                return Error<T>(std::unexpected(byte_error.error()));

            Byte byte = byte_error.value();

            /* decode byte */
            result |= (T(byte & 0x7F) << shift);

            /* if end is reached return result*/
            if ((byte & 0x80) == 0)
            {
                /* sign-extend */
                if (shift < num_bits && (byte & 0x40))
                    result |= (~T(0) << shift);

                return Error<T>(result);
            }
        }

        /* encoding is too long */
        std::stringstream error_message;
        error_message << "Encoding of integer is longer than " << max_bytes << " bytes";
        return Error<T>(std::unexpected(error_message.str()));
    };
};

WASM_DECODE_NAMESPACE_END

/// include subclasses
#include "BinaryDecoder.hpp"
#include "SectionDecoder.hpp"

#endif
