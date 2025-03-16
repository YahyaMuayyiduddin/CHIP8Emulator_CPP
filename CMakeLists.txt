cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Set the project name and the required C++ standard
project(Chip8)
set(FOO_SRCS chip8.cpp)
# set(FOO_SRCS chip8.cpp instructions.cpp)
include_directories(${CMAKE_SOURCE_DIR}/lib/SDL3.framework/Headers)
# include_directories(${CMAKE_SOURCE_DIR}/include/Headers)
link_directories(${CMAKE_SOURCE_DIR}/lib/SDL3.framework/Headers)


add_executable(chip8 ${FOO_SRCS})

find_package(SDL3 REQUIRED)

target_link_libraries(chip8 SDL3::SDL3)

# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


#
# cmake_minimum_required(VERSION 3.10)



# # Set the project name and the required C++ standard
# project(Chip8)
# set(FOO_SRCS chip8.cpp)
# include_directories(${CMAKE_SOURCE_DIR}/lib/SDL3.framework/Headers)
# include_directories(${CMAKE_SOURCE_DIR}/include/Headers)
# link_directories(${CMAKE_SOURCE_DIR}/lib/SDL3.framework/Headers)


# add_executable(chip8 ${FOO_SRCS})

# find_package(SDL3 REQUIRED)

# target_link_libraries(chip8 SDL3::SDL3)

# # set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")





