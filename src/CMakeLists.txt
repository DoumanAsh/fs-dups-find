add_executable(${PROJECT_NAME} main.cpp)

#target_include_directories(${PROJECT_NAME} PUBLIC "${Boost_INCLUDE_DIRS}")
target_link_libraries(${PROJECT_NAME} CONAN_PKG::fmt CONAN_PKG::cryptopp)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} INTERFACE _WIN32_WINNT=0x0600)
endif()
