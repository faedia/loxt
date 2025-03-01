set(
    HEADER_LIST
    "${Loxt_SOURCE_DIR}/include/loxt/lexer.hpp"
    "${Loxt_SOURCE_DIR}/include/loxt/token_kinds.def"
)

add_library(
    loxt_library
    lexer.cpp
    ${HEADER_LIST}
)

target_include_directories(loxt_library PUBLIC "${Loxt_SOURCE_DIR}/include/")

target_compile_features(loxt_library PUBLIC cxx_std_20)

source_group(
    TREE "${Loxt_SOURCE_DIR}/include/"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)

set_target_properties(loxt_library PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY}")
target_link_libraries(loxt_library PRIVATE fmt::fmt)
