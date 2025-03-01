include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v2.6
)
FetchContent_MakeAvailable(argparse)

add_executable(loxt loxt.cpp)

set_target_properties(loxt PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
target_compile_features(loxt PRIVATE cxx_std_20)
target_link_libraries(loxt loxt_library argparse fmt::fmt)
