cmake_minimum_required(VERSION 3.16)

project(ufc-mg-csg LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Não gere o build dentro da pasta do código-fonte. Use, por exemplo: cmake -S . -B build")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# --- GLFW -------------------------------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# No Linux, X11 é o padrão; Wayland pode ser ativado pela opção abaixo.
if(UNIX AND NOT APPLE)
    option(UFC_MG_USE_WAYLAND "Compilar a GLFW com suporte a Wayland" OFF)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND ${UFC_MG_USE_WAYLAND} CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

# --- Dear ImGui ---------------------------------------------------------
# A ImGui não tem CMakeLists próprio; os alvos são montados com seus fontes.
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.9
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

file(GLOB IMGUI_CORE_FILES
    "${imgui_SOURCE_DIR}/*.cpp"
    "${imgui_SOURCE_DIR}/*.h"
)

file(GLOB IMGUI_BACKEND_FILES
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.*"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.*"
)

file(GLOB IMGUI_STDLIB_FILES
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.*"
)

set(IMGUI_DEBUG_FILES "")
if(MSVC)
    file(GLOB IMGUI_DEBUG_FILES "${imgui_SOURCE_DIR}/misc/debuggers/*")
endif()

# --- OpenGL ---------------------------------------------------------------
find_package(OpenGL REQUIRED)

# --- Camadas do projeto ---------------------------------------------------
# Uma camada ainda sem fontes vira biblioteca INTERFACE, para que os alvos
# possam declarar dependências desde o início.
function(adiciona_camada nome diretorio)
    file(GLOB_RECURSE fontes ${CMAKE_CURRENT_SOURCE_DIR}/src/${diretorio}/*.cpp)

    if(fontes)
        add_library(${nome} ${fontes})
        target_include_directories(${nome} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_link_libraries(${nome} PUBLIC ${ARGN})
    else()
        add_library(${nome} INTERFACE)
        target_include_directories(${nome} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_link_libraries(${nome} INTERFACE ${ARGN})
    endif()
endfunction()

adiciona_camada(${PROJECT_NAME}_core Core)
adiciona_camada(${PROJECT_NAME}_persistence Persistence ${PROJECT_NAME}_core)
adiciona_camada(${PROJECT_NAME}_application Application ${PROJECT_NAME}_core ${PROJECT_NAME}_persistence)

# --- Executável -----------------------------------------------------------
file(GLOB_RECURSE UI_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/*.cpp)

add_executable(${PROJECT_NAME}
    ${IMGUI_CORE_FILES}
    ${IMGUI_BACKEND_FILES}
    ${IMGUI_STDLIB_FILES}
    ${IMGUI_DEBUG_FILES}
    ${UI_SRC_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}_application
    glfw
    OpenGL::GL
)

# --- Testes ---------------------------------------------------------------
option(BUILD_TEST_SUIT "Compilar a suíte de testes unitários" ON)
if(BUILD_TEST_SUIT)
    enable_testing()
    add_subdirectory(test)
endif()
