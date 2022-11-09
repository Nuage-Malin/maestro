#
# EPITECH PROJECT, 2022 clang-tidy.cmake File description: additional target to
# perform clang-tidy run, requires clang-tidy executable
#

# Execute with : `make clang-tidy -C build`

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES ${SRC_DIR}/*.cpp)

# message("ALL_SOURCE_FILES") message("${PROJECT_SOURCE_DIR}")
# message("${ALL_SOURCE_FILES}")

# this was in the script I picked from
# https://arcanis.me/en/2015/10/17/cppcheck-and-clang-tidy, but I don't see why
# foreach (SOURCE_FILE ${ALL_SOURCE_FILES}) string(FIND ${SOURCE_FILE}
# ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND "") if (NOT
# ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1) list(REMOVE_ITEM ALL_SOURCE_FILES
# ${SOURCE_FILE}) endif () endforeach ()

find_program(CLANG_TIDY NAMES clang-tidy)

if(NOT CLANG_TIDY)
  execute_process(COMMAND pip install clang-tidy)
  find_program(CLANG_TIDY NAMES clang-tidy)
endif()

if(CLANG_TIDY)
  add_custom_target(clang-tidy COMMAND clang-tidy ${ALL_SOURCE_FILES} -- -I
                                       include)
endif()
