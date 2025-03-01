FetchContent_Declare(
    catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(catch)

add_executable(loxt_test test.cpp)
target_compile_features(loxt_test PRIVATE cxx_std_20)
target_link_libraries(loxt_test PRIVATE loxt_library Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(loxt_test)