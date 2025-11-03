# setup executable components
## add include directories
target_include_directories(wasm_library
  PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
)

## add sources
target_sources(wasm
  PUBLIC
  "main.cpp"
)
target_sources(wasm_library
  PUBLIC
  "wasm.cpp"

  "ast/Expression.cpp"
  "ast/Module.cpp"

  "ast/types/CompositeType.cpp"
  "ast/types/RecursiveType.cpp"

  "decode/BinaryDecoder.cpp"
  "decode/CodeSectionDecoder.cpp"
  "decode/ExportSectionDecoder.cpp"
  "decode/FunctionSectionDecoder.cpp"
  "decode/GlobalSectionDecoder.cpp"
  "decode/MemorySectionDecoder.cpp"
  "decode/SectionDecoder.cpp"
  "decode/TypeSectionDecoder.cpp"

  "execute/FunctionInstance.cpp"
  "execute/State.cpp"
  "execute/Store.cpp"
)
