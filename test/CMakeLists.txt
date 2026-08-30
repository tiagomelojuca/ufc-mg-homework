# Crie a receita deste CMakeLists para a suíte de testes unitários da aplicação
# Deve utilizar gtest ( https://github.com/google/googletest ), tag v1.18.0,
# obtido a partir da API de FetchContent. O build deste módulo não é obrigatório,
# deve ser incluído no CMakeLists raiz por meio de add_subdirectory, mas apenas
# mediante a uma option ligada, a saber, BUILD_TEST_SUIT. Caso esteja off,
# não inclui o presente CMakeLists

include(FetchContent)

# No MSVC, força o gtest a usar a runtime library dinâmica (/MD, /MDd),
# a mesma usada pelo restante do projeto, evitando conflitos de linkagem.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(googletest)

# --- Alvo da suíte de testes --------------------------------------------
file(GLOB TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}_tests
    ${TEST_SRC_FILES}
)

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests)
