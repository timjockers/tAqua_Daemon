cmake_minimum_required(VERSION 3.10)
project(_DAEMON CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBCONFIGPP REQUIRED libconfig++)

add_executable(_daemon
    main.cpp
    fileexchange.cpp
    events.cpp
    io.cpp
)

target_link_libraries(_daemon PRIVATE ${LIBCONFIGPP_LIBRARIES})
target_include_directories(_daemon PRIVATE ${LIBCONFIGPP_INCLUDE_DIRS})


# Linux / Raspberry Pi
if(UNIX AND NOT APPLE)

    # Require libgpiod >= 2.0
    pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET
        libgpiod>=2.0
    )

    target_link_libraries(_daemon
        PRIVATE
            PkgConfig::GPIOD
    )

    target_compile_definitions(_daemon
        PRIVATE
            HAS_GPIOD
    )

endif()
