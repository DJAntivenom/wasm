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
)
