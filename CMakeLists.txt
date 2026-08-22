cmake_minimum_required(VERSION 3.10)
project(_DAEMON CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBCONFIGPP REQUIRED libconfig++)

add_executable(_daemon
    main.cpp
    fileexchange.cpp
)

target_link_libraries(_daemon PRIVATE ${LIBCONFIGPP_LIBRARIES})
target_include_directories(_daemon PRIVATE ${LIBCONFIGPP_INCLUDE_DIRS})
