# setup executable components
## add include directories
target_include_directories(${WASM_TARGET_NAME}
  PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
)

## add sources
target_sources(${WASM_TARGET_NAME}
  PRIVATE
  "main.cpp"
  "wasm.cpp"

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
)
