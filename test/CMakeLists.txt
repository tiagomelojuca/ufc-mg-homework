include(FetchContent)

# No MSVC, usa a mesma runtime dinâmica do restante do projeto.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_tests
    ${TEST_SRC_FILES}
)

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_NAME}_application
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests)
