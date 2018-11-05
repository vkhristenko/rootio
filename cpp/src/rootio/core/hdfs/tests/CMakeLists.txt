file (GLOB SOURCES *.c)
foreach (SOURCE ${SOURCES})
    get_filename_component(EXE ${SOURCE} NAME_WE)
    list (APPEND EXECS ${EXE})
    add_executable(${EXE} ${SOURCE})
    target_link_libraries(${EXE} rootio-core rootio-core-hdfs)
endforeach(SOURCE)

# install generated artifacts
install(TARGETS ${EXECS}
        RUNTIME DESTINATION tests/core/hdfs)
