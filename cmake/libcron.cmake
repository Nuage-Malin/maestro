if (NOT DEFINED LIBCRON_INSTALLED)
    if (NOT EXISTS ${THIRD_PARTIES_DIR}/libcron/install OR
        NOT EXISTS ${THIRD_PARTIES_DIR}/libcron/install/include OR
        NOT EXISTS ${THIRD_PARTIES_DIR}/libcron/install/include/date OR
        NOT EXISTS ${THIRD_PARTIES_DIR}/libcron/install/include/libcron OR
        NOT EXISTS ${THIRD_PARTIES_DIR}/libcron/install/lib OR
        NOT EXISTS ${THIRD_PARTIES_DIR}/libcron/install/lib/liblibcron.a)
        message(STATUS "Installing libcron")
        execute_process(COMMAND cmake -S ${THIRD_PARTIES_DIR}/libcron -B ${THIRD_PARTIES_DIR}/libcron/build -DCMAKE_INSTALL_PREFIX=${THIRD_PARTIES_DIR}/libcron/install RESULTS_VARIABLE result)
        if (result)
            message(FATAL_ERROR "Failed to configure libcron {result}")
        endif ()

        execute_process(COMMAND make -C ${THIRD_PARTIES_DIR}/libcron/build RESULTS_VARIABLE result)
        if (result)
            message(FATAL_ERROR "Failed to build libcron {result}")
        endif ()

        execute_process(COMMAND make install -C ${THIRD_PARTIES_DIR}/libcron/build RESULTS_VARIABLE result)
        if (result)
            message(FATAL_ERROR "Failed to install libcron {result}")
        endif ()
    endif ()
    set(LIBCRON_INSTALLED true)

    include_directories(${THIRD_PARTIES_DIR}/libcron/install/include/)
    set(LIBCRON_LIBRARIES ${THIRD_PARTIES_DIR}/libcron/install/lib/liblibcron.a)
    target_link_libraries(${EXECUTABLE_NAME} ${LIBCRON_LIBRARIES})
endif ()