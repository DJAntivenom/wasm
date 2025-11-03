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
        << "  --run-function <name> [n] [args...] \n"
        << "    Run the function called <name>. If n is given the next n values are arguments.\n"
        << "    Each one is of the form i32 v, i64 v, f32 v or f64 v, where v is the number.\n"
        << std::endl;
}

std::optional<wasm::options::FuncArg> parseFuncArg(const std::string &type_str, const std::string &val_str)
{
    try
    {
        if (type_str == "i32")
        {
            return wasm::options::I32{static_cast<int32_t>(std::stoll(val_str))};
        }
        else if (type_str == "i64")
        {
            return wasm::options::I64{std::stoll(val_str)};
        }
        else if (type_str == "f32")
        {
            return wasm::options::F32{std::stof(val_str)};
        }
        else if (type_str == "f64")
        {
            return wasm::options::F64{std::stod(val_str)};
        }
    }
    catch (...)
    {
        return std::nullopt;
    }
    return std::nullopt;
}

void parseArguments(std::vector<std::string> &arguments)
{
    for (auto iter = ++arguments.begin(); iter != arguments.end();)
    {
        const std::string &arg = *iter;

        if (arg == "-h" || arg == "--help")
        {
            printUsage(std::cout, arguments);
            std::exit(0);
        }
        else if (arg == "-v" || arg == "--verbose")
        {
            wasm::options::is_verbose = true;
            ++iter;
        }
        else if (arg == "--run-function")
        {
            ++iter;
            if (iter == arguments.end())
            {
                std::cerr << "Expected function name after --run-function";
                printUsage(std::cerr, arguments);
                std::exit(1);
            }
            wasm::options::RunFunction rf;
            rf.name = *iter++;
            // Check for optional n number of args
            size_t nargs = 0;
            if (iter != arguments.end())
            {
                std::istringstream iss(*iter);
                if ((iss >> nargs) && iss.eof())
                {
                    ++iter; // consume n
                }
                else
                {
                    nargs = 0; // no count given
                }
            }
            // Parse the args
            for (size_t i = 0; i < nargs; ++i)
            {
                if (iter == arguments.end())
                {
                    std::cerr << "Not enough arguments for function " + rf.name + "\n";
                    printUsage(std::cerr, arguments);
                    std::exit(1);
                }
                std::string type, val;
                std::istringstream extractor(*iter);
                if (!(extractor >> type >> val))
                {
                    std::cerr << "Invalid argument format for function " + rf.name + "\n";
                    printUsage(std::cerr, arguments);
                    std::exit(1);
                }
                auto parsed_arg = parseFuncArg(type, val);
                if (!parsed_arg)
                {
                    std::cerr << "Failed to parse function argument '" + *iter + "' for function " + rf.name + "\n";
                    printUsage(std::cerr, arguments);
                    std::exit(1);
                }
                rf.args.push_back(std::move(*parsed_arg));
                ++iter;
            }
            wasm::options::functions_to_run.push_back(std::move(rf));
        }
        else
        {
            // Assume last positional argument is the file name
            if (wasm::options::cli_filename != "")
            {
                std::cerr << "Multiple input files specified\n";
                printUsage(std::cerr, arguments);
                std::exit(1);
            }
            wasm::options::cli_filename = arg;
            ++iter;
        }
    }

    // Check file presence if no parse errors
    if (wasm::options::cli_filename == "")
    {
        std::cerr << "No input file specified\n";
        printUsage(std::cerr, arguments);
        std::exit(1);
    }
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

    if (options::is_verbose)
        std::cout << "main: Decoded module, starting instantiation\n";

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

    for (const auto &function : wasm::options::functions_to_run)
    {
        std::vector<wasm::execute::Value> args;
        std::transform(function.args.begin(),
                       function.args.end(),
                       std::back_inserter(args),
                       [](const auto &arg)
                       {
                           switch (arg.index())
                           {
                           case 0:
                               return wasm::execute::Value(std::get<0>(arg).value);
                           case 1:
                               return wasm::execute::Value(std::get<1>(arg).value);
                           case 2:
                               return wasm::execute::Value(std::get<2>(arg).value);
                           default:
                               return wasm::execute::Value(std::get<3>(arg).value);
                           }
                       });
        auto values = store->runFunction(std::u32string(function.name.begin(),
                                                        function.name.end()),
                                         args);
        if (!values)
        {
            std::cerr << "Running function \"" << function.name << "\" failed\n"
                      << "  Reason: " << values.error() << "\n";
            continue;
        }
    }

    return 0;
}
