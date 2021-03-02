cmake_minimum_required(VERSION 3.1)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/third_party/entityx/cmake")
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/third_party/bullet3")

# Disabled cotire due to issues with GL includes, renable later if needed
# set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/third_party/cotire/CMake")
# include(cotire)

project(LeagueOfDwarves)

set(LeagueOfDwarves_VERSION_MAJOR 0)
set(LeagueOfDwarves_VERSION_MINOR 1)
set(LeagueOfDwarves_VERSION_PATCH 0)
set(LeagueOfDwarves_VERSION "${LeagueOfDwarves_VERSION_MAJOR}.${LeagueOfDwarves_VERSION_MINOR}.${LeagueOfDwarves_VERSION_PATCH}")

set(PROJ_NAME "\"League Of Dwarves\"") # PROJECT_NAME is a predefined variable
set(PROJECT_DESCRIPTION "\"A cross-platform C++14 project template\"")

configure_file(src/league-of-dwarves/config.hpp.in
    ${CMAKE_BINARY_DIR}/src/league-of-dwarves/config.hpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build test executable" OFF)
option(GEN_DOCS "Generate documentation" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as no build type was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type (Debug/Release)" FORCE)
endif (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

if (ENABLE_COVERAGE AND NOT CMAKE_CONFIGURATION_TYPES)
    if (NOT BUILD_TESTS)
        message(FATAL_ERROR "Tests must be enabled for code coverage!")
    endif (NOT BUILD_TESTS)
    
    include(CodeCoverage)
    
    append_coverage_compiler_flags()
    set(COVERAGE_EXCLUDES "/usr/include/\\*;${CMAKE_SOURCE_DIR}/src/league-of-dwarves/main.cpp;${CMAKE_SOURCE_DIR}/test/*;${CMAKE_SOURCE_DIR}/third_party/doctest/*")
    
    setup_target_for_coverage(NAME coverage EXECUTABLE ctest DEPENDENCIES coverage)
else (ENABLE_COVERAGE AND NOT CMAKE_CONFIGURATION_TYPES)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
endif (ENABLE_COVERAGE AND NOT CMAKE_CONFIGURATION_TYPES)

file(GLOB_RECURSE ENGINE_SOURCE_FILES
     "src/boundless-engine/*.cpp"
)

file(GLOB_RECURSE LOD_SOURCE_FILES
     "src/league-of-dwarves/*.cpp"
)


set(LIBRARY_SOURCES
    ${ENGINE_SOURCE_FILES}
)

set(GAME_SOURCES
    ${LOD_SOURCE_FILES}
)

# Add boost, glew, glfw, glm
FIND_PACKAGE( Boost 1.75 COMPONENTS system filesystem REQUIRED )
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()

FIND_PACKAGE( GLEW 2.2 REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
endif()

FIND_PACKAGE( GLFW3 3.3 REQUIRED)
if (GLFW3_FOUND)
    include_directories(${GLFW3_INCLUDE_DIRS})
endif()

FIND_PACKAGE( GLM REQUIRED)
if (GLM_FOUND)
    include_directories(${GLM_INCLUDE_DIRS})
endif()

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
include_directories( ${OPENCL_INCLUDE_DIR})    

find_package(GameNetworkingSockets REQUIRED)
if (GLM_FOUND)
    include_directories(${GameNetworkingSockets_INCLUDE_DIRS})
endif()

find_package(OpenAL REQUIRED)
if(APPLE)
    set(ENV{OPENALDIR} "/usr/local/opt/openal-soft")
    include_directories( "/usr/local/opt/openal-soft/include")
else()
    include_directories( ${OpenAL_INCLUDE_DIR})
endif()

find_package(spdlog REQUIRED)

FIND_PACKAGE( entityx REQUIRED)
if (entityx_FOUND)
    include_directories(${ENTITYX_INCLUDE_DIRS})
endif()

FIND_PACKAGE( Bullet REQUIRED)
if (Bullet_FOUND)
    include_directories(${Bullet_INCLUDE_DIRS})
endif()


# Add third party libaries
include_directories("third_party/eventpp/include")
include_directories("third_party/SimplexNoise/src")

set(SIMPLEX_NOISE
    "third_party/SimplexNoise/src/SimplexNoise.cpp"
)

add_library(BoundlessEngine ${LIBRARY_SOURCES} ${SIMPLEX_NOISE}) # A library
set_target_properties(BoundlessEngine PROPERTIES PREFIX "") # Remove the lib prefix
target_include_directories(BoundlessEngine PUBLIC ${CMAKE_SOURCE_DIR}/src/boundless-engine ${CMAKE_BINARY_DIR}/src/boundless-engine)

if(APPLE)
    target_link_libraries(BoundlessEngine LINK_PUBLIC ${GLM_LIBRARIES}
         ${GLFW3_LIBRARIES} ${GLEW_LIBRARIES} ${Boost_LIBRARIES} ${OpenAL_LIBRARIES} GLEW::GLEW glfw OpenCL::OpenCL 
         "-framework OpenCL" spdlog::spdlog ${GameNetworkingSockets_LIBRARIES} ${ENTITYX_LIBRARIES} ${Bullet_LIBRARIES}) 
else()
    target_link_libraries(BoundlessEngine LINK_PUBLIC ${GLM_LIBRARIES}
         ${GLFW3_LIBRARIES} ${GLEW_LIBRARIES} ${Boost_LIBRARIES} ${OpenAL_LIBRARIES} GLEW::GLEW glfw OpenCL::OpenCL 
         spdlog::spdlog ${GameNetworkingSockets_LIBRARIES} ${ENTITYX_LIBRARIES} ${Bullet_LIBRARIES}) 
endif()
# cotire(BoundlessEngine)

add_executable(LeagueOfDwarves ${GAME_SOURCES}) # The main game executable
target_link_libraries(LeagueOfDwarves LINK_PUBLIC BoundlessEngine) 
# cotire(LeagueOfDwarves)

if (BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif (BUILD_TESTS)

if (GEN_DOCS)
    add_subdirectory(doc)
endif (GEN_DOCS)

# Install the built library and executable into the appropriate directory
install(TARGETS LeagueOfDwarves DESTINATION bin)
install(TARGETS BoundlessEngine
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install the project header files into the appropriate directory
install(DIRECTORY src/ DESTINATION src PATTERN "config.hpp.in" EXCLUDE) # Install our main header files
install(FILES ${CMAKE_BINARY_DIR}/src/league-of-dwarves/config.hpp DESTINATION src/league-of-dwarves) # Install our configuration file

# Install the project's extra files
install(FILES README.md LICENSE DESTINATION share/league-of-dwarves)
