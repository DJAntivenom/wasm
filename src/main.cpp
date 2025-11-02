#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

#include <wasm/wasm.hpp>

std::string trim(const std::string &s)
{
    auto start = std::find_if(s.begin(), s.end(), [](unsigned char ch)
                              { return !std::isspace(ch); });
    auto end = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                            { return !std::isspace(ch); })
                   .base();
    if (start >= end)
        return "";
    return std::string(start, end);
}

static void printUsage(std::ostream &out, std::vector<std::string> const &arguments)
{
    out << "Usage: " << arguments[0] << " [options] <file>\n"
        << "\n"
        << "  Loads the file given in <file>, validates and interprets it."
        << "\n"
        << "Options:\n"
        << "  -h, --help\n"
        << "    Print this help message and exit\n."
        << "  -v, --verbose\n"
        << "    Be more verbose.\n"
        << std::endl;
}

static void parseArguments(std::vector<std::string> &arguments)
{
    auto has_help = [](const std::string &s)
    {
        auto trimmed = trim(s);
        return trimmed == "--help" || trimmed == "-h";
    };

    if (std::any_of(arguments.cbegin(), arguments.cend(), has_help))
    {
        printUsage(std::cout, arguments);
        std::exit(0);
    }

    auto matches_verbose = [](const std::string &s)
    {
        auto trimmed = trim(s);
        return trimmed == "--verbose" || trimmed == "-v";
    };

    auto new_end = std::remove_if(arguments.begin(), arguments.end(), matches_verbose);
    if (new_end != arguments.end())
    {
        std::cout << "Output is verbose\n";
        wasm::options::is_verbose = true;
        arguments.erase(new_end, arguments.end());
    }

    if (arguments.size() > 2)
    {
        std::cerr << "Only one binary file allowed, got " << arguments.size() - 1 << "\n\t";
        for (std::size_t i = 1; i < arguments.size(); ++i)
        {
            std::cerr << arguments[i];
            if (i != arguments.size() - 1)
                std::cerr << ", ";
        }
        std::cerr << "\n\n";
        std::exit(1);
    }

    if (arguments.size() < 2)
    {
        std::cerr << "No file given\n\n";
        printUsage(std::cerr, arguments);
        std::exit(1);
    }
    wasm::options::cli_filename = arguments[1];
}

int main(int argc, const char *argv[])
{
    using namespace wasm;

    std::vector<std::string> arguments(argv, argv + argc);
    parseArguments(arguments);

    std::unique_ptr<decode::BinaryDecoder> decoder;
    {
        auto error_decoder = decode::BinaryDecoder::createBinaryDecoder(options::cli_filename);

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

    return 0;
}
