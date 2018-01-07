cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
include(cmake/HunterGate.cmake)

HunterGate(
    URL "https://github.com/lightman1998/hunter/archive/v0.19.137.1.tar.gz"
    SHA1 "a7d84022ffa743c3a3647ce4841a42f00f11a8ad"
)

project(builder)

hunter_add_package(Boost COMPONENTS filesystem system program_options)
hunter_add_package(BoostProcess)

find_package(Boost COMPONENTS filesystem system program_options REQUIRED)
find_package(BoostProcess CONFIG REQUIRED)

add_executable(builder ${CMAKE_CURRENT_SOURCE_DIR}/builder.cpp)

target_link_libraries(builder Boost::filesystem Boost::system BoostProcess::boost_process Boost::program_options)

install(TARGETS builder RUNTIME DESTINATION bin)
