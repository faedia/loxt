set(HEADER_LIST "${Loxt_SOURCE_DIR}/include/loxt/lib.hpp")

add_library(loxt_library lib.cpp)

target_include_directories(loxt_library PUBLIC "${Loxt_SOURCE_DIR}/include/")

target_compile_features(loxt_library PUBLIC cxx_std_20)

source_group(
    TREE "${Loxt_SOURCE_DIR}/include/"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
set_target_properties(loxt_library PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")