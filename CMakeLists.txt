# Odeio CMake, então você vai escrever pra mim
# Vou te dar a receita em alto nível, daí você se vira para escrever
# Quero um CMakeLists bem simples que defina um projeto "ufc-mg-homework",
# utilizando C++17 e que use a API de FetchContent para baixar a tag 1.92.9
# da biblioteca Dear ImGui, consagrada para UIs em Immediate Mode
# ( https://github.com/ocornut/imgui ). Após o MakeAvailable, deve existir
# uma pasta para o imgui. Deve incluir imgui/ e imgui/backends aos Include Paths.
# Adicionalmente, deve adicionar ao projeto todos *.cpp e *.h da raíz.
# Deve adicionar também os arquivos fonte e arquivos cabeçalho do backend
# que iremos utilizar, a saber, imgui_impl_glfw.* e imgui_impl_opengl3.*
# Deve incluir também as utils imgui/misc/cpp/imgui_stdlib.*, que pelo visto
# é necessário para interagir com alguns containers da STL, como std::string
# Finalmente, como uso MSVC, parece uma boa também incluir os arquivos de
# depuração auxiliares imgui/misc/debuggers, imgui.natvis e imgui.natstepfilter
# Qualquer dúvida pode consultar a documentação de integração a projetos em
# https://github.com/ocornut/imgui/wiki/Getting-Started, mas acho que já te passei
# os pontos mais relevantes para a receita.
# Note que optei pelo backend em GLFW ( https://github.com/glfw/glfw ), entao também
# precisaremos adicioná-lo ao projeto, inclusive, provavelmente antes da própria ImGUI
# Pode utilizar também a API de FetchContent para isso, com alvo na tag 3.5.1
# É importante nossa receita ser autocontida, reprodutível e multi-plataforma,
# pois o time é diversificado: alguns usam Windows+MSVC, outros Windows+GCC, Linux+GCC etc
# Se for declarar variáveis ou comentários na receita, faça em português, a não ser que
# seja algo herdado do projeto original ou idiomático para o contexto

cmake_minimum_required(VERSION 3.16)

project(ufc-mg-homework LANGUAGES CXX)

# Evita build "in-source", que costuma causar dor de cabeça em times com
# ambientes diferentes (Windows/Linux, MSVC/GCC).
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Não gere o build dentro da própria pasta do código-fonte. Use, por exemplo: cmake -S . -B build")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# --- GLFW -------------------------------------------------------------
# Baixado antes da Dear ImGui, já que o backend da ImGui depende dele.
# Desligamos exemplos/testes/documentação da GLFW: não precisamos disso
# e assim o build fica mais rápido e mais leve.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

# --- Dear ImGui ---------------------------------------------------------
# A ImGui não possui CMakeLists.txt próprio na raiz do repositório (ela é
# distribuída como um conjunto de fontes para serem adicionadas direto ao
# projeto do usuário, conforme o guia oficial de integração). Por isso,
# FetchContent_MakeAvailable só baixa e popula os fontes (em imgui_SOURCE_DIR),
# sem tentar fazer add_subdirectory — quem monta os alvos de compilação
# somos nós, logo abaixo.
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.9
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

# Arquivos da raiz da ImGui (core: imgui.cpp, imgui_draw.cpp, imgui_widgets.cpp,
# imgui_tables.cpp, imgui_demo.cpp e respectivos headers).
file(GLOB IMGUI_CORE_FILES
    "${imgui_SOURCE_DIR}/*.cpp"
    "${imgui_SOURCE_DIR}/*.h"
)

# Backend escolhido: GLFW (janela/input) + OpenGL3 (renderização).
file(GLOB IMGUI_BACKEND_FILES
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.*"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.*"
)

# Utilitário para interagir com containers da STL (ex.: std::string em
# ImGui::InputText).
file(GLOB IMGUI_STDLIB_FILES
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.*"
)

# Arquivos auxiliares de depuração, úteis no Visual Studio/MSVC
# (natvis/natstepfilter não têm efeito em outros compiladores, então
# só os adicionamos quando fizer sentido).
set(IMGUI_DEBUG_FILES "")
if(MSVC)
    file(GLOB IMGUI_DEBUG_FILES "${imgui_SOURCE_DIR}/misc/debuggers/*")
endif()

# --- OpenGL ---------------------------------------------------------------
find_package(OpenGL REQUIRED)

# --- Alvo do projeto --------------------------------------------------------
file(GLOB_RECURSE PROJECT_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME}
    ${IMGUI_CORE_FILES}
    ${IMGUI_BACKEND_FILES}
    ${IMGUI_STDLIB_FILES}
    ${IMGUI_DEBUG_FILES}
    ${PROJECT_SRC_FILES}
)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
)

# --- Suíte de testes --------------------------------------------------
# Inclusão opcional da suíte de testes unitários, controlada pela option
option(BUILD_TEST_SUIT "Compilar a suíte de testes unitários" ON)
if(BUILD_TEST_SUIT)
    enable_testing()
    add_subdirectory(test)
endif()
