# wasm
Small WebAssembly interpreter written as part of the application process for an NVIDIA internship.

# Building

The project can be built using cmake. The easiest way is using presets:
```sh
$ cmake --preset debug
$ cmake --build ./build_debug
$ ./build_debug/wasm
or on windows
$ .\build_debug\Debug\wasm.exe
```

By specifying the `-h` flag, usage information is printed. I tested all functionality on Debian using gcc-14. On windows I made sure the project builds with MSVC 19.44.35219.0 more about that in the Limitations section.

# Structure

The project consists of 3 major components.
- `include/wasm/ast` contains classes that represent the abstract syntax tree of a WebAssembly module. This includes all the types, instructions, memories etc. The most important part is the `Module` class which is an abstract representation of one WebAssembly module.
- `include/wasm/decode` contains decoders for the different section of a binary. I use a shallow type-hierarchy, where each subclass of the `SectionDecoder` baseclass is responsible for decoding one section. The `SectionDecoder::createDecoder()` method acts as a factory. These decoders are used as members in the `BinaryDecoder` class, which is responsible for decoding a whole file. Together they decode a `Module` object from the `ast` section.
- `include/wasm/execute` contains instantiations of the abstract representation which are then used to execute the code. They contain all the runtime data of the wasm program.

The `src` folder is follows the same structure.

Additionally, there is a `test` folder which contains a test file, which decodes the `test/wasm/01_test.wasm` file, runs each function it exports and checks the value returned in `address[0]`. I wanted to add more tests, namely some that check if decoding is handled correctly, even if there are errors in the binary and some that check validation, of which I have only little.

## Operation

First a decoder can be created using `BinaryDecoder::createBinaryDecoder(std::string)`. It can be used to decode a `Module`. This is the abstract representation of the module. It has a member-function `Module::instantiate()` which returns a `Store`. As described in the specification, a store holds all the runtime state and a reference to the `ModuleInstance` which is just the runtime representation of a Module. This store can then invoke functions using the corresponding `runFunction` method. It returns a `State`, i.e. locals, operand stack, etc..

Almost all functions return a `Error<...>` object, because I don't like the std-library exceptions, because they can be disabled on some host-machines, which I had to learn the hard way in the past ^^

# Limitations

Because this is the first time I worked with WebAssembly, I was a bit lost in the beginning. So what I did is I followed the specification. This turned out not to be such a good idea. I spent way too much time on the decoding and validation part. My implementation can decode way more things than it can execute or validate. If I could do it again, I would immediately focus more on the execution part.

I then spent a _lot_ of time trying to get the whole type checking/ validation to work. After approx 11h I finally gave up on the subtype and up-rolling, etc. I therefore only have validation for the types defined in a Module.

Furthermore, I had hardly any time left for the execution-implementation. (Eventhough I did work all night, see the commit timestamps in the squash). That's why my limitation can only execute the first function of the first test case (01_test.wasm), which is really disappointing. However, I do think that my structure of the project is not too bad and with a couple more day, I think this could work really well and be easily extended later to e.g. a native compiler (because of the abstract representatin I have).

And finally, I have just now realized that when compiling with MSVC the decoding breaks, and I think it's because of some unsigned/signed bounds checking error with the std::streampos. I use those sometimes to check how many bytes my decoders read. Obviously gcc does something different than MSVC ^^

Anyways, thank you for this opportunity :)
