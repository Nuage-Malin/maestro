#
# EPITECH PROJECT, 2022 clang-format.cmake File description: additional target
# to perform clang-format run, requires clang-format executable
#

# Execute with : `make clang-format -C build`

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.c
     ${PROJECT_SOURCE_DIR}/include/*.h)

# this was in the script I picked from
# https://arcanis.me/en/2015/10/17/cppcheck-and-clang-format, but I don't see
# why foreach (SOURCE_FILE ${ALL_SOURCE_FILES}) string(FIND ${SOURCE_FILE}
# ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND "") if (NOT
# ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1) list(REMOVE_ITEM ALL_SOURCE_FILES
# ${SOURCE_FILE}) endif () endforeach ()

find_program(CLANG_FORMAT NAMES clang-format)

if(NOT CLANG_FORMAT)
  execute_process(COMMAND pip install clang-format)
  find_program(CLANG_FORMAT NAMES clang-format)
endif()

if(CLANG_FORMAT)
  add_custom_target(
    clang-format COMMAND clang-format -style=file # will use .clang-format file
                         -i ${ALL_SOURCE_FILES})
endif()
