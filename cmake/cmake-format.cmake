#
# EPITECH PROJECT, 2022 clang-format.cmake File description: additional target
# to perform clang-format run, requires clang-format executable
#

# Execute with : `make cmake-format -C build`

# cmake-format will use .cmake-format.py for formatting rules

# get all project files
file(GLOB_RECURSE ALL_CMAKE_FILES ${PROJECT_SOURCE_DIR}/cmake/*.cmake
     ${PROJECT_SOURCE_DIR}/CMakeLists.txt)

find_program(CMAKE_FORMAT NAMES cmake-format)

if(NOT CMAKE_FORMAT)
  execute_process(COMMAND pip install cmakelang)
  find_program(CMAKE_FORMAT NAMES cmake-format)
endif()

if(CMAKE_FORMAT)
  add_custom_target(cmake-format COMMAND cmake-format -i ${ALL_CMAKE_FILES})
endif()
