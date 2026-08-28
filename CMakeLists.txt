cmake_minimum_required(VERSION 3.20)
project(graphicsbox LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Les dependances (glm, assimp/zlib...) declarent encore un
# cmake_minimum_required < 3.5, refuse par CMake >= 4.0.
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
endif()

# Tout est lie en statique : pas de DLL a copier a cote de l'exe.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Les binaires atterrissent tous au meme endroit (multi-config VS inclus).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(_cfg IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg} "${CMAKE_BINARY_DIR}/bin")
endforeach()

include(FetchContent)

# ---------- GLFW (fetched automatically) ----------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# ---------- GLM (fetched automatically) ----------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
set(GLM_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)

# ---------- Assimp (Open Asset Import Library) ----------
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
    GIT_SHALLOW    TRUE
)
set(ASSIMP_BUILD_TESTS         OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS  OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES       OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL             OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS  OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT           ON  CACHE BOOL "" FORCE)   # import seulement
set(ASSIMP_BUILD_ZLIB          ON  CACHE BOOL "" FORCE)   # zlib embarque (Windows)
# Importers : on garde OBJ / FBX / glTF / Collada / PLY / STL, le reste est coupe
# pour reduire fortement le temps de compilation.
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
foreach(_fmt IN ITEMS OBJ FBX GLTF COLLADA PLY STL BLEND)
    set(ASSIMP_BUILD_${_fmt}_IMPORTER ON CACHE BOOL "" FORCE)
endforeach()

FetchContent_MakeAvailable(glfw glm assimp)

# Assimp compile avec beaucoup de warnings : on les tait pour ne pas noyer les notres.
if(MSVC)
    target_compile_options(assimp PRIVATE /W0)
else()
    target_compile_options(assimp PRIVATE -w)
endif()

# ---------- OpenGL ----------
find_package(OpenGL REQUIRED)

# ---------- GLAD (local, checked-in) ----------
add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
target_link_libraries(glad PUBLIC OpenGL::GL)

# ---------- stb_image (local, header-only + 1 TU d'implementation) ----------
add_library(stb_image STATIC
    external/stb/stb_image_impl.c
)
target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")
if(MSVC)
    target_compile_definitions(stb_image PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(stb_image PRIVATE /W0)
endif()

# ---------- Sources (src/ scanne recursivement) ----------
# GLOB_RECURSE + CONFIGURE_DEPENDS : cree un dossier et des sous-dossiers dans
# src/, depose tes .cpp/.h dedans, et un simple build les prend en compte.
# Aucune ligne a ajouter ici.
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

if(NOT SRC_FILES)
    message(FATAL_ERROR "Aucun fichier source trouve dans ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

list(LENGTH SRC_FILES _src_count)
message(STATUS "Source files found: ${_src_count}")

# ---------- Shaders (visible dans la solution VS) ----------
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.shader"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.comp"
)

message(STATUS "Shader files found: ${SHADER_FILES}")

# ---------- Main executable ----------
add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${SHADER_FILES}
)

# Permet d'ecrire #include "render/Mesh.h" depuis n'importe quel fichier,
# au lieu de chemins relatifs du type "../../render/Mesh.h".
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Empeche VS d'essayer de "compiler" les shaders comme du C++
set_source_files_properties(${SHADER_FILES} PROPERTIES
    VS_TOOL_OVERRIDE "None"
    HEADER_FILE_ONLY TRUE
)

# Reproduit l'arborescence reelle de src/ dans le Solution Explorer
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
             PREFIX "Source"
             FILES ${SRC_FILES})

# Idem pour Shaders/ (sous-dossiers compris)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
             PREFIX "Shaders"
             FILES ${SHADER_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
    glm::glm
    stb_image
    assimp::assimp
    OpenGL::GL
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    # F5 lance directement le bon projet, depuis le dossier de l'exe
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                 PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif()

# Copie le dossier Shaders a cote de l'exe apres chaque build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
    COMMENT "Copie des shaders vers le dossier de sortie"
)

# Copie le dossier assets (modeles / textures) s'il existe
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copie des assets vers le dossier de sortie"
    )
endif()
