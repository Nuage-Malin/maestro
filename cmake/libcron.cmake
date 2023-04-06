if (NOT DEFINED LIBCRON_INSTALLED)
    set(LIBCRON_INSTALLED true)

    message(STATUS "Installing libcron")
    execute_process(COMMAND cmake -S ${THIRD_PARTIES_DIR}/libcron -B ${THIRD_PARTIES_DIR}/libcron/build -DCMAKE_INSTALL_PREFIX=${THIRD_PARTIES_DIR}/libcron/install RESULTS_VARIABLE result)
    if (result)
        message(FATAL_ERROR "Failed to configure libcron {result}")
    endif()

    execute_process(COMMAND make -C ${THIRD_PARTIES_DIR}/libcron/build RESULTS_VARIABLE result)
    if (result)
        message(FATAL_ERROR "Failed to build libcron {result}")
    endif()

    execute_process(COMMAND make install -C ${THIRD_PARTIES_DIR}/libcron/build RESULTS_VARIABLE result)
    if (result)
        message(FATAL_ERROR "Failed to install libcron {result}")
    endif()

    include_directories(${THIRD_PARTIES_DIR}/libcron/install/include/)
    target_link_libraries(${EXECUTABLE_NAME} ${THIRD_PARTIES_DIR}/libcron/install/lib/liblibcron.a)
endif()