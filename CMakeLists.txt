cmake_minimum_required(VERSION 3.21)
project(SFML-Engine LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(SFML-Engine src/main.cpp src/utils/Render.h)
target_link_libraries(SFML-Engine PRIVATE sfml-graphics)
target_compile_features(SFML-Engine PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET SFML-Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:SFML-Engine> $<TARGET_FILE_DIR:SFML-Engine> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS SFML-Engine)
