cmake_minimum_required(VERSION 3.29)
project(containerofuniquetest VERSION 1.0)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GOOGLETEST_VERSION "1.14.0" CACHE STRING "Version of Google Test")

add_subdirectory(src)
add_subdirectory(thirdparty/googletest)

# Create an executable with a specific C++ standard
function(create_test_executable target_name cpp_standard)
    add_executable(${target_name}_deque tests/test_dequeofunique.cpp)
    add_executable(${target_name}_vector tests/test_vectorofunique.cpp)
    
    target_compile_features(${target_name}_deque PRIVATE cxx_std_${cpp_standard})
    target_compile_features(${target_name}_vector PRIVATE cxx_std_${cpp_standard})

    target_link_libraries(${target_name}_deque PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        containerofunique
    )
    
    target_link_libraries(${target_name}_vector PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        containerofunique
    )

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(${target_name}_deque)
    gtest_discover_tests(${target_name}_vector)
endfunction()

# Build dequeofuniquetest executables for different C++ versions
create_test_executable(test_cxx14 14)  
create_test_executable(test_cxx17 17)  
create_test_executable(test_cxx20 20)  
create_test_executable(test_cxx23 23)
