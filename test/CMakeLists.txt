FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(loxt_test test.cpp)
set_target_properties(loxt_test PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
target_compile_features(loxt_test PRIVATE cxx_std_20)
target_link_libraries(loxt_test PRIVATE loxt_library GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(loxt_test)