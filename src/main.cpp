#include <iostream>
#include <vector>

static void printUsage(std::vector<std::string> const &arguments)
{
    std::cout << "Usage: " << arguments[0] << " <binary>\n"
              << "\n"
              << "  Loads the file given in <binary>, validates and interprets it."
              << std::endl;
}

int main(int argc, const char *argv[])
{
    std::vector<std::string> arguments(argv, argv + argc);

    printUsage(arguments);

    return 0;
}
