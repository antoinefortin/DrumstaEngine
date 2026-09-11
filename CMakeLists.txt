cmake_minimum_required(VERSION 3.20)

project(graphicsbox LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
endif()

# Static libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


# =========================================================
# OUTPUT
# =========================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)

foreach(_cfg IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg}
        "${CMAKE_BINARY_DIR}/bin"
    )
endforeach()


# =========================================================
# DEPENDENCIES
# =========================================================

include(FetchContent)


# ---------------- GLFW ----------------

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)


# ---------------- GLM ----------------

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)


# ---------------- ASSIMP ----------------

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
    GIT_SHALLOW TRUE
)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)

set(
    ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT
    OFF
    CACHE BOOL "" FORCE
)

foreach(_fmt IN ITEMS OBJ FBX GLTF COLLADA PLY STL BLEND)
    set(
        ASSIMP_BUILD_${_fmt}_IMPORTER
        ON
        CACHE BOOL "" FORCE
    )
endforeach()


FetchContent_MakeAvailable(
    glfw
    glm
    assimp
)

if(MSVC)
    target_compile_options(assimp PRIVATE /W0)
else()
    target_compile_options(assimp PRIVATE -w)
endif()


# =========================================================
# OPENGL
# =========================================================

find_package(OpenGL REQUIRED)


# =========================================================
# GLAD
# =========================================================

add_library(
    glad STATIC
    external/glad/src/glad.c
)

target_include_directories(
    glad PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include"
)

target_link_libraries(
    glad PUBLIC
    OpenGL::GL
)


# =========================================================
# STB IMAGE
# =========================================================

add_library(
    stb_image STATIC
    external/stb/stb_image_impl.c
)

target_include_directories(
    stb_image PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/external/stb"
)


# =========================================================
# STB IMAGE WRITE
# =========================================================

add_library(
    stb_image_write STATIC
    external/stb/stb_image_write_impl.c
)

target_include_directories(
    stb_image_write PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/external/stb"
)


if(MSVC)

    target_compile_definitions(
        stb_image PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )

    target_compile_definitions(
        stb_image_write PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )

    target_compile_options(stb_image PRIVATE /W0)
    target_compile_options(stb_image_write PRIVATE /W0)

endif()


# =========================================================
# SOURCE FILES
# =========================================================

file(
    GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS

    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

if(NOT SRC_FILES)
    message(
        FATAL_ERROR
        "No source files found inside src/"
    )
endif()


# =========================================================
# ASSETS
# =========================================================

file(
    GLOB_RECURSE ASSET_FILES
    CONFIGURE_DEPENDS

    "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*"
)


# =========================================================
# EXECUTABLE
# =========================================================

add_executable(
    ${PROJECT_NAME}

    ${SRC_FILES}
    ${ASSET_FILES}
)


# Allows:
#
# #include "Importer/AssetImporter.h"
# #include "Engine/Rendering/OpenGL/Texture.h"

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE

    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)


# =========================================================
# VISUAL STUDIO FOLDERS
# =========================================================

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "Source"
    FILES ${SRC_FILES}
)


if(ASSET_FILES)

    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        PREFIX "Assets"
        FILES ${ASSET_FILES}
    )

    # Models, textures and shaders are NOT C++ source files
    set_source_files_properties(
        ${ASSET_FILES}
        PROPERTIES
        HEADER_FILE_ONLY TRUE
    )

endif()


# =========================================================
# LINK LIBRARIES
# =========================================================

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE

    glfw
    glad
    glm::glm

    stb_image
    stb_image_write

    assimp::assimp
    OpenGL::GL
)


# =========================================================
# VISUAL STUDIO SETTINGS
# =========================================================

if(MSVC)

    target_compile_definitions(
        ${PROJECT_NAME}
        PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )

    set_property(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        PROPERTY
        VS_STARTUP_PROJECT ${PROJECT_NAME}
    )

    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES

        VS_DEBUGGER_WORKING_DIRECTORY
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )

endif()


# =========================================================
# COPY ASSETS BESIDE EXECUTABLE
# =========================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Assets")

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD

        COMMAND
        ${CMAKE_COMMAND}
        -E copy_directory

        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"

        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"

        COMMENT "Copying Assets folder..."
    )

endif()
